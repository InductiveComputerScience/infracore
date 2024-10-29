#include <ProcessingUnitController.h>

#include <unistd.h>
#include <stdio.h>

void Program(ProcessingUnitControllerStructure *puc){
	int64_t pus, ps;

	GetProcessingUnitsAndPrograms(puc, &pus, &ps);

	printf("Controller 1: PUs: %ld, Programs: %ld\n", pus, ps);

	printf("Starting program 1 on PU 1.\n");
	SetProgram(puc, 0, 0);
	Start(puc, 0);

	sleep(1);

	printf("Starting program 2 on PU 2.\n");
	SetProgram(puc, 1, 1);
	Start(puc, 1);

	sleep(5);

	printf("Exiting -- programs will be terminated\n");
}
