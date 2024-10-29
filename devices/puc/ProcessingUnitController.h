#ifndef ProcessingUnitControllerH
#define ProcessingUnitControllerH

#include <stdint.h>
#include <stdbool.h>

struct ProcessingUnitControllerStructure{
	void *p;
};

typedef struct ProcessingUnitControllerStructure ProcessingUnitControllerStructure;

void GetProcessingUnitsAndPrograms(ProcessingUnitControllerStructure *puc, int64_t *pus, int64_t *ps);
void Start(ProcessingUnitControllerStructure *puc, int64_t n);
void Stop(ProcessingUnitControllerStructure *puc, int64_t n);
void SetProgram(ProcessingUnitControllerStructure *puc, int64_t n, int64_t p);
_Bool WasException(ProcessingUnitControllerStructure *puc);

#endif

