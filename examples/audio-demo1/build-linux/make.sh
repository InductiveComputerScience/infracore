# Program 1
# - Devices
gcc -c ../../../drivers/audio-linux-alsa/audio-linux-alsa.c -I ../../../devices/audio/

# - Infrastructural Program
gcc -c launcher.c -I ../../../devices/audio

# Program
gcc -c ../program.c -I ../../../devices/audio

# - Combine the whole program
gcc -o program program.o launcher.o audio-linux-alsa.o -lm -lasound
