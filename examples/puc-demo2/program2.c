#include <ProcessingUnitController.h>

#include <unistd.h>
#include <stdio.h>

void Program2(ProcessingUnitControllerStructure *puc){
	int i;
	char *exceptionMessage;
	bool wasException;

	wasException = WasException(puc);

	if(wasException){
		exceptionMessage = "after exception";
	}else{
		exceptionMessage = "";
	}

	printf("Program 2 started %s\n", exceptionMessage);

	for(i = 0; i < 7; i++){
		printf("Program 2: %d\n", i);
		usleep(500 * 1000);
	}

	if(!wasException){
		printf("Program 2: Dividing by zero ...\n");

		int a = 5, b = 0, x;
		x = a / b;
	}

	sleep(10000);
}
