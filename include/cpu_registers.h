#pragma once
#include <cstdint>

enum RegisterIndex {
    REG_EAX = 0,
    REG_EBX = 1,
    REG_ECX = 2,
    REG_EDX = 3
};

struct CPURegisters {
    uint32_t regs[4] = {0};

    uint32_t& EAX() { return regs[REG_EAX]; }
    uint32_t& EBX() { return regs[REG_EBX]; }
    uint32_t& ECX() { return regs[REG_ECX]; }
    uint32_t& EDX() { return regs[REG_EDX]; }
};