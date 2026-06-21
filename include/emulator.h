#pragma once

#include "memory.h"
#include "manual_cpu.h"

class Emulator{
    public:
        explicit Emulator(const std::vector<uint8_t>& rom_memory);

        void Run();

        private:
            Memory memory;
            Manual_CPU cpu;
};