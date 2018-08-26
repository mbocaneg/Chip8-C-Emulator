OBJS = main.c Chip8/Chip8.c Chip8/Chip8_io.c
CC = gcc

COMPILER_FLAGS = -w
SDL_FLAGS = $(shell sdl2-config --libs --cflags)
INCLUDES = -IChip8

OBJ_NAME = Chip8-C

all:
	${CC} ${OBJS} ${COMPILER_FLAGS} ${SDL_FLAGS} ${INCLUDES} -o ${OBJ_NAME}
clean:
	-rm -rf ${OBJ_NAME}
