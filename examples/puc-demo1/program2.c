#include <unistd.h>
#include <stdio.h>

void Program2(){
	int i;

	for(i = 0; i < 100; i++){
		printf("Program 2: %d\n", i);
		usleep(500 * 1000);
	}

	sleep(10000);
}
