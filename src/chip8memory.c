#include "chip8memory.h"
#include <assert.h>

static void chip8_is_memory_in_bounds(int index) {
    assert(index >= 0 && index < CHIP8_MEMORY_SIZE);
}

void chip8_memory_set(struct chip8_memory* memory, int index, unsigned char val) {
    chip8_is_memory_in_bounds(index);

    // Setting memory
    memory->memory[index] = val;
}

unsigned char chip8_memory_get(struct chip8_memory* memory, int index) {
    chip8_is_memory_in_bounds(index);
    
    // Getting memory
    return memory->memory[index];
}

unsigned short chip8_memory_get_short(struct chip8_memory* memory, int index) {
    // Getting each byte of memory
    unsigned char byte1 = chip8_memory_get(memory, index);
    unsigned char byte2 = chip8_memory_get(memory, index + 1);

    // Merging these 2 unsigned chars together, thus making it an unsigned short, since it's now 2 bytes.
    // This allows us to read 2 bytes of memory.
    return byte1 << 8 | byte2;
}