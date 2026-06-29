#include "emulator.h"

int main() {
    //Fake Rom memory = [0x01][0x05][0x02][0x07][0xFF]
    /*std::vector<uint8_t> rom = {
        0x01, 10, // LOAD A, 10
        0x02, 7, // ADD A, 7
        0xFF
    }; //lvalue
    */

    std::vector<uint8_t> rom = {
        0x01, 10, // LOAD A, 10
        0x02, (uint8_t)-1, // ADD A, -1
        0x11, 8, //JZ -> HALT (addr==8)
        0x10, 2, //JMP -> ADD A, -1 (addr==2)
        0xFF
    };

    Emulator emulator{std::move(rom)};

    emulator.Run();

    return 0;
}