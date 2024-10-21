# Program 1
# - Devices
gcc -c ../../../drivers/screen-linux-sdl/screen-linux-sdl.c -O3 -march=native -I ../../../devices/screen/

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/screen

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/screen

# - Combine the whole program
gcc -o program program.o launcher.o screen-linux-sdl.o -lm -lpthread -lSDL2
