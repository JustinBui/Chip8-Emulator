#include "chip8.h"
#include "chip8screen.h"
#include "SDL2/SDL.h"
#include "chip8keyboard.h"

#include<memory.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// Chip8 draws graphics on the screen through the use of SPRITES - Group of bytes which are binary is representation
// of the desired picture. Chip-8 sprites are up to 15 bytes (8x15 pixels)
// ==> Programs may also refer to a group of sprites representing the hexadecimal digits 0 through F. These sprites are 5 bytes long, or 8x5 pixels.
// ==> The data should be stored in the interpreter area of Chip-8 memory (0x000 to 0x1FF)

// List of character sets to be displayed on the screen
const char chip8_default_character_set[] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,   // 0 (Location: 0x00)
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1 (Location: 0x05)
    0xf0, 0x10, 0xf0, 0x80, 0xf0,   // 2 (Location: 0x0A)
    0xf0, 0x10, 0xf0, 0x10, 0xf0,   // 3 (Location: 0x0F)
    0x90, 0x90, 0xf0, 0x10, 0x10,   // 4 (Location: 0x14)
    0xf0, 0x80, 0xf0, 0x10, 0xf0,   // 5 (Location: 0x19)
    0xf0, 0x80, 0xf0, 0x90, 0xf0,   // 6 (Location: 0x1E)
    0xf0, 0x10, 0x20, 0x40, 0x40,   // 7 (Location: 0x23)
    0xf0, 0x90, 0xf0, 0x90, 0xf0,   // 8 (Location: 0x28)
    0xf0, 0x90, 0xf0, 0x10, 0xf0,   // 9 (Location: 0x2D)
    0xf0, 0x90, 0xf0, 0x90, 0x90,   // A (Location: 0x32)
    0xe0, 0x90, 0xe0, 0x90, 0xe0,   // B (Location: 0x37)
    0xf0, 0x80, 0x80, 0x80, 0xf0,   // C (Location: 0x3C)
    0xe0, 0x90, 0x90, 0x90, 0xe0,   // D (Location: 0x41)
    0xf0, 0x80, 0xf0, 0x80, 0xf0,   // E (Location: 0x46)
    0xf0, 0x80, 0xf0, 0x80, 0x80    // F (Location: 0x4B)

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

// 8xy0, 8xy1, 8xy2, 8xy3, ..., 8xy7, 8xyE
static void chip8_exec_extended_eight(struct chip8* chip8, unsigned short opcode) {
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char y = (opcode >> 4) & 0x000f;
    unsigned char final_four_bits = opcode & 0x000f;
    unsigned short tmp = 0;

    switch(final_four_bits) {
        // 8xy0 - LD Vx, Vy - Set Vx = Vy.
        case 0x00:
            chip8->registers.V[x] = chip8->registers.V[y];
        break;

        // 8xy1 - OR Vx, Vy - Performs a bitwise OR on Vx and Vy, stores results in Vx
        case 0x01:
            chip8->registers.V[x] = chip8->registers.V[x] | chip8->registers.V[y];
        break;

        // 8xy2 - AND Vx, Vy - Performs a bitwise AND on Vx and Vy, stores results on Vx
        case 0x02:
            chip8->registers.V[x] = chip8->registers.V[x] & chip8->registers.V[y];
        break;

        // 8xy3 - XOR Vx, Vy - Performs a bitwise XOR on Vx and Vy, stores results on Vx
        case 0x03:
            chip8->registers.V[x] = chip8->registers.V[x] ^ chip8->registers.V[y];
        break;

        // 8xy4 - ADD Vx, Vy - The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
        // ==> Only the lowest 8 bits of the result are kept, and stored in Vx.
        case 0x04:
            tmp = chip8->registers.V[x] + chip8->registers.V[y];
            chip8->registers.V[0x0f] = tmp > 0xff ? true : false; // Setting our carry flag
            chip8->registers.V[x] = tmp;
        break;
        
        // 8xy5 - SUB Vx, Vy - If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
        case 0x05:
            chip8->registers.V[0x0f] = chip8->registers.V[x] > chip8->registers.V[x] ? true : false;
            chip8->registers.V[x] = chip8->registers.V[x] - chip8->registers.V[y];
        break;

        // 8xy6 - SHR Vx {, Vy} - If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
        case 0x06:
            chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x01; 
            chip8->registers.V[x] /= 2;
        break;

        // 8xy7 - SUBN Vx, Vy - If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
        case 0x07:
            chip8->registers.V[0x0f] = chip8->registers.V[y] > chip8->registers.V[x]; 
            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
        break;

        // 8xyE - SHL Vx {, Vy} - If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
        case 0x0E:
            chip8->registers.V[0x0f] = chip8->registers.V[x] & 0x80;
            chip8->registers.V[x] *= 2;
        break;
    }
}

static char chip8_wait_for_key_press(struct chip8* chip8) {
    SDL_Event event;

    while(SDL_WaitEvent(&event)) {
        if (event.type != SDL_KEYDOWN)
            continue;
        
        char c = event.key.keysym.sym;  // Raw key data being pressed
        char chip8_key = chip8_keyboard_map(&chip8->keyboard, c);
        if (chip8_key != -1) {
            return chip8_key;
        }
    }

    return -1;
}

static void chip8_exec_extended_F(struct chip8* chip8, unsigned short opcode) {
    unsigned char x = (opcode >> 8) & 0x000f;

    switch (opcode & 0x00FF) {
        // Fx07 - LD Vx, DT - Set Vx to the delay timer value
        case 0x07:
            chip8->registers.V[x] = chip8->registers.delay_timer;
        break;

        // Fx0A - LD Vx, K - Wait for a key press, store the value of the key in Vx.
        case 0x0A:
        {
            char pressed_key = chip8_wait_for_key_press(chip8);
            chip8->registers.V[x] = pressed_key;
        } 
        break;

        // Fx15 - LD DT, Vx - Set delay timer = Vx.
        case 0x15:
            chip8->registers.delay_timer = chip8->registers.V[x];
        break;

        // Fx18 - LD ST, Vx - Set sound timer = Vx.
        case 0x18:
            chip8->registers.sound_timer = chip8->registers.V[x];
        break;

        // Fx18 - LD ST, Vx - Set sound timer = Vx.
        case 0x1E:
            chip8->registers.I += chip8->registers.V[x];
        break;

        // Fx29 - LD F, Vx - Set I = location of sprite for digit Vx.
        case 0x29:
            chip8->registers.I = chip8->registers.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
        break;

        // Fx33 - LD B, Vx - The interpreter takes the decimal value of Vx, and places the hundreds 
        // digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
        case 0x33:
        {
            unsigned char hundreds = chip8->registers.V[x] / 100;
            unsigned char tens = chip8->registers.V[x] / 10 % 10;
            unsigned char units = chip8->registers.V[x] % 10;
            chip8_memory_set(&chip8->memory, chip8->registers.I, hundreds);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 1, tens);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 2, units);
        }

        // Fx55 - LD [I], Vx - The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
        case 0x55:
        {
            for (int i = 0; i <= x; i++) {
                chip8_memory_set(&chip8->memory, chip8->registers.I + i, chip8->registers.V[i]);
            }
        }
        break;

        // Fx65 - LD Vx, [I] - The interpreter reads values from memory starting at location I into registers V0 through Vx.
        case 0x65:
        {
            for (int i = 0; i <= x; i++) {
                chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I + i);
            }
        }
        break;
    }
}

static void chip8_exec_extended(struct chip8* chip8, unsigned short opcode) {
    // Ex: 2nnn
    unsigned short nnn = opcode & 0x0fff;

    // Ex: 3xkk
    unsigned char x = (opcode >> 8) & 0x000f;
    unsigned char kk = opcode & 0x00ff;

    // Ex: 4xff
    unsigned char y = (opcode >> 4) & 0x000f;

    unsigned char n = opcode & 0x000f;

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

        case 0x8000:
            chip8_exec_extended_eight(chip8, opcode);
        break;

        // 9xy0 - SNE Vx, Vy - Skip next instruction if Vx != Vy.
        case 0x9000:
            if (chip8->registers.V[x] != chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
        break;

        // Annn - LD I, addr - The value of register I is set to nnn.
        case 0xA000:
            chip8->registers.I = nnn;
        break;

        // Bnn - JP V0, addr - Jump to location nnn + V0
        case 0xB000:
            chip8->registers.PC = nnn + chip8->registers.V[0x00];
        break;

        // Cxkk - RND Vx, byte - Set Vx = random byte AND kk.
        case 0xC000:
            srand(clock());
            chip8->registers.V[x] = (rand() % 255) & kk;
        break;

        // Dxyn - DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
        case 0xD000:
        {
            const char* sprite = (const char*) &chip8->memory.memory[chip8->registers.I];
            chip8->registers.V[0x0f] = chip8_screen_draw_sprite(&chip8->screen, chip8->registers.V[x], chip8->registers.V[y], sprite, n);
        }
        break;

        case 0xE000:
        {
            switch(opcode & 0x00ff) {
                case 0x9E: // Ex9E - SKP Vx - Skip next instruction if key with the value of Vx is pressed.
                    if (chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])) {
                        chip8->registers.PC += 2;
                    }
                break;

                case 0xA1: // ExA1 - SKNP Vx - Skip next instruction if key with the value of Vx is not pressed.
                    if (!chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])) {
                        chip8->registers.PC += 2;
                    }
                break;
            }
        }
        break;

        // Fx07 - LD Vx, DT - Set Vx = delay timer value.
        case 0xF000:
            chip8_exec_extended_F(chip8, opcode);
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