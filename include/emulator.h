#pragma once

#include "emulator_cpu.h"
#include "memory.h"
#include "manual_cpu.h"

// [main] rom (lvalue)
//    ↓ move
// [Emulator param] rom_memory (rvalue)
//    ↓ move
// [Memory param] rom_instructions (rvalue)
//    ↓ move
// [Memory member] rom_memory  FINAL OWNER, transfered ownership from main.
class Emulator{
    public:
        explicit Emulator(std::vector<uint8_t> rom_memory);

        void Run();

        private:
            Memory memory;
            //Manual_CPU cpu;
            Emulator_CPU cpu;
};