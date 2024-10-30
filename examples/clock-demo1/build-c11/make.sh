# Program 1
# - Devices
gcc -c ../../../drivers/clock-c11/clock-c11.c -std=c11 -I ../../../devices/clock/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/clock

# Program
gcc -c ../program.c -I ../../../devices/clock

# - Combine the whole program
gcc -o program program.o launcher.o clock-c11.o -lm
