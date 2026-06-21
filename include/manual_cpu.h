#pragma once

#include "icpu.h"
#include <unordered_map>

class Memory;

class Manual_CPU : public ICPU{
    public:
        void Run() override;
        
        void SetMemory(Memory* mem);
    private:
        Memory* memory = nullptr;

        //size_t -> unsigned int, range 4(32-bit) to 8 bytes(64-bit)
        std::unordered_map<size_t, JitFunc> cache;

        void* AllocateExecutableMemory(size_t size) override;
        JitFunc CompileBlock(size_t pc) override;
};  