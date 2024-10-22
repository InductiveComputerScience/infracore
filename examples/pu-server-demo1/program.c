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
	ByteArrayReference message;
	char c;

	// Wait for and receive next request:
	ServerReceive(pu, &message);

	// Print message:
	printf("Server got: \n");
	for(int i = 0; i < message.byteArrayLength; i++){
			printf("%c", (char)message.byteArray[i]);
	}
	printf("\n");

	// Convert to upper case.
	for(int i = 0; i < message.byteArrayLength; i++){
			c = (char)message.byteArray[i];
			if(c >= 'a' && c <= 'z'){
				c = c - 'a' + 'A';
			}
			message.byteArray[i] = c;
	}

	// Respond:
	ServerSend(pu, message.byteArray, message.byteArrayLength);
}










