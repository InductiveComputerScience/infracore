#ifndef DiskH
#define DiskH

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

struct NumberReference;
typedef struct NumberReference NumberReference;
struct NumberArrayReference;
typedef struct NumberArrayReference NumberArrayReference;

struct DiskStructure{
	void *p;
};

typedef struct DiskStructure DiskStructure;

struct DiskServerStructure{
	void *p;
};

typedef struct DiskServerStructure DiskServerStructure;

/* Disk Controller Instructions */

/* Synchronous */
void Entries(DiskStructure *disk, NumberReference *entries, NumberReference *entrysize);
_Bool Write(DiskStructure *disk, double entry, double *data, size_t dataLength);
_Bool Read(DiskStructure *disk, double entry, NumberArrayReference *data);

/* Message */
/*
#pragma pack(push)
#pragma pack(1)
typedef struct{
	double operation; /* 1=Entries, 2=Write, 3=Read * /
	double arg1, arg2;
	_Bool success;
	double data[];
} DiskMessage;
#pragma pack(pop)
*/
/* Asynchronous */
/*
void SendEntries(DiskStructure *disk);
void ReceiveEntries(DiskStructure *disk, NumberReference *entries, NumberReference *entrysize);
void ServerSendEntries(DiskServerStructure *disk, double entries, double entrysize);
_Bool ServerReceiveEntries(DiskServerStructure *disk);

void SendWrite(DiskStructure *disk, double entry, double *data, size_t dataLength);
_Bool ReceiveWrite(DiskStructure *disk);
void ServerSendWrite(DiskServerStructure *disk, _Bool success);
_Bool ServerReceiveWrite(DiskServerStructure *disk, NumberReference *entry, NumberArrayReference *data);

void SendRead(DiskStructure *disk, double entry);
_Bool ReceiveRead(DiskStructure *disk, NumberArrayReference *data);
void ServerSendRead(DiskServerStructure *disk, double *data, size_t dataLength, _Bool success);
_Bool ServerReceiveRead(DiskServerStructure *disk, NumberReference *entry);
*/

#endif
