#include "emulator_cpu.h"
#include "memory.h"

#include <windows.h>
#include <iostream>

Emulator_CPU::Emulator_CPU(Memory& mem) : memory(mem) 
{

}

//Executes.
void Emulator_CPU::Run(){
    size_t pc = 0;

    while(true) {
        //Check if code is already compiled at this (pc). If true, reuse, if false, compile.
        if(cache.find(pc) == cache.end()){
            cache[pc] = CompileBlock(pc);
        }

        JitFunc func = cache[pc]; //Get the cached Jit compiled code.

        pc = func(&registers); //Executes.
        
        std::cout << "[CPU LOOP] Current EAX: " << registers.eax << std::endl;
        if(pc == SIZE_MAX) break; // HALT signal
    }

    std::cout << "[END] Final EAX: " << registers.eax << std::endl;
}

//Allocates "size" of memory(Bytes). Windows OS API, OS gives a block o memory.
//PAGE_EXECUTE_READWRITE: memory is data + executable code
void* Emulator_CPU::AllocateExecutableMemory(size_t size){
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

namespace Emit {
    //inline = compiler can replace this function call with its body, no function call overhead
    inline void MovRaxImm64(uint8_t*& cursor, uint64_t value) {
        *cursor++ = 0x48; // REX.W, “Use 64-bit operand size”, [48 B8 xx xx xx xx xx xx xx xx]
        *cursor++ = 0xB8; // mov rax, imm64
        *(uint64_t*)cursor = value; //value == imm64
        cursor += 8; //8 because its 8 bytes, uint64_t
    }

    inline void Ret(uint8_t*& cursor) {
        *cursor++ = 0xC3;
    }

    inline void Return(uint8_t*& cursor, uint64_t value) {
        MovRaxImm64(cursor, value);
        Ret(cursor);
    }
}

//Compile the instructions into a JitFunc. Compile a block of instructions starting at PC until stop point. Converts local opcode to actual machine opcode instructions.
JitFunc Emulator_CPU::CompileBlock(size_t pc){
    //Allocates memory for the code, cursor starts at the start of memory.
    uint8_t* code = (uint8_t*)AllocateExecutableMemory(1024);
    uint8_t* cursor = code;

    //Read opcode -> generate machine code.
    while(true){
        uint8_t opcode = memory.Read(pc++); //read instruction and increment later.

        switch(opcode){
            case 0x00: { // NOP
                //1 bytes, 1 opcode
                // do nothing
                Emit::Return(cursor, pc);
                goto done;
            }

            case 0x01:{ //LOAD A, value. 
                //2 bytes, 1 opcode, 1 value
                //on x64: RCX = pointer to CPURegisters* registers (func 1st param)
                //instruction: mov dword ptr [rcx], imm32 -> registers.eax = value;
                int8_t value = (int8_t)memory.Read(pc++); //read value and increment later.
                int32_t extended = value;
                
                //Every *cursor++ = X writes one byte into executable memory and move the cursor.
                *cursor++ = 0xC7; // opcode. Dereferences cursor, Writes 0xC7 at current cursor position, increment cursor by 1 byte later.
                *cursor++ = 0x01; // modrm: [rcx]

                memcpy(cursor, &extended, 4);
                cursor += 4;
                //Generates [C7 01 05 00 00 00] == mov dword ptr [rcx], 5.  [C7 01 FB FF FF FF] for -5
                
                Emit::Return(cursor, pc);
                goto done;
            }

            case 0x02: { //ADD A, value
                //2 bytes, 1 opcode, 1 value
                //instruction: add dword ptr [rcx], imm32 -> registers.eax += value;
                int8_t value = (int8_t)memory.Read(pc++); //read value

                *cursor++ = 0x83; // opcode
                *cursor++ = 0x01; // modrm: [rcx]
                *cursor++ = (uint8_t)value;
                //Generates [81 01 07 xx xx xx] == add dword ptr [rcx], 7
                
                Emit::Return(cursor, pc);
                goto done;
            }

            case 0x10: { //JMP addr, jump to address value (only instruction jump valid)
                //2 bytes, 1 opcode, 1 address
                uint8_t addr = memory.Read(pc++); //read value
                //mov rax, addr -> ret. Return the address PC.
                Emit::Return(cursor, addr);
                goto done;
            }

            case 0x11: { //JZ addr, jump if zero. If the result is zero, jump to address X
                //2 bytes, 1 opcode, 1 address
                uint8_t addr = memory.Read(pc++);
                size_t nextPc = pc;

                // cmp dword ptr [rcx], 0
                *cursor++ = 0x83; // opcode for cmp r/m32, imm8
                *cursor++ = 0x39; // modrm: [rcx]
                *cursor++ = 0x00; // imm8 = 0

                // je +offset (we will patch this)
                *cursor++ = 0x0F; // JE rel32 -> je +offset. Jump forward if 0 flag is set. Jump to (next_instruction + offset)
                *cursor++ = 0x84;
                uint32_t* jumpOffset = (uint32_t*)cursor;
                cursor += 4;

                // NOT ZERO path:
                Emit::Return(cursor, nextPc);

                // ZERO path:
                uint8_t* zeroLabel = cursor;
                Emit::Return(cursor, addr);

                *jumpOffset = (uint32_t)(zeroLabel - (uint8_t*)(jumpOffset + 1));

                goto done;
            }

            case 0xFF: { //HALT, Stop code
                //1 bytes, 1 opcode
                //mov rax, SIZE_MAX . x64 always returns on RAX. so move size_max to rax and return.
                //ret
                Emit::Return(cursor,  SIZE_MAX);
                goto done;
            }

            default:
                std::cout << "Unknown opcode at pc: " << pc-1 << std::endl;
                //mov rax, SIZE_MAX . x64 always returns on RAX. so move size_max to rax and return.
                //ret
                Emit::Return(cursor,  SIZE_MAX);
                goto done;
        }
    }

    done:
        return (JitFunc)code;
}