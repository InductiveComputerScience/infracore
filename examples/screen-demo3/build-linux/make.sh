# Program 1
# - Devices
gcc -c ../../../drivers/screen-linux-sdl/screen-linux-sdl.c -I ../../../devices/screen/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/screen

# Program
gcc -c ../program.s

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o screen-linux-sdl.o -lm -lpthread -lSDL2