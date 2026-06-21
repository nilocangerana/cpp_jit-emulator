#include "emulator.h"

int main() {
    //Fake Rom memory = [0x01][0x05][0x02][0x07][0xFF]
    std::vector<uint8_t> rom = {
        0x01, 10, // LOAD A, 10
        0x02, 7, // ADD A, 7
        0xFF
    }; //lvalue

    Emulator emulator{std::move(rom)};

    emulator.Run();

    return 0;
}