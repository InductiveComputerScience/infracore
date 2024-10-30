# Program 1
# - Devices
gcc -c ../../../drivers/disk-c89-file/disk-c89-file.c -std=c89 -I ../../../devices/disk/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/disk

# Program
gcc -c ../program.c -I ../../../devices/disk

# - Combine the whole program
gcc -o program program.o launcher.o disk-c89-file.o -lm -lrt -lpthread
