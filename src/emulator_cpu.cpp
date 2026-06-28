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

        if(pc == SIZE_MAX) break; // HALT signal
    }

    std::cout << "Final EAX: " << registers.eax << std::endl;
}

//Allocates "size" of memory(Bytes). Windows OS API, OS gives a block o memory.
//PAGE_EXECUTE_READWRITE: memory is data + executable code
void* Emulator_CPU::AllocateExecutableMemory(size_t size){
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

//inline = compiler can replace this function call with its body, no function call overhead
inline void EmitMovRaxImm64(uint8_t*& cursor, uint64_t value) {
    *cursor++ = 0x48; // REX.W, “Use 64-bit operand size”, [48 B8 xx xx xx xx xx xx xx xx]
    *cursor++ = 0xB8; // mov rax, imm64
    *(uint64_t*)cursor = value; //value == imm64
    cursor += 8; //8 because its 8 bytes, uint64_t
}

inline void EmitRet(uint8_t*& cursor) {
    *cursor++ = 0xC3;
}

inline void EmitJitFuncReturn(uint8_t*& cursor, uint64_t value) {
    EmitMovRaxImm64(cursor, value);
    EmitRet(cursor);
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
            case 0x01:{ //LOAD A, value. 
                //on x64: RCX = pointer to CPURegisters* registers (func 1st param)
                //instruction: mov dword ptr [rcx], imm32 -> registers.eax = value;
                uint8_t value = memory.Read(pc++); //read value and increment later.
                
                *cursor++ = 0xC7; // opcode. Dereferences cursor, Writes 0xC7 at current cursor position, increment cursor by 1 byte later.
                *cursor++ = 0x01; // modrm: [rcx]
                *(uint32_t*)cursor = value;
                cursor += 4;
                //Generates [C7 01 05 00 00 00] == mov dword ptr [rcx], 5
                
                EmitJitFuncReturn(cursor, pc);
                goto done;
            }

            case 0x02: { //ADD A, value
                //instruction: add dword ptr [rcx], imm32 -> registers.eax += value;
                uint8_t value = memory.Read(pc++); //read value

                *cursor++ = 0x81; // opcode
                *cursor++ = 0x01; // modrm: [rcx]
                *(uint32_t*)cursor = value;
                cursor += 4;
                //Generates [81 01 07 xx xx xx] == add dword ptr [rcx], 7
                
                EmitJitFuncReturn(cursor, pc);
                goto done;
            }

            case 0xFF: { //HALT
                //mov rax, SIZE_MAX . x64 always returns on RAX. so move size_max to rax and return.
                //ret
                EmitJitFuncReturn(cursor,  SIZE_MAX);
                goto done;
            }

            case 0x00: { // NOP
                // do nothing
                EmitJitFuncReturn(cursor, pc);
                goto done;
            }

            default:
                std::cout << "Unknown opcode at pc: " << pc-1 << std::endl;
                //mov rax, SIZE_MAX . x64 always returns on RAX. so move size_max to rax and return.
                //ret
                EmitJitFuncReturn(cursor,  SIZE_MAX);
                goto done;
        }
    }

    done:
        return (JitFunc)code;
}