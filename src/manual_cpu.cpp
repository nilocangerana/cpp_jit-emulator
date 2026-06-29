#include "manual_cpu.h"
#include "memory.h"

#include <windows.h>
#include <iostream>

Manual_CPU::Manual_CPU(Memory& mem) : memory(mem) 
{

}

//Executes.
void Manual_CPU::Run(){
    size_t pc = 0;

    while(true) {
        //Check if code is already compiled at this (pc). If true, reuse, if false, compile.
        if(cache.find(pc) == cache.end()){
            cache[pc] = CompileBlock(pc);
        }

        JitFunc func = cache[pc]; //Get the cached Jit compiled code.

        int result = func(); //Executes.

        std::cout << "Result (A): " << result << std::endl;
    }
}

//Allocates "size" of memory(Bytes). Windows OS API, OS gives a block o memory.
//PAGE_EXECUTE_READWRITE: memory is data + executable code
void* Manual_CPU::AllocateExecutableMemory(size_t size){
    return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

//Compile the instructions into a JitFunc. Compile a block of instructions starting at PC until stop point. Converts local opcode to actual machine opcode instructions.
JitFunc Manual_CPU::CompileBlock(size_t pc){
    //Allocates memory for the code, cursor starts at the start of memory.
    uint8_t* code = (uint8_t*)AllocateExecutableMemory(1024);
    uint8_t* cursor = code;

    //Read opcode -> generate machine code.
    while(true){
        uint8_t opcode = memory.Read(pc++); //read instruction and increment later.

        switch(opcode){
            case 0x01:{ //LOAD A, value
                uint8_t value = memory.Read(pc++); //read value and increment later.
                                      
                //mov eax, value == (0x01 5 -> mov eax, 5) . eax = x86 CPU register. On x86 [mov eax, imm32] encode as [B8 xx xx xx xx]
                *cursor++ = 0xB8; //[B8][2?][3?][4?][5?] - cursor at 2. Dereferences cursor, Writes 0xB8 at current cursor position, increment cursor by 1 byte later.
                /* *cursor = 0xB8;
                   cursor++; */

                *(uint32_t*)cursor = value; //[B8][05][3?][4?][5?] - cursor at 2. Treat cursor as 32bit integer (uint32_t*)cursor, dereference it. Writes a 4-byte integer(value) into memory, starting at cursor.
                cursor += 4; //Advance cursor by 4 -> [B8][05][00][00][00] [6?] - cursor at 6.
                break; //Generates [B8][05][00][00][00]
            }

            case 0x02: { //ADD A, value
                uint8_t value = memory.Read(pc++); //read value

                //add eax, value
                *cursor++ = 0x05; //[add eax, imm32] encode as [05 xx xx xx xx]
                *(uint32_t*)cursor = value; //[05 07 00 00 00]
                cursor+=4; //advances cursor by 4
                break; //Generates [05][07][00][00][00]
            }

            case 0xFF: { //HALT
                *cursor++ = 0xC3; //[ret] encode as [C3]. // return value goes onto eax.
                goto done;
            }

            default:
                *cursor++ = 0xC3; //[ret] encode as [C3]
                goto done;
        }
    }

    done:
        return (JitFunc)code;
}