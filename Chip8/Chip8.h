#ifndef CHIP8_H
#define CHIP8_H

#ifdef __cplusplus
extern "C" {
    #endif

    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <time.h>

    /*
    HELPFUL MACROS USED TO REDUCE CODE VERBOSITY WHEN ACCESSING STRUCT MEMBERS, AS WELL
    AS SOME MACRO FUNCTIONS USED WITH KEYPRESSES AND GRAPHICS HANDLING.
    */
    #define X (((chip8->instruction & 0x0F00) >> 8) & 0xFF)
    #define Y (((chip8->instruction & 0x00F0) >> 4) & 0xFF)
    #define NNN (chip8->instruction & 0x0FFF)
    #define NN (chip8->instruction & 0x00FF)
    #define N (chip8->instruction & 0x000F)

    #define INSTRUCTION (chip8->instruction)
    #define MEMORY (chip8->memory)
    #define STACK (chip8->stack)
    #define SP (chip8->sp)
    #define PC (chip8->pc)
    #define V (chip8->regV)
    #define I (chip8->regI)

    #define DELAY (chip8->delay)
    #define SOUND (chip8->sound)
    #define HALT (chip8->halt)
    #define PAUSE (chip8->pause)

    #define KEYPAD (chip8->keypad)
    #define KEYSET(K) ( (chip8->keypad) |= (1 << (K)) )
    #define KEYRESET(K) ( (chip8->keypad) &= ~(1 << (K)) )
    #define KEYTEST(K) (KEYPAD & (1 << (K)))

    #define DISPLAY (chip8->display)
    #define DISPLAY_WIDTH 64
    #define DISPLAY_HEIGHT 32
    #define PIXELXOR(I, J) DISPLAY[(I)/8][(J) % 32] ^= (1 << ((I) % 8))
    #define PIXELTEST(I, J) (DISPLAY[(I)/8][(J) % 32]) & (1 << ((I) % 8))

    typedef struct Chip8_t {

        /* wait and halt flags */
        bool halt;
        bool pause;

        /* program counter */
        uint16_t pc;

        /* current instruction */
        uint16_t instruction;

        /* memory */
        uint8_t memory[4096];

        /* V and I registers */
        uint8_t regV[16];
        uint16_t regI;

        /* stack and stack pointer*/
        uint16_t stack[16];
        uint16_t sp;

        /* timers */
        uint8_t delay;
        uint8_t sound;

        /* I/O */
        /* CHIP8 GRAPHICS BUFFER. REPRESENTED AS AN 8X32 uint8_t ARRAY, AS
        OPPOSED TO CONVENTIONAL IMPLEMENTATIONS THAT USE uint8_t[64*32].
        ADDRESSING INDIVIDUAL BITS IS MORE INVOLVED, BUT USES LESS MEMORY. THIS
        IS HELPFUL FOR RUNNING ON EMBEDDED PLATFORMS, WHERE RESOURCES ARE
        LIMITED.
        */
        uint8_t display[8][32];
        /*  KEYPAD REGISTER, REPRESENTED AS AN UNSIGNED 16 INTEGER*/
        uint16_t keypad;


    } Chip8;

    static const uint16_t memsize = 4096;
    static const uint8_t chip8_font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80 //F
    };

    /* initialization and runtime routines */
    void chip8_loadrom(Chip8 *chip8, char *romname);
    void chip8_loadmem(Chip8 *chip8, uint8_t rom[], uint16_t length);
    void chip8_init(Chip8 *chip8);

    void chip8_clockcycle(Chip8 *chip8);
    void chip8_decode(Chip8 *chip8);

    /* opcode decoding functions */
    void chip8_op0(Chip8 *chip8);
    void chip8_op1(Chip8 *chip8);
    void chip8_op2(Chip8 *chip8);
    void chip8_op3(Chip8 *chip8);
    void chip8_op4(Chip8 *chip8);
    void chip8_op5(Chip8 *chip8);
    void chip8_op6(Chip8 *chip8);
    void chip8_op7(Chip8 *chip8);
    void chip8_op8(Chip8 *chip8);
    void chip8_op9(Chip8 *chip8);
    void chip8_opA(Chip8 *chip8);
    void chip8_opB(Chip8 *chip8);
    void chip8_opC(Chip8 *chip8);
    void chip8_opD(Chip8 *chip8);
    void chip8_opE(Chip8 *chip8);
    void chip8_opF(Chip8 *chip8);

    /* I/O Routines*/
    void chip8_bind_io(void (*getKeystate)(Chip8 *chip8), void (*drawScreen)(Chip8 *chip8),
    uint32_t (*_get_tick)(), void (*_beep)());
    void (*chip8_getKeystate)(Chip8 *chip8);
    void (*chip8_drawScreen)(Chip8 *chip8);
    uint32_t (*chip8_get_tick)();
    void (*chip8_beep)();

    /* Debugging Routines*/
    void chip8_printCurrentInstruction(Chip8 *chip8);
    void chip8_printRom(Chip8 *chip8);
    void chip8_printMem(Chip8 *chip8);


    #ifdef __cplusplus
}
#endif

#endif /* CHIP8_H */
