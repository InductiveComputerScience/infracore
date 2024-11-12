# Program 1
# - Devices
i686-w64-mingw32-gcc -c ../../../drivers/disk-c89-file/disk-c89-file.c -std=c89 -I ../../../devices/disk/

# - Infrastructural Program
i686-w64-mingw32-gcc -c launcher.c -I ../../../devices/disk

# Program
i686-w64-mingw32-gcc -c ../program.c -I ../../../devices/disk

# - Combine the whole program
i686-w64-mingw32-gcc -o program.exe program.o launcher.o disk-c89-file.o
