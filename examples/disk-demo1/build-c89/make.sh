# Program 1
# - Devices
gcc -c ../../../drivers/disk-c89-file/disk-c89-file.c -std=c89 -O3 -march=native -I ../../../devices/disk/

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/disk

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/disk

# - Combine the whole program
gcc -o program program.o launcher.o disk-c89-file.o -lm -lrt -lpthread
