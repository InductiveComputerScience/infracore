#include <ProcessingUnit.h>

#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void Program(ProcessingUnitServerStructure *pu){
	NumberArrayReference message;

	ServerReceive(pu, &message);

	ServerSend(pu, message.numberArray, message.numberArrayLength);
}










