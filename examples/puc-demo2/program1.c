#include <ProcessingUnitController.h>

#include <unistd.h>
#include <stdio.h>

void Program1(ProcessingUnitControllerStructure *puc){
	int i;
	char *exceptionMessage;
	bool wasException;

	wasException = WasException(puc);

	if(wasException){
		exceptionMessage = "after exception";
	}else{
		exceptionMessage = "";
	}

	printf("Program 1 started %s\n", exceptionMessage);

	for(i = 0; i < 5; i++){
		printf("Program 1: %d\n", i);
		usleep(500 * 1000);
	}

	if(!wasException){
		printf("Program 1: Doing a null pointer dereference ...\n");

		int *x = NULL;
		*x = 5;
	}

	sleep(10000);
}
