#ifndef CHIP8KEYBOARD_H
#define CHIP8KEYBOARD_H

#include <stdbool.h>
#include "config.h"

// The computers which originally used the Chip-8 Language had a 16-key hexadecimal keypad with the following layout:
// This layout must be mapped into various other configurations to fit the keyboards of today's platforms.

struct chip8_keyboard {
    bool keyboard[CHIP8_TOTAL_KEYS];
    const char* keyboard_map;
};

void chip8_keyboard_set_map(struct chip8_keyboard* keyboard, const char* map);

// Desktop keyboard is different from original CHIP8 keyboard. Whenever the user wants to key down 
// on actual keyboard, we want to map the actual key to the virtual keyboard, simulating CHIP8's keyboard
int chip8_keyboard_map(struct chip8_keyboard* keyboard, char key);

void chip8_keyboard_down(struct chip8_keyboard* keyboard, int key);

void chip8_keyboard_up(struct chip8_keyboard* keyboard, int key);

bool chip8_keyboard_is_down(struct chip8_keyboard* keyboard, int key);

#endif