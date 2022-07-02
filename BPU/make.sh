gcc -S prog.c -O3 -march=native -masm=intel
gcc -S prog2.c -O3 -march=native -masm=intel
gcc -S test.c -O3 -march=native -masm=intel
gcc -c prog.s
gcc -c prog2.s
gcc -c test.s
gcc prog.o prog2.o test.o

gcc prog.c prog2.c test.c -g
