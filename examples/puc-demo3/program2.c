#include <unistd.h>
#include <stdio.h>

void Program2(){
	int i;

	for(i = 0; i < 10; i++){
		printf("Program 2: %d\n", i);
		sleep(1);
	}

	sleep(10000);
}
