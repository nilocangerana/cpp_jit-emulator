#pragma once

#include "icpu.h"
#include <unordered_map>

class Memory;

class Emulator_CPU : public ICPU{
    public:
        explicit Emulator_CPU(Memory& mem);
        void Run() override;
        
    private:
        Memory& memory;

        //size_t -> unsigned int, range 4(32-bit arch.) / 8 bytes(64-bit arch.) / holds maximum value.
        std::unordered_map<size_t, JitFunc> cache;

        void* AllocateExecutableMemory(size_t size) override;
        JitFunc CompileBlock(size_t pc);

        CPURegisters registers;
};