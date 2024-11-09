# Program 1
# - Devices
i686-w64-mingw32-gcc -c ../../../drivers/audio-windows-waveform/audio-windows-waveform.c -I ../../../devices/audio/

# - Infrastructural Program
i686-w64-mingw32-gcc -c launcher.c -I ../../../devices/audio

# Program
i686-w64-mingw32-gcc -c ../program.c -I ../../../devices/audio

# - Combine the whole program
i686-w64-mingw32-gcc -o program.exe program.o launcher.o audio-windows-waveform.o -lm -lwinmm

