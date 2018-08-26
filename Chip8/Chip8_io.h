/*
Header file that holds b
*/



#include "Chip8.h"
#include <SDL2/SDL.h>
#include <dirent.h>
#include <stdio.h>

/******************************************************************************/
/******************************************************************************/

static const int SCREEN_WIDTH = 650;
static const int SCREEN_HEIGHT = 330;

SDL_Event event;
SDL_Window* window;
SDL_Surface* screenSurface;

char *rom_name;
char window_name[1024];
char window_name_pause[1024];

/******************************************************************************/
//CORE PLATFORM DEFINITIONS. USER *MUST* IMPLEMENT THESE ACCORDING TO THE PLATFORM
//THIS EMULATOR WILL RUN ON.
/******************************************************************************/

void _window_init();
void _window_kill();
void _drawScreen(Chip8 *chip8);
void _drawScreenInvert(Chip8 *chip8);
void _getKeystate(Chip8 *chip8);
uint32_t _get_tick();
void _beep();

/******************************************************************************/
/********************  ADDITIONAL HELPER FUNCTIONS ****************************/
/******************************************************************************/

uint16_t read_rom_dir(char *filename[]);
char *pick_rom();
