# Program 1
# - Devices
gcc -c ../../../drivers/puc-linux-processes/linux-processes-puc.c -O3 -march=native -I ../../../devices/puc/

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/puc

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/puc

# Programs for PUs
gcc -c -O3 ../program1.c -march=native
gcc -c -O3 ../program2.c -march=native

# - Combine the whole program
gcc -o program program.o program1.o program2.o launcher.o linux-processes-puc.o -lm -lrt -lpthread
