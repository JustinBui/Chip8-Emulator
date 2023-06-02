#include<stdio.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "config.h"

int main(int argc, char** argv) {
    struct chip8 chip8;
    chip8_memory_set(&chip8.memory, 50, 'Z');
    printf("%c\n", chip8_memory_get(&chip8.memory, 50));

    SDL_Init(SDL_INIT_EVERYTHING);

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
            if (event.type == SDL_QUIT) {
                goto out;
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