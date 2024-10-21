#include <ProcessingUnit.h>

#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void Program(ProcessingUnitStructure *pu){
	NumberArrayReference message;
	char c;
	double *d;
	size_t dLength;
	char *str = "Hello, world!";
	long i;

	dLength = strlen(str);
	d = malloc(dLength * sizeof(double));
	for(i = 0; i < dLength; i++){
		d[i] = str[i];
	}

	Send(pu, d, dLength);
	Receive(pu, &message);

	printf("Response:\n");
	for(i = 0; i < message.numberArrayLength; i++){
		printf("%c", (char)message.numberArray[i]);
	}
	printf("\n");
}










