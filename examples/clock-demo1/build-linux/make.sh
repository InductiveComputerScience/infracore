# Program 1
# - Devices
gcc -c ../../../drivers/clock-linux/clock-linux.c -O3 -march=native -I ../../../devices/clock/
gcc -c ../../../drivers/clock-linux/clock-linux-asm.s

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/clock

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/clock

# - Combine the whole program
gcc -o program program.o launcher.o clock-linux.o clock-linux-asm.o -lm
