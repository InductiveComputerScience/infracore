# Program 1
# - Devices
gcc -c ../../../drivers/puc-linux-processes/linux-processes-puc.c -I ../../../devices/puc/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/puc

# Program
gcc -c ../program.c -I ../../../devices/puc

# Programs for PUs
gcc -c ../program1.c -I ../../../devices/puc/
gcc -c ../program2.c -I ../../../devices/puc/

# - Combine the whole program
gcc -o program program.o program1.o program2.o launcher.o linux-processes-puc.o -lm -lrt -lpthread
