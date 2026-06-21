#include "emulator.h"
#include "memory.h"

//Copy of rom_memory is made inside the emulator constructor.
//move: transforms into rvalue, Memory class receives a value.
Emulator::Emulator(std::vector<uint8_t> rom_memory) 
: memory(std::move(rom_memory)), cpu(memory)
{

}

void Emulator::Run()
{
    cpu.Run();
}