#include <ProcessingUnit.h>

#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void Program(ProcessingUnitStructure *pu){
	ByteArrayReference message;
	char c;
	size_t dLength;
	char *str = "Hello, world!";
	long i;

	Send(pu, (uint8_t*)str, strlen(str));
	Receive(pu, &message);

	// Equivalently:
	// Call(pu, (uint8_t*)str, strlen(str), &message);

	printf("Response:\n");
	for(i = 0; i < message.byteArrayLength; i++){
		printf("%c", (char)message.byteArray[i]);
	}
	printf("\n");
}










