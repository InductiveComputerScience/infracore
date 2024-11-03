# Program 1
# - Devices
gcc -g -c ../../../drivers/audio-linux-alsa/audio-linux-alsa.c -I ../../../devices/audio/

# - Infrastructural Program
gcc -g -c launcher.c -I ../../../devices/audio

# Program
nasm -f elf64 ../program.asm -o program.o -i ../../../devices/audio/

# - Combine the whole program
gcc -no-pie -o program program.o launcher.o audio-linux-alsa.o -lm -lasound
