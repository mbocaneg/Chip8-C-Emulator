#include "Chip8.h"
#include <stdio.h>
#include <string.h>


#define PSC 10

uint32_t start_time = 0;
uint16_t rom_size = 0;
char *rom_name;

uint16_t currentPC = 0;

void chip8_loadrom(Chip8 *chip8, char *romname) {
    /* Clear system memory*/
    memset(MEMORY, 0x00, memsize);

    /* Open ROM and find its size*/
    FILE *fp = fopen(romname, "rb");
    fseek(fp, 0, SEEK_END);
    uint16_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* Copy necessary rom bytes into
    * system memory*/
    fread(&MEMORY[0x200], 1, length, fp);
    rom_size = length;
    rom_name = romname;
}

void chip8_loadmem(Chip8 *chip8, uint8_t rom[], uint16_t length) {
    /* Clear system memory*/
    memset(MEMORY, 0x00, memsize);
    memcpy(&MEMORY[0x200], rom, length);
}

void chip8_bind_io(void (*getKeystate)(Chip8 *chip8), void (*drawScreen)(Chip8 *chip8),
uint32_t (*_get_tick)(), void (*_beep)()){
    chip8_drawScreen = drawScreen;
    chip8_getKeystate = getKeystate;
    chip8_get_tick = _get_tick;
    chip8_beep = _beep;
}

void chip8_init(Chip8 *chip8) {

    /* Clear display, stack. keypad, and V registers*/
    memset(DISPLAY,0, sizeof(DISPLAY[0][0])*8*32);

    KEYPAD = 0x00;
    for(uint8_t i = 0; i < 16; i++){
        STACK[i] = 0x0000;
        //KEYPAD[i].Key = 0x0;
        V[i] = 0x00;
    }

    /* Copy fonts into system memory */
    memcpy(MEMORY, chip8_font, 80);

    HALT = false;
    PAUSE = false;

    /* Initialize PC to address 0x200*/
    PC = 0x200;

    /* Clear SP, I, and Instruction registers, as
    * as well as draw flag */
    SP = 0x0;
    I = 0x0;
    INSTRUCTION = 0x0000;

    start_time = chip8_get_tick();
}

void chip8_clockcycle(Chip8 *chip8) {
    uint32_t elapsed = 0;
    uint32_t dt = 0;

    if(!PAUSE){

        elapsed = chip8_get_tick();
        dt = elapsed - start_time;

        if(dt > 3){
            // fetch instruction
            INSTRUCTION = (MEMORY[PC] << 8) | (MEMORY[PC + 1]);

            currentPC = PC;

            // increment program counter
            PC += 2;

            // decode instruction
            chip8_decode(chip8);

        }

        if (dt > 16 ) {

            if(DELAY > 0){
                DELAY -= 1;
            }

            if(SOUND > 0){
                SOUND -= 1;
                if(chip8_beep != NULL){
                    chip8_beep();
                }
            }

            start_time = elapsed;
            elapsed = 0;
        }

        if(chip8_drawScreen != NULL){
            chip8_drawScreen(chip8);
        }

    }

    if(chip8_getKeystate != NULL){
        chip8_getKeystate(chip8);
    }

}

void chip8_decode(Chip8 *chip8) {
    /* Extract Instruction opcode */
    uint16_t opcode = INSTRUCTION & 0xF000;

    /* execute instruction based on opcode */
    switch (opcode) {
        case 0x0000:
        chip8_op0(chip8);
        break;

        case 0x1000:
        chip8_op1(chip8);
        break;

        case 0x2000:
        chip8_op2(chip8);
        break;

        case 0x3000:
        chip8_op3(chip8);
        break;

        case 0x4000:
        chip8_op4(chip8);
        break;

        case 0x5000:
        chip8_op5(chip8);
        break;

        case 0x6000:
        chip8_op6(chip8);
        break;

        case 0x7000:
        chip8_op7(chip8);
        break;

        case 0x8000:
        chip8_op8(chip8);
        break;

        case 0x9000:
        chip8_op9(chip8);
        break;

        case 0xA000:
        chip8_opA(chip8);
        break;

        case 0xB000:
        chip8_opB(chip8);
        break;

        case 0xC000:
        chip8_opC(chip8);
        break;

        case 0xD000:
        chip8_opD(chip8);
        break;

        case 0xE000:
        chip8_opE(chip8);
        break;

        case 0xF000:
        chip8_opF(chip8);
        break;
    }

}

void chip8_op0(Chip8 *chip8) {
    // 00E0 - CLEARS SCREEN
    if (NN == 0x00E0) {
        memset(DISPLAY,0, sizeof(DISPLAY[0][0])*8*32);
    }

    // 00EE - Returns from a subroutine
    else if (NN == 0x00EE) {
        SP -= 1;
        PC = STACK[SP];
    }
}

// 1NNN - Jumps to address NNN
void chip8_op1(Chip8 *chip8) {
    PC = NNN;
}

// 2NNN - Calls subroutine at NNN
void chip8_op2(Chip8 *chip8) {
    STACK[SP] = PC;
    SP++;
    PC = NNN;
}

// 3XNN - Skips the next instruction if VX equals NN
void chip8_op3(Chip8 *chip8) {
    if (V[X] == NN)
    PC += 2;
}

// 4XNN - Skips the next instruction if VX doesn't equal NN
void chip8_op4(Chip8 *chip8) {
    if (V[X] != NN)
    PC += 2;
}

// 5XY0 - Skips the next instruction if VX equals VY
void chip8_op5(Chip8 *chip8) {
    if (V[X] == V[Y])
    PC += 2;
}

// 6XNN - Sets VX to NN
void chip8_op6(Chip8 *chip8) {
    V[X] = NN;
}

// 7XNN - Adds NN to VX
void chip8_op7(Chip8 *chip8) {
    V[X] += NN;
}

void chip8_op8(Chip8 *chip8) {

    // 8XY0 - Sets VX to the value of VY
    if (N == 0x0) {
        V[X] = V[Y];
    }

    /*8XY1 - Sets VX to VX or VY*/
    else if (N == 0x1) {
        V[X] |= V[Y];
    }

    // 8XY2 - Sets VX to VX and VY
    else if (N == 0x2) {
        V[X] &= V[Y];
    }

    // 8XY3 - Sets VX to VX xor VY
    else if (N == 0x3) {
        V[X] ^= V[Y];
    }

    /* 8XY4 - Adds VY to VX. VF is set to 1 when
    there's a carry, and to 0 when there isn't */
    else if (N == 0x4) {
        V[0xF] = (V[X] > (0xFF - V[Y])) ? 0x1 : 0x0;
        V[X] += V[Y];
    }

    /* 8XY5 - VY is subtracted from VX. VF is set to 0 when
    * there's a borrow, and 1 when there isn't*/
    else if(N == 0x5){
        V[0xF] = (V[X] > V[Y] )? 0x1: 0x0;
        V[X] -= V[Y];
    }

    /* 8XY6 - Shifts VX right by one. VF is set
    * to the value of LSB of VX before the shift */
    else if(N == 0x6){
        V[0xF] = V[X] & 0x01;
        V[X] = (V[X] >> 1);
    }

    /* 8XY7 - Sets VX to VY minus VX. VF is set to
    * 0 when there's a borrow, and 1 when there isn't */
    else if(N == 0x7){
        V[0xF] = (V[Y] > V[X] )? 0x1: 0x0;
        V[X] = V[Y] - V[X];
    }

    /* 8XYE - Shifts VX left by one. VF is set to the
    * value of the MSB of VX before the shift */
    else if(N == 0xE){
        V[0xF] = V[X] & 0x80;
        V[X] = V[X] << 1;
    }
}

// 9XY0 - Skips the next instruction if VX doesn't equal VY
void chip8_op9(Chip8 *chip8) {
    if (V[X] != V[Y])
    PC += 2;
}

// ANNN - Sets I to the address NNN
void chip8_opA(Chip8 *chip8) {
    I = NNN;
}

// BNNN - Jumps to the address NNN plus V0
void chip8_opB(Chip8 *chip8) {
    PC = NNN + V[0];
}

/* CXNN - Sets VX to the result of a bitwise
* and operation on a random number and NN */
void chip8_opC(Chip8 *chip8) {
    srand(time(0));
    V[X] = NN & (rand() % 0xFF);
}

/* DXYN - Draws a sprite at coordinate (VX, VY) that has
* a width of 8 pixels and a height of N pixels */
void chip8_opD(Chip8 *chip8) {

    V[0xF] = 0;
    uint8_t xx = V[X];
    uint8_t yy = V[Y];
    for(int i = 0; i < N; i++){
        uint8_t pixel = MEMORY[I + i];
        for(int j = 0; j < 8; j++){
            if( (pixel & (0x80 >> j)) != 0x00){
                if(PIXELTEST(xx+j, yy+i))
                V[0xF] |= 1;
                PIXELXOR(xx+j, yy+i);
            }
        }
    }

}

void chip8_opE(Chip8 *chip8) {
    // EXA1 - Skips the next instruction if the key stored in VX isn't pressed
    if (NN == 0x00A1) {
        if (KEYTEST(V[X]) == 0)
        PC += 2;
    }

    // EX9E - Skips the next instruction if the key stored in VX isn't pressed
    else if (NN == 0x009E) {
        if (KEYTEST(V[X]) != 0)
        PC += 2;
    }
}

void chip8_opF(Chip8 *chip8) {
    /* FX07 - Sets VX to the value of the delay timer */
    if(NN == 0x07){
        V[X] = DELAY;
    }

    /* FX0A - A key press is awaited, and then stored in VX. (Blocking Operation.
    * All instruction halted until next key event */
    else if(NN == 0x0A){
        bool wait = true;
        while(wait && !HALT){
            if(chip8_getKeystate != NULL){
                chip8_getKeystate(chip8);
            }
            for(uint8_t i = 0; i < 16; i++){
                if(KEYTEST(i) != 0){
                    V[X] = i;
                    wait = false;
                    return;
                }
            }
        }
    }

    /* FX15 - Sets the delay timer to VX */
    else if(NN == 0x15){
        DELAY = V[X];
    }

    /* FX18 - Sets the sound timer to VX */
    else if(NN == 0x18){
        SOUND = V[X];
    }

    /* FX1E - Adds VX to I */
    else if(NN == 0x1E){
        I += V[X];
    }

    /* FX29 - Sets I to the location of the
    * sprite for the character in VX */
    else if(NN == 0x29){
        I =  V[X]*5;
    }

    /* FX33 - Stores the binary-coded decimal representation
    * of VX @ addr I, I+1, I+2 */
    else if(NN == 0x33){

        uint8_t val = V[X];
        uint16_t hundreds = val / 100;
        uint16_t tens = (val - 100*(val/100)) / 10;
        uint16_t ones = val - (hundreds*100 + tens*10);

        MEMORY[I] = hundreds;
        MEMORY[I + 1] = tens;
        MEMORY[I + 2] = ones;
    }

    /* FX55 - Stores V0 to VX (including VX) in memory starting
    * at address I. I is increased by 1 for each value written. */
    else if(NN == 0x55){
        for(int i = 0; i <= X; i++){
            MEMORY[I + i] = V[i];
        }
        I += (X + 1);
    }

    /* FX65 - Fills V0 to VX (including VX) with values from memory starting at
    * address I. I is increased by 1 for each value written. */
    else if(NN == 0x65){
        for(int i = 0; i <= X; i++){
            V[i] = MEMORY[I + i];
        }
        I += (X + 1);
    }
}

/********************************************************************************/
void chip8_printCurrentInstruction(Chip8 *chip8) {
    printf("PC: 0x%04x | ", (unsigned short) currentPC);
    printf("INSTRUCTION: 0x%04X\n\n", (unsigned short) INSTRUCTION);
}

void chip8_printRom(Chip8 *chip8) {
    printf("**************************\n");
    printf("ROM : %s \nLENGTH: %hu bytes\n", rom_name, rom_size);
    printf("**************************\n");
    for (uint16_t i = 0x200; i < 0x200 + rom_size; i++) {
        printf("0x%04x: ", (unsigned short) i);
        printf("%x", (unsigned char) MEMORY[i] & 0xff);
        printf("\n");
    }
}

void chip8_printMem(Chip8 *chip8) {
    printf("**************************\n");
    printf("MEMORY DUMP\n");
    printf("**************************\n");

    uint16_t memsize = 4096;
    for (uint16_t i = 0; i < memsize; i++) {
        printf("0x%04x: ", (unsigned short) i);
        printf("%02hhX", (unsigned char) MEMORY[i] & 0xff);
        printf("\n");
    }
}
