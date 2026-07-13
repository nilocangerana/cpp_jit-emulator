#include "emulator.h"

int main() {
    //Fake Rom memory = [0x01][0x05][0x02][0x07][0xFF]
    /*std::vector<uint8_t> rom = {
        0x01, 5, // LOAD A, 5
        0x02, 7, // ADD A, 7
        0xFF
    }; //lvalue
    */

    //Fake Rom memory = [0x01][0x01][0x0A] [0x02][0x01][0xFF] [0x11][0x01][0x0B] [0x10][0x03] [0xFF]
    std::vector<uint8_t> rom = {
        0x01, REG_EBX, 10, // LOAD reg, 10
        0x02, REG_EBX, (uint8_t)-1, // ADD reg, -1
        0x11, REG_EBX, 11, //JZ -> HALT (addr==11)
        0x10, 3, //JMP -> ADD reg, -1 (addr==3)
        0xFF
    };

    Emulator emulator{std::move(rom)};

    emulator.Run();

    return 0;
}