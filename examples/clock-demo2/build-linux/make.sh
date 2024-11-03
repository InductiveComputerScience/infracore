# Program 1
# - Devices
gcc -c ../../../drivers/clock-linux/clock-linux.c -I ../../../devices/clock/
gcc -c ../../../drivers/clock-linux/clock-linux-asm.s

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/clock

# Program
nasm -f elf64 ../program.asm -o program.o -i ../../../devices/clock/

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o clock-linux.o clock-linux-asm.o -lm
