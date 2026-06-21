#include "emulator.h"

int main() {
    std::vector<uint8_t> rom = {
        0x01, 10, // LOAD A, 10
        0x02, 7, // ADD A, 7
        0xFF
    };

    Emulator emulator{rom};

    emulator.Run();

    return 0;
}