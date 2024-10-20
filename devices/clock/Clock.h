#ifndef ClockH
#define ClockH

#include <stdint.h>

struct ClockStructure{
	void *p;
};

typedef struct ClockStructure ClockStructure;

void ClockSpecs(ClockStructure *clock, double *resolution, double *range);
double CurrentTime(ClockStructure *clock, double resolution);

#endif
