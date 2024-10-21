#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void Program(ClockStructure *clock){
	double resolution, range;
	double currentTime, lastTime;
	bool done;

	ClockSpecs(clock, &resolution, &range);

	currentTime = CurrentTime(clock, -2);

	printf("The current time is: %f\n", currentTime);
	printf("Waiting one second\n");

	lastTime = currentTime;
	done = false;
	for(; !done;){
		currentTime = CurrentTime(clock, -2);
		done = currentTime - lastTime > 1;
	}

	printf("The current time is: %f\n", currentTime);
}


