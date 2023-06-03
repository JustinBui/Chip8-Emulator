#include "chip8screen.h"
#include "config.h"
#include <assert.h>

static void chip8_screen_check_bounds(int x, int y) {
    assert(x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);
}

// Setting a pixel on the chip8 screen (Specifying x and y)
void chip8_screen_set(struct chip8_screen* screen, int x, int y) {
    chip8_screen_check_bounds(x, y);
    screen->pixels[y][x] = true;
}


// Checking if a pixel is set on a certain x and y coord (This checking is used during rendering)
bool chip8_screen_is_set(struct chip8_screen* screen, int x, int y) {
    chip8_screen_check_bounds(x, y);
    return screen->pixels[y][x];
}