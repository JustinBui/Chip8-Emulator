#ifndef CHIP8STACK_H
#define CHIP8STACK_H

#include "config.h"

struct chip8;

struct chip8_stack {
    // The stack has 16 16-bit values (Hence, array of unsigned shorts, where each address = 2 bytes)
    unsigned short stack[CHIP8_TOTAL_STACK_DEPTH];
}

#endif