#pragma once

using JitFunc = int(*)(); //defining int(*)() as JitFunc. Pointer to a no param function. returns int.

class ICPU{
    public:
        virtual ~ICPU() = default;

        virtual void* AllocateExecutableMemory(size_t size) = 0;
        virtual JitFunc CompileBlock(size_t pc) = 0;

        virtual void Run() = 0;
};