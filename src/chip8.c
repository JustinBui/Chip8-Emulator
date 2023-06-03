#include "chip8.h"
#include<memory.h>

// Chip8 draws graphics on the screen through the use of SPRITES - Group of bytes which are binary is representation
// of the desired picture. Chip-8 sprites are up to 15 bytes (8x15 pixels)
// ==> Programs may also refer to a group of sprites representing the hexadecimal digits 0 through F. These sprites are 5 bytes long, or 8x5 pixels.
// ==> The data should be stored in the interpreter area of Chip-8 memory (0x000 to 0x1FF)

const char chip8_default_character_set[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,   // 0 displayed on screen
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xf0, 0x10, 0xf0, 0x80, 0xf0,   // 2
    0xf0, 0x10, 0xf0, 0x10, 0xf0,   // 3
    0x90, 0x90, 0xf0, 0x10, 0x10,   // 4
    0xf0, 0x80, 0xf0, 0x10, 0xf0,   // 5
    0xf0, 0x80, 0xf0, 0x90, 0xf0,   // 6
    0xf0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xf0, 0x90, 0xf0, 0x90, 0xf0,   // 8
    0xf0, 0x90, 0xf0, 0x10, 0xf0,   // 9
    0xf0, 0x90, 0xf0, 0x90, 0x90,   // A
    0xe0, 0x90, 0xe0, 0x90, 0xe0,   // B
    0xf0, 0x80, 0x80, 0x80, 0xf0,   // C
    0xe0, 0x90, 0x90, 0x90, 0xe0,   // D
    0xf0, 0x80, 0xf0, 0x80, 0xf0,   // E
    0xf0, 0x80, 0xf0, 0x80, 0x80    // F

};

void chip8_init(struct chip8* chip8) {
    // Setting all values to null
    memset(chip8, 0, sizeof(struct chip8));
    
    // Initialize chip8 memory
    memcpy(&chip8->memory.memory, chip8_default_character_set, sizeof(chip8_default_character_set));
}