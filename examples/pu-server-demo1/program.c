#include <ProcessingUnit.h>

#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// Test on linux:
// echo "              2hi" | nc -U socketfile

void Program(ProcessingUnitServerStructure *pu){
	NumberArrayReference message;
	char c;

	// Wait for and receive next request:
	ServerReceive(pu, &message);

	printf("Server got: \n");
	for(int i = 0; i < message.numberArrayLength; i++){
			printf("%c", (char)message.numberArray[i]);
	}
	printf("\n");

	// Convert to upper case.
	for(int i = 0; i < message.numberArrayLength; i++){
			c = (char)message.numberArray[i];
			if(c >= 'a' && c <= 'z'){
				c = c - 'a' + 'A';
			}
			message.numberArray[i] = c;
	}

	// Respond:
	ServerSend(pu, message.numberArray, message.numberArrayLength);
}










