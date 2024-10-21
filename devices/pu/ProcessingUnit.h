#ifndef ProcessingUnitH
#define ProcessingUnitH

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef NumbeRefH
#define NumbeRefH

struct NumberReference{
  double numberValue;
};
typedef struct NumberReference NumberReference;
struct NumberArrayReference{
  double *numberArray;
  size_t numberArrayLength;
};
typedef struct NumberArrayReference NumberArrayReference;

#endif

struct ProcessingUnitStructure{
	void *p;
};

typedef struct ProcessingUnitStructure ProcessingUnitStructure;

struct ProcessingUnitServerStructure{
	void *p;
};

typedef struct ProcessingUnitServerStructure ProcessingUnitServerStructure;

// Processing Unit Instructions
void Call(ProcessingUnitStructure *pu, double *s, size_t sLength, NumberArrayReference *r);

void Send(ProcessingUnitStructure *pu, double *d, size_t dLength);
bool Check(ProcessingUnitStructure *pu);
void Receive(ProcessingUnitStructure *pu, NumberArrayReference *d);

void ServerSend(ProcessingUnitServerStructure *pu, double *message, size_t messageLength);
bool ServerCheck(ProcessingUnitServerStructure *pu);
void ServerReceive(ProcessingUnitServerStructure *pu, NumberArrayReference *message);

#endif
