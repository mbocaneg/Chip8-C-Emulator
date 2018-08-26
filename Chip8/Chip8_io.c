#include "Chip8_io.h"

/*
Platform dependent function that initializes a graphics window.

For this implemntation(Unix/Linux Platforms), graphics are handled using SDL.
This function simply initializes an SDL window instance and paints it black.
*/
void _window_init(){

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return;
    }

    sprintf(window_name, "CHIP8-C: %s", rom_name);
    sprintf(window_name_pause, "%s - PAUSED", window_name);

    window = SDL_CreateWindow(window_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        return;
    }

    screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
    SDL_UpdateWindowSurface( window );

}

/*
Platform dependent function that kills a graphics window.

In this implementation, this function kills the running SDL wind0w.
*/
void _window_kill(){
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("EXITING...\n");
}

/*
Platform dependent function that draws the Chip8 graphics
memory onto a graphics window

In this implementation, an SDL rectangle is drawn onto the SDL window and is filled
with a green color.
*/
void _drawScreen(Chip8 *chip8){

    //black screen
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

    for(uint8_t y = 0; y < 32; y++){
        for(uint8_t x = 0; x < 64; x++){
            Uint32 color = (PIXELTEST(x, y))? 0x00FF00: 0x000000;
            SDL_Rect rect = {x * 10, y * 10, 10, 10};
            SDL_FillRect(screenSurface, &rect, color);
        }
    }

    SDL_UpdateWindowSurface(window);

}

/*
Platform dependent function that inverts the graphics window. Used
to indicate a paused state.
*/
void _drawScreenInvert(Chip8 *chip8){

    //black screen
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

    for(uint8_t y = 0; y < 32; y++){
        for(uint8_t x = 0; x < 64; x++){
            Uint32 color = (PIXELTEST(x, y))?  0x000000: 0x00FF00;
            SDL_Rect rect = {x * 10, y * 10, 10, 10};
            SDL_FillRect(screenSurface, &rect, color);
        }
    }

    SDL_UpdateWindowSurface(window);

}

/*
Platform dependent function that captures system keypresses, and
then alters the Chip8 keypress state accordingly.

For this implementation, keypresses are handled using SDL keypress
events. The Chip8 keypad is mapped to a pc keyboard in the following
manner:

1234   ->   |1|2|3|C|
qwer   ->   |4|5|6|D|
asdf   ->   |7|8|9|E|
zxcv   ->   |A|0|B|F|

Whenever a keypad press is detected, the corresponding bit in the keypad
register is set to 1, else, the corresponding bit is set to 0.

*/
void _getKeystate(Chip8 *chip8){
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);
    SDL_PollEvent(&event);

    if(event.type == SDL_QUIT){
        HALT = true;
        return;
    }

    if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
        if(keyboard_state_array[SDL_SCANCODE_ESCAPE]){
            chip8_init(chip8);
            return;
        }

        if(keyboard_state_array[SDL_SCANCODE_P]){
            PAUSE = !PAUSE;

            if(PAUSE == true){
                SDL_SetWindowTitle(window, window_name_pause);
                _drawScreenInvert(chip8);
            }
            else{
                SDL_SetWindowTitle(window, window_name);
                _drawScreen(chip8);
            }
            return;
        }

        (keyboard_state_array[SDL_SCANCODE_1])? KEYSET(0x01) : KEYRESET(0x01);
        (keyboard_state_array[SDL_SCANCODE_2])? KEYSET(0x02) : KEYRESET(0x02);
        (keyboard_state_array[SDL_SCANCODE_3])? KEYSET(0x03) : KEYRESET(0x03);
        (keyboard_state_array[SDL_SCANCODE_4])? KEYSET(0x0C) : KEYRESET(0x0C);

        (keyboard_state_array[SDL_SCANCODE_Q])? KEYSET(0x04) : KEYRESET(0x04);
        (keyboard_state_array[SDL_SCANCODE_W])? KEYSET(0x05) : KEYRESET(0x05);
        (keyboard_state_array[SDL_SCANCODE_E])? KEYSET(0x06) : KEYRESET(0x06);
        (keyboard_state_array[SDL_SCANCODE_R])? KEYSET(0x0D) : KEYRESET(0x0D);

        (keyboard_state_array[SDL_SCANCODE_A])? KEYSET(0x07) : KEYRESET(0x07);
        (keyboard_state_array[SDL_SCANCODE_S])? KEYSET(0x08) : KEYRESET(0x08);
        (keyboard_state_array[SDL_SCANCODE_D])? KEYSET(0x09) : KEYRESET(0x09);
        (keyboard_state_array[SDL_SCANCODE_F])? KEYSET(0x0E) : KEYRESET(0x0E);

        (keyboard_state_array[SDL_SCANCODE_Z])? KEYSET(0x0A) : KEYRESET(0x0A);
        (keyboard_state_array[SDL_SCANCODE_X])? KEYSET(0x00) : KEYRESET(0x00);;
        (keyboard_state_array[SDL_SCANCODE_C])? KEYSET(0x0B) : KEYRESET(0x0B);
        (keyboard_state_array[SDL_SCANCODE_V])? KEYSET(0x0F) : KEYRESET(0x0F);

    }

}

/*
function dependent function that gets the number of milliseconds elapsed
since the emulator began execution.

In this implementation, this is handled through SDL's SDL_GetTicks()
function.
*/
uint32_t _get_tick(){
    SDL_GetTicks();
}

/*
platform dependent function that produces a beep tone. NEED A BETTER WAY TO
HANDLE THIS
*/
void _beep(){
    // printf("beep\n");
}

/***********************************************************************************/

/*
Linux/Unix dependent function that prints out a list of names of ROM files
contained within the roms/ directory.
*/
uint16_t read_rom_dir(char *filename[]){
    DIR *d;
    struct dirent *dir;

    uint16_t i = 0;
    if (d = opendir("roms/")) {
        printf("****************************\n" );
        printf("*                          *\n" );
        printf("* ROMs in roms/ directory: *\n" );
        printf("*                          *\n" );
        printf("****************************\n" );

        while ((dir = readdir(d)) != NULL ) {
            if (!strcmp (dir->d_name, "."))
            continue;
            if (!strcmp (dir->d_name, ".."))
            continue;

            printf("%d:", i+1);
            printf(" %s\n", dir->d_name);

            filename[i] = dir->d_name;
            i++;
        }

        closedir(d);
    }

    return i;
}

/*
Linux/Unix dependent function that prompts a user to pick the ROM
to be loaded onto the Chip8's memory
*/
char *pick_rom(){

    char *filename[256];
    uint16_t num_roms = read_rom_dir(filename);

    int n;
    printf("\nLoad which rom?(1-%u)\n", num_roms);
    scanf("%d", &n);
    printf("Loading %s...\n", filename[n-1]);

    static char rom_name[1024];
    sprintf(rom_name, "roms/%s", filename[n-1]);
    return rom_name;
}
