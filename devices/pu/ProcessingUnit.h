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

#ifndef ByteArrayRefH
#define ByteArrayRefH

struct ByteArrayReference{
  uint8_t *byteArray;
  size_t byteArrayLength;
};
typedef struct ByteArrayReference ByteArrayReference;

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
void Call(ProcessingUnitStructure *pu, uint8_t *s, size_t sLength, ByteArrayReference *r);

void Send(ProcessingUnitStructure *pu, uint8_t *d, size_t dLength);
bool Check(ProcessingUnitStructure *pu);
void Receive(ProcessingUnitStructure *pu, ByteArrayReference *d);

void ServerSend(ProcessingUnitServerStructure *pu, uint8_t *message, size_t messageLength);
bool ServerCheck(ProcessingUnitServerStructure *pu);
void ServerReceive(ProcessingUnitServerStructure *pu, ByteArrayReference *message);

#endif
