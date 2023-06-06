#include "chip8keyboard.h"
#include "config.h"
#include <assert.h>

static void chip8_keyboard_ensure_in_bounds(int key) {
    assert(key >= 0 && key <= CHIP8_TOTAL_KEYS);
}

void chip8_keyboard_set_map(struct chip8_keyboard* keyboard, const char* map) {
    keyboard->keyboard_map = map;
}


// map represents our virtual keyboard
// key represents the physical desktop keyboard
int chip8_keyboard_map(struct chip8_keyboard* keyboard, char key) {
    for (int i = 0; i < CHIP8_TOTAL_KEYS; i++) {
        if (keyboard->keyboard_map[i] == key) {
            return i; // Key was found
        }
    }
    return -1; // Key was not found
}


// Key pressed
void chip8_keyboard_down(struct chip8_keyboard* keyboard, int key) {
    keyboard->keyboard[key] = true;
}

// Key released
void chip8_keyboard_up(struct chip8_keyboard* keyboard, int key) {
    keyboard->keyboard[key] = false;
}

bool chip8_keyboard_is_down(struct chip8_keyboard* keyboard, int key) {
    return keyboard->keyboard[key];
}