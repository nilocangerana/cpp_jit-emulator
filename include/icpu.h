#pragma once

//defining int(*)() as JitFunc. Pointer to a func param(). returns int.
using JitFunc = int(*)();

class ICPU{
    public:
        virtual ~ICPU() = default;

        virtual void* AllocateExecutableMemory(size_t size) = 0;
        virtual JitFunc CompileBlock(size_t pc) = 0;

        virtual void Run() = 0;
};