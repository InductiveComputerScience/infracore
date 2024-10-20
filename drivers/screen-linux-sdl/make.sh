gcc -c screen-linux-sdl.c -O3 -D FIN=main -D FOUT=ScreenPUProgram -D WWIDTH=640 -D WHEIGHT=480 -I..
gcc -c ../demo.c -O3 -I..

gcc demo.o -o demo screen-linux-sdl.o -lSDL2main -lSDL2 -lpthread -lm
