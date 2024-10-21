# Program 1
# - Devices
gcc -c ../../../drivers/audio-linux-alsa/audio-linux-alsa.c -O3 -march=native -I ../../../devices/audio/

# - Infrastructural Program
gcc -c -O3 launcher.c -march=native -I ../../../devices/audio

# Program
gcc -c -O3 ../program.c -march=native -I ../../../devices/audio

# - Combine the whole program
gcc -o program program.o launcher.o audio-linux-alsa.o -lm -lasound
