#ifndef DiskH
#define DiskH

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

struct DiskStructure{
	void *p;
};

typedef struct DiskStructure DiskStructure;

struct DiskServerStructure{
	void *p;
};

typedef struct DiskServerStructure DiskServerStructure;

void Entries(DiskStructure *disk, NumberReference *entries, NumberReference *entrysize);
_Bool Write(DiskStructure *disk, double entry, double *data, size_t dataLength);
_Bool Read(DiskStructure *disk, double entry, NumberArrayReference *data);

#endif
