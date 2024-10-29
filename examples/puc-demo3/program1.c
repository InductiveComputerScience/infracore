#include <unistd.h>
#include <stdio.h>

void Program1(){
	int i;

	for(i = 0; i < 10; i++){
		printf("Program 1: %d\n", i);
		sleep(1);
	}

	sleep(10000);
}
