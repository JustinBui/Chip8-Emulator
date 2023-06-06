#include<stdio.h>
#include <stdbool.h>
#include <windows.h>
#include "SDL2/SDL.h"
#include "chip8.h"
#include "config.h"
#include "chip8keyboard.h"
#include "chip8screen.h"

const char keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,
    SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char** argv) {

    // ----------------------- Reading into specified file from user and making our buffer -----------------------
    if (argc < 2) {
        printf("You must provide a file to load");
        return -1;
    }

    const char* filename = argv[1];
    printf("Reading in file %s...\n", filename);
    
    FILE* f = fopen(filename, "rb"); // Read binary files

    if (!f) {
        printf("Failed to open file");
        return -1;
    }

    fseek(f, 0, SEEK_END);      // Identifying the buffer size of the file provided from start (0) to end (SEEK_END)
    long size = ftell(f);       // Return size of the file
    fseek(f, 0, SEEK_SET);      // Seek back to the beginning of the file

    char buf[size]; // Making our buffer
    int res = fread(buf, size, 1, f); // Read from file

    if (res != 1) {
        printf("Failed to read from file");
        return -1;
    }

    // ----------------------- Initializing/Setup Chip8 -----------------------
    struct chip8 chip8;
    chip8_init(&chip8);
    chip8_load(&chip8, buf, size);
    chip8_keyboard_set_map(&chip8.keyboard, keyboard_map);

    // ----------------------- Create SDL Window -----------------------
    SDL_Init(SDL_INIT_EVERYTHING); // Initalize everything with SDL
    SDL_Window* window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE, // Title
        SDL_WINDOWPOS_UNDEFINED, // X coordinate window position (In this case we are using default)
        SDL_WINDOWPOS_UNDEFINED, // Y coordinate window position
        CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER, CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER, // 64x32 pixel window is what CHIP8 uses, but we will be scaling it up to be larger
        SDL_WINDOW_SHOWN
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

    // ----------------------- Running the program (Infinite Loop) -----------------------
    while(1) {
        SDL_Event event;

        // Event Handling
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    goto out;
                break;

                case SDL_KEYDOWN:
                {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(&chip8.keyboard, key);
                    if (vkey != -1) {
                        chip8_keyboard_down(&chip8.keyboard, vkey);
                    }
                }
                break;

                case SDL_KEYUP:
                {
                    char key = event.key.keysym.sym;
                    int vkey = chip8_keyboard_map(&chip8.keyboard, key);
                    if (vkey != -1) {
                        chip8_keyboard_up(&chip8.keyboard, vkey);
                    }
                }
                break;
            }
        }

        // ----------------------- Drawing pixels to the screen with renderer -----------------------

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Passing in our renderer, then setting the screen to black (Red, Blue, Green, Alpha) = 0
        SDL_RenderClear(renderer); // Paint over the entire screen black
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); // Setting renderer to white so our rectangle is also white

        for (int x = 0; x < CHIP8_WIDTH; x++) {
            for (int y = 0; y < CHIP8_HEIGHT; y++) {
                if (chip8_screen_is_set(&chip8.screen, x, y)) {
                    // One rectangle represents one giant pixel (Since we are scaling up for bigger screen sizes on desktop)
                    SDL_Rect r;
                    r.x = x * CHIP8_WINDOW_MULTIPLIER;
                    r.y = y * CHIP8_WINDOW_MULTIPLIER;
                    r.w = CHIP8_WINDOW_MULTIPLIER;
                    r.h = CHIP8_WINDOW_MULTIPLIER;
                    // SDL_RenderDrawRect(renderer, &r);
                    SDL_RenderFillRect(renderer, &r); // Filled white pixel
                }
            }
        }
        SDL_RenderPresent(renderer);

        if (chip8.registers.delay_timer > 0) {
            Sleep(1);
            chip8.registers.delay_timer -= 1;
        }

        if (chip8.registers.sound_timer > 0) {
            Beep(1500, 10 * chip8.registers.sound_timer);
            chip8.registers.sound_timer = 0;
        }

        // Read 2 bytes from memory from where the program counter is pointing to (Opcode), then execute opcode
        unsigned short opcode = chip8_memory_get_short(&chip8.memory, chip8.registers.PC);
        chip8.registers.PC += 2; // Increasing program counter by 2 to read the next 2 bytes in the loop
        chip8_exec(&chip8, opcode);
    } 

out:
    SDL_DestroyWindow(window); // Deallocate this pointer
    return 0;
}
