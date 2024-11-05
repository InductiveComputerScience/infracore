# Program 1
# - Devices
i686-w64-mingw32-gcc -c ../../../drivers/screen-windows-winapi/screen-windows-winapi.c -I ../../../devices/screen/

# - Infrastructural Program
i686-w64-mingw32-gcc -c launcher.c -I ../../../devices/screen

# Program
i686-w64-mingw32-gcc -c ../program.c -I ../../../devices/screen

# - Combine the whole program
i686-w64-mingw32-gcc -o program.exe program.o launcher.o screen-windows-winapi.o -lm -lgdi32

