# Program 1
# - Devices
gcc -c ../../../drivers/screen-linux-sdl/screen-linux-sdl.c -I ../../../devices/screen/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/screen

# Program: program.s in GNU Assembler, program-nasm.asm in NASM
#gcc -c ../program.s
nasm -f elf64 ../program-nasm.asm -o program.o -i ../../../devices/screen/

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o screen-linux-sdl.o -lm -lpthread -lSDL2
