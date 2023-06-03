#include "chip8.h"
#include<memory.h>


void chip8_init(struct chip8* chip8) {
    memset(chip8, 0, sizeof(struct chip8)); // Setting all values to null
}