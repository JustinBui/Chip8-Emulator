#include<stdio.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "config.h"
#include "chip8keyboard.h"

const char keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,
    SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char** argv) {
    struct chip8 chip8;
    chip8_init(&chip8);
    
    // Create SDL Window
    SDL_Window* window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE, // Title
        SDL_WINDOWPOS_UNDEFINED, // X coordinate window position (In this case we are using default)
        SDL_WINDOWPOS_UNDEFINED, // Y coordinate window position
        CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER, CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER, // 64x32 pixel window is what CHIP8 uses, but we will be scaling it up to be larger
        SDL_WINDOW_SHOWN
    );

    // Drawing pixels to the screen with renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
    while(1) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) { // Handling events 
            switch(event.type) {
                case SDL_QUIT:
                    goto out;
                break;

                case SDL_KEYDOWN:
                {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(keyboard_map, key);
                    if (vkey != -1) {
                        chip8_keyboard_down(&chip8.keyboard, vkey);
                    }
                }
                break;

                case SDL_KEYUP:
                {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(keyboard_map, key);
                    if (vkey != -1) {
                        chip8_keyboard_down(&chip8.keyboard, vkey);
                    }
                }
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Passing in our renderer, then setting draw color to black (Red, Blue, Green, Alpha) = 0
        SDL_RenderClear(renderer); // Paint over the entire screen black
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

        // Draw a rectangle @ x-coordinate and y-coordinate 0, then width and height to 40 px
        SDL_Rect r;
        r.x = 0;
        r.y = 0;
        r.w = 40;
        r.h = 40;

        SDL_RenderDrawRect(renderer, &r);
        SDL_RenderPresent(renderer);
    } 

out:
    SDL_DestroyWindow(window); // Deallocate this pointer
    return 0;
}