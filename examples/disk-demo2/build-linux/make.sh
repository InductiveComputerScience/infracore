# Program 1
# - Devices
gcc -g -c ../../../drivers/disk-linux-file/disk-linux-file.c -I ../../../devices/disk/
gcc -g -c ../../../drivers/disk-linux-file/disk-linux-file-asm.s

# - Infrastructural Program
gcc -g -c launcher.c -I ../../../devices/disk

# Program
nasm -f elf64 ../program.asm -o program.o -i ../../../devices/disk/

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o disk-linux-file.o disk-linux-file-asm.o -lm -lrt -lpthread
