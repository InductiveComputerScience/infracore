# Program 1
# - Devices
gcc -c ../../../drivers/disk-linux-file/disk-linux-file.c -I ../../../devices/disk/
gcc -c ../../../drivers/disk-linux-file/disk-linux-file-asm.s

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/disk

# Program
gcc -c ../program.c -I ../../../devices/disk

# - Combine the whole program
gcc -o program program.o launcher.o disk-linux-file.o disk-linux-file-asm.o -lm -lrt -lpthread
