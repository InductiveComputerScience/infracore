# Program 1
# - Devices
gcc -c ../../../drivers/pu-linux-tcp/pu-linux-tcp-server.c -I ../../../devices/pu/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/pu

# Program
gcc -c ../program.c -I ../../../devices/pu

# - Combine the whole program
gcc -o program program.o launcher.o pu-linux-tcp-server.o -lm -lrt -lpthread
