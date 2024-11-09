# Program 1
# - Devices
gcc -g -c ../../../drivers/screen-linux-sdl/screen-linux-sdl.c -I ../../../devices/screen/ -I ../../../devices/keyboard/

# - Infrastructural Program
gcc -g -c launcher.c -I ../../../devices/screen -I ../../../devices/keyboard

# Program: GNU Assembler: program.s, NASM: program.asm
#gcc -c ../program.s
nasm -f elf64 ../program.asm -o program.o -i ../../../devices/screen/ -i ../../../devices/keyboard/

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o screen-linux-sdl.o -lm -lpthread -lSDL2
