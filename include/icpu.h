#pragma once

#include "cpu_registers.h"

//defining int(*)() as JitFunc. Pointer to a func param(). returns int.
using Manual_JitFunc = int(*)();
using JitFunc = size_t(*)(CPURegisters*);

class ICPU{
    public:
        virtual ~ICPU() = default;

        virtual void* AllocateExecutableMemory(size_t size) = 0;

        //Compiling Blocks.
        //virtual JitFunc CompileBlock(size_t pc) = 0; //emulate registers
        //virtual Manual_JitFunc Manual_CompileBlock(size_t pc) = 0; //real register assignments

        virtual void Run() = 0;
};