#include "memory.h"
#include <stdexcept>

Memory::Memory(std::vector<uint8_t> rom_instructions) 
: rom_memory(std::move(rom_instructions))
{

}

uint8_t Memory::Read(size_t address) const {
    if (address >= rom_memory.size()) {
        throw std::out_of_range("Memory read out of bounds");
    }
    return rom_memory[address];
}

void Memory::Write(size_t address, uint8_t value) {
    if (address >= rom_memory.size()) {
        throw std::out_of_range("Memory write out of bounds");
    }
    rom_memory[address] = value;
}