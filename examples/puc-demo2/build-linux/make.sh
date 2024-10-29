# Program 1
# - Devices
gcc -c ../../../drivers/puc-linux-processes/linux-processes-puc.c -march=native -I ../../../devices/puc/

# - Infrastructural Program
gcc -c launcher.c -march=native -I ../../../devices/puc

# Program
gcc -c ../program.c -march=native -I ../../../devices/puc

# Programs for PUs
gcc -c ../program1.c -march=native -I ../../../devices/puc/
gcc -c ../program2.c -march=native -I ../../../devices/puc/

# - Combine the whole program
gcc -o program program.o program1.o program2.o launcher.o linux-processes-puc.o -lm -lrt -lpthread
