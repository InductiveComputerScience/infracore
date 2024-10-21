# Program 1
# - Devices
gcc -c ../../../drivers/clock-c11/clock-c11.c -std=c11 -O3 -march=native -I ../../../devices/clock/

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/clock

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/clock

# - Combine the whole program
gcc -o program program.o launcher.o clock-c11.o -lm
