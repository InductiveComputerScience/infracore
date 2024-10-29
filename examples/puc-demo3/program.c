#include <ProcessingUnitController.h>

#include <unistd.h>

void Program(ProcessingUnitControllerStructure *puc){
	int64_t pus, ps;

	GetProcessingUnitsAndPrograms(puc, &pus, &ps);

	for(;;){
		SetProgram(puc, 0, 0);
		Start(puc, 0);
		sleep(5);
		Stop(puc, 0);

		SetProgram(puc, 0, 1);
		Start(puc, 0);
		sleep(5);
		Stop(puc, 0);
	}
	
}
