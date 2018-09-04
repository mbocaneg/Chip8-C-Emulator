![chip8_invaders](https://user-images.githubusercontent.com/8182077/45005679-74040b00-afba-11e8-92e9-753823941374.png)

# Chip8-C-Emulator
Code for a Chip8 interpreter written in C. The core implementation was
written with portability in mind, so I/O calls (graphics/key input) are
loosely coupled with the Chip8 model, through the use of function pointers. 
In this implementation, graphics and keypresses are handled using SDL2.0. 
However, it should be more than possible to adapt this code to run on Embedded 
platforms(such as Arduino or STM32).

# Usage
Code is to be built using the included Makefile, and assumes libsdl is 
installed on your system. This code was only tested in Ubuntu 16.04, so
your mileage may vary. The emulator is run by excuting the `Chip8-C` binary
generated by the Makefile. This presents the user with a list of roms available.
After picking a rom, the emulator starts execution. The standard Chip8 hex keypad 
is mapped to the user's keyboard in the following manner:

```
1234   ->   |1|2|3|C|
qwer   ->   |4|5|6|D|
asdf   ->   |7|8|9|E|
zxcv   ->   |A|0|B|F|
```

The user may toggle a pause state by simply pressing the `P` key.
