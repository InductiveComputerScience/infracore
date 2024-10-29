#include <unistd.h>
#include <stdio.h>

void Program1(){
	int i;

	for(i = 0; i < 100; i++){
		printf("Program 1: %d\n", i);
		usleep(500 * 1000);
	}

	sleep(10000);
}
