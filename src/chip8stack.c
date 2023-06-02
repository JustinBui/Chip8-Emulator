#include "chip8stack.h"
#include "chip8.h"
#include <assert.h>

// Index of the stack starts at 0 (Similar to that of an array)
//      For instance, if we push the first element to the stack, the first element is indexed 0
//      If we push the second element to the stack, the second element is indexed 1
//      Etc...

static void chip8_stack_in_bounds(struct chip8* chip8) {
    // NOTE: The Max value of the stack is 15 (Since there are 16 total addresses on the CHIP8 stack), and stack is 0-indexed
    // Also, we do not need to check if the stack pointer is < 0, since it's data type is unsigned
    assert(chip8->registers.SP < CHIP8_TOTAL_STACK_DEPTH);
}

void chip8_stack_push(struct chip8* chip8, unsigned short val) {
    chip8_stack_in_bounds(chip8);

    // Stack pointer points to the first available memory location on top of the stack, so assign the input val to that address
    chip8->stack.stack[chip8->registers.SP] = val; 

    chip8->registers.SP += 1; // Increment the stack pointer
}

unsigned short chip8_stack_pop(struct chip8* chip8) {
    chip8->registers.SP -= 1; // Decrement the stack pointer

    chip8_stack_in_bounds(chip8);

    unsigned short val = chip8->stack.stack[chip8->registers.SP]; 

    return val; // Return value that was popped
}