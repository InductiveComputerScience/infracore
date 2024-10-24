# Program 1
# - Devices
gcc -c ../../../drivers/pu-linux-tcp/pu-linux-tcp-server.c -O3 -march=native -I ../../../devices/pu/

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/pu

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/pu

# - Combine the whole program
gcc -o program program.o launcher.o pu-linux-tcp-server.o -lm -lrt -lpthread
