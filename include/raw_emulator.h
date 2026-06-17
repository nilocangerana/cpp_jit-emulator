#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>

using JitFunc = int(*)(); //defining int(*)() as JitFunc. Pointer to a no param function. returns int.

class Raw_Emulator{
    public:
        Raw_Emulator();

        void Run();
    
    private:
        //uint8_t -> unsigned int, range: 0 -> 255 == 1 byte != (int == 4 bytes)
        //size_t -> unsigned int, range 4(32-bit) to 8 bytes(64-bit)
        std::vector<uint8_t> memory; //Array of bytes, representing the emulator RAM. Stores 0xA9, 0x42 (instructions or data)
        std::unordered_map<size_t, JitFunc> cache;

        void* AllocateExecutableMemory(size_t size);
        JitFunc CompileBlock(size_t pc);
};