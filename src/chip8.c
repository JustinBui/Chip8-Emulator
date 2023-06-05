#include "chip8.h"
#include "chip8screen.h"
#include<memory.h>
#include <assert.h>
#include <stdio.h>

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
    
    // Initialize chip8 memory - Loading character set into chip8 memory
    memcpy(&chip8->memory.memory, chip8_default_character_set, sizeof(chip8_default_character_set));
}

void chip8_load(struct chip8* chip8, const char* buffer, size_t size) {
    // If the size of the given program is over the CHIP8 memory size, then it throws an error
    // (NOTE: We add size with CHIP8_PROGRAM_LOAD_ADDRESS because mist chip-8 programs start at memory location 0x200)
    assert(size+CHIP8_PROGRAM_LOAD_ADDRESS < CHIP8_MEMORY_SIZE);

    memcpy(&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDRESS], buffer, size);     // Loading the buffer source to chip8 mmeory

    chip8->registers.PC = CHIP8_PROGRAM_LOAD_ADDRESS;   // Setting the program counter to the starting load address
}


/*******************************************************************
* nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
* n or nibble - A 4-bit value, the lowest 4 bits of the instruction
* x - A 4-bit value, the lower 4 bits of the high byte of the instruction
* y - A 4-bit value, the upper 4 bits of the low byte of the instruction
* kk or byte - An 8-bit value, the lowest 8 bits of the instruction
*******************************************************************/
static void chip8_exec_extended(struct chip8* chip8, unsigned short opcode) {
    // Ex: 2nnn
    unsigned short nnn = opcode & 0x0fff;

    // Ex: 3xkk
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char kk = opcode & 0x00ff;
    unsigned char y = (opcode >> 4) & 0x000f;

    switch(opcode & 0xf000) {
        // JP addr - 1nnn jump to location nnn's
        case 0x1000:
            chip8->registers.PC = nnn;
        break;

        // CALL addr - 2nn Call subroutine at location nnn
        case 0x2000:
            chip8_stack_push(chip8, chip8->registers.PC); // Push program counter to the stack
            chip8->registers.PC = nnn; // PC is set to nnn
        break;

        // SE Vx, byte - 3xkk Skip next instruction if Vx=kk
        case 0x3000:
            if (chip8->registers.V[x] == kk) {
                chip8->registers.PC += 2; // Each instruction in CHIP8 is 2 bytes, therefore skip by 2
            }
        break;

        // SNE Vx, byte - Skip next instruction if Vx != kk.
        case 0x4000:
            if (chip8->registers.V[x] != kk) {
                chip8->registers.PC += 2; // Each instruction in CHIP8 is 2 bytes, therefore skip by 2
            }
        break;

        // SE Vx, Vy - Skip the next instruction if V[x] == V[y]
        case 0x5000:
            if (chip8->registers.V[x] == chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
        break;

        // LD Vx, byte - Set Vx = kk.
        case 0x6000:
            chip8->registers.V[x] = kk;
        break;

        // ADD Vx, byte - Set Vx = Vx + kk
        case 0x7000:
            chip8->registers.V[x] += kk;
        break;

        
    }
}

// Function to execute a specific instruction set
// ==> Each opcode is 8 bytes long in CHIP8, therefore it is unsigned short
void chip8_exec(struct chip8* chip8, unsigned short opcode) {
    switch(opcode) {
        case 0x00E0: // Clear the display.
            chip8_screen_clear(&chip8->screen);
        break;

        case 0x00EE: // Return from a subroutine.
            chip8->registers.PC = chip8_stack_pop(chip8);
        break;

        default: // Special case instructions where bitwise operators needa be done
            chip8_exec_extended(chip8, opcode);
    }
}