# Program 1
# - Devices
gcc -c ../../../drivers/disk-linux-file/disk-linux-file.c -O3 -march=native -I ../../../devices/disk/
gcc -c ../../../drivers/disk-linux-file/disk-linux-file-asm.s

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/disk

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/disk

# - Combine the whole program
gcc -o program program.o launcher.o disk-linux-file.o disk-linux-file-asm.o -lm -lrt -lpthread
