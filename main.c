#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include <dirent.h>
#include "Chip8/Chip8.h"
#include "Chip8/Chip8_io.h"

int main(int argc, char** argv) {
    rom_name = pick_rom();

    Chip8 chip8;
    chip8_loadrom(&chip8, rom_name);
    chip8_bind_io(&_getKeystate, &_drawScreen, &_get_tick, &_beep);
    chip8_init(&chip8);
    
    _window_init(rom_name);

    while(!chip8.halt){
        chip8_clockcycle(&chip8);
    }

    _window_kill();
    return 1;
}
