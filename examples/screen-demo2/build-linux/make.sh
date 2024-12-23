# Program 1
# - Devices
gcc -c ../../../drivers/screen-linux-sdl/screen-linux-sdl.c -I ../../../devices/screen/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/screen

# Program: GNU Assembler: program.s, NASM: program.asm
#gcc -c ../program.s
nasm -f elf64 ../program.asm -o program.o -i ../../../devices/screen/

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o screen-linux-sdl.o -lm -lpthread -lSDL2
