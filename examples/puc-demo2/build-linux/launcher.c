#include <ProcessingUnitController.h>

#include "../../../drivers/puc-linux-processes/linux-processes-puc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

void Program1();
void Program2();

static ProcessingUnitControllerStructure *puc;

void Program1Launcher(){
	Program1(puc);
}
void Program2Launcher(){
	Program2(puc);
}

int main(){
	int success;
	int returnCode;
	void (**programs)();

	programs = malloc(sizeof(void*));
	programs[0] = Program1Launcher;
	programs[1] = Program2Launcher;

	success = 1;
	CreateProcessingUnitControllerProcesses(&puc, 2, programs, 2);

	if(success){
		Program(puc);

		CloseProcessingUnitControllerProcesses(puc);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return returnCode;
}
