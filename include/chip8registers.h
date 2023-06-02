#ifndef CHIP8REGISTERS_H
#define CHIP8REGISTERS_H

#include "config.h"

// --------------- List of Registers CHIP8 uses: ---------------
// ==> 16 general purpose registers (Each 8-bit): Refered to as Vx, where x is a hex digit from 0 - F
// ==> I register (16-bits): Used to store memory addresses
// ==> Delay and timer registers (Each 8-bit): When these registers are non-zero, they are decremented at 60Hz 
// ==> Program counter (PC) (16-bit): Holds the memory address of the current instruction to be executed in a program (Pseudo Register)
// ==> Stack pointer (8-bit) used to point to the top of the stack (Also a pseudo register)
// -------------------------------------------------------------

struct chip8_registers {
    unsigned char V[CHIP8_TOTAL_DATA_REGISTERS]; // 1 byte = 8 bits
    unsigned short I; // 2 bytes = 16 bits
    unsigned char delay_timer; // 1 byte = 8 bits
    unsigned char sound_timer; // 1 byte = 8 bits
    unsigned short PC; // 2 bytes = 16 bits
    unsigned short SP; // 2 bytes = 16 bits
};

#endif