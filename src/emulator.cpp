#include "emulator.h"

Emulator::Emulator(const std::vector<uint8_t>& rom_memory){
    //Fake Rom memory = [0x01][0x05][0x02][0x07][0xFF]
    memory.DefineRomMemory(rom_memory);

    cpu.SetMemory(&memory);
}

void Emulator::Run()
{
    cpu.Run();
}