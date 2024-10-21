#include <Clock.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int main(){
	int success;
	int returnCode;
	ClockStructure *clock;

	success = true;

	if(success){
		Program(clock);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return !success;
}



