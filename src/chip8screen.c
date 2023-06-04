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

// The chip8 interpreter reads n bytes from memory, starting at address stored in I. These bytes are then displayed as sprites 
// at coordinates x and y. 
// ==> Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF = 1, else VF = 0
// ==> If sprite overflows outside of the screen, it wraps around to the opposite side of screen.
bool chip8_screen_draw_sprite(struct chip8_screen* screen, int x, int y, const char* sprite, int num) {
    // We change this variable to true if the sprite was "hit" (i.e if a pixel was changed)
    bool pixel_collision = false;

    for (int ly = 0; ly < num; ly++) { // Looping through rows of bytes
        char c = sprite[ly]; // Give the byte in the sprite

        for (int lx = 0; lx < 8; lx++) { // Each byte is 8 bits, hence lx < 8
            // If the current bit is 0, then no pixel is set
            if ((c & (0b10000000 >> lx)) == 0)
                continue;

            if (screen->pixels[(ly+y) % CHIP8_HEIGHT][(lx+x) % CHIP8_WIDTH]) {
                pixel_collision = true;
            }
            screen->pixels[(ly+y) % CHIP8_HEIGHT][(lx+x) % CHIP8_WIDTH] ^= true; // XOR
        }
    }

    return pixel_collision;
}