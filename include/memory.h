#pragma once

#include <vector>

class Memory{
    public:
        void DefineRomMemory(const std::vector<uint8_t>& memory);

        uint8_t Read(size_t address) const;
        void Write(size_t address, uint8_t value); // optional but useful later

    private:
        //uint8_t -> unsigned int, range: 0 -> 255 == 1 byte != (int == 4 bytes)
        std::vector<uint8_t> rom_memory; //Array of bytes, representing the emulator RAM. Stores 0xA9, 0x42 (instructions or data)
};