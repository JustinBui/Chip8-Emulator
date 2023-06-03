#ifndef CHIP8_H
#define CHIP8_H

#include "chip8.h"
#include "chip8memory.h"
#include "chip8registers.h"
#include "chip8stack.h"
#include "chip8keyboard.h"

// Anything regarding chip8 internals go here: Memory, registers, screen pixels, keyboard, etc...
struct chip8 {
    struct chip8_memory memory;
    struct chip8_stack stack;
    struct chip8_registers registers;
    struct chip8_keyboard keyboard;
};

void chip8_init(struct chip8* chip8);


#endif