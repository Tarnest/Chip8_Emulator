#define BLOCK_SIZE_IN_PIXELS 16

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32

#define SDL_WINDOW_WIDTH (BLOCK_SIZE_IN_PIXELS * CHIP8_WIDTH)
#define SDL_WINDOW_HEIGHT (BLOCK_SIZE_IN_PIXELS * CHIP8_HEIGHT)

#define EMULATION_SPEED 700 // Hz
#define EMULATION_SPEED_TIMER 60 // Hz

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <string>
#include <memory>
#include <chrono>
#include "chip8.h"
#include "open_file.h"

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

std::string SelectedFile;
std::string FilePath;

auto last_cycle_time = std::chrono::high_resolution_clock::now();

// const std::string path = "C:\\Users\\trist\\Documents\\VSCode Projects\\EmuDev\\Chip8\\rom\\5-quirks.ch8";

std::shared_ptr<Chip8> chip8;

bool result = FALSE;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Chip8 Emulator", "1.0", "com.example.CATEGORY-NAME");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Chip8 Emulator", SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, SDL_WINDOW_WIDTH, SDL_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    result = openFile(SelectedFile, FilePath);    

    // ! FIX THIS LATER WHEN CREATING ERROR CHECKING
    switch (result)
    {
        case(TRUE):
        {
            printf("SELECTED FILE: %s\nFILE PATH: %s\n\n", SelectedFile.c_str(), FilePath.c_str());
        }
        case(FALSE):
        {
            printf("ENCOUNTERED AN ERROR: (%d)\n", GetLastError());
        }
    }
    
    /*
    * 
    * CHIP8 PROGRAM START
    *  
    */
    
    if (FilePath.empty())
    {
        return SDL_APP_SUCCESS;
    }

    chip8 = std::make_shared<Chip8>(FilePath, renderer);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    auto current_time = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle_time).count();
    
    // uint64_t start = SDL_GetPerformanceCounter();
    
    if (dt > (1000.0f / EMULATION_SPEED))
    {
        last_cycle_time = current_time;
        
        chip8->fetch();

        chip8->decode_and_execute();
    }

    if (dt > (1000.0f / EMULATION_SPEED_TIMER))
    {
        chip8->update_timers();
    }
    // uint64_t end = SDL_GetPerformanceCounter();
    // float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    
    // Cap to 60 FPS (16.666f)
	// SDL_Delay(floor((1000.0f / 700.0f) - elapsedMS)); // 400 instructions / sec

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}