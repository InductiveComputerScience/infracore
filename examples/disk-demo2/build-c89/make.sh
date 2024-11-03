# Program 1
# - Devices
gcc -c ../../../drivers/disk-c89-file/disk-c89-file.c -std=c89 -I ../../../devices/disk/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/disk

# Program
nasm -f elf64 ../program.asm -o program.o -i ../../../devices/disk/

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o disk-c89-file.o -lm -lrt -lpthread
