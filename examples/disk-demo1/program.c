#include <Disk.h>

#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void Program(DiskStructure *disk){
	NumberReference entries, entrysize;
	_Bool success;
	NumberArrayReference data;
	int i;
	char str[100];

	Entries(disk, &entries, &entrysize);

	printf("Disk size: %ld bytes\n", (long)(entries.numberValue * entrysize.numberValue));

	data.numberArrayLength = entries.numberValue * entrysize.numberValue;
	data.numberArray = malloc(sizeof(double*) * data.numberArrayLength);

	success = Read(disk, 0, &data);

	for(i = 0; i < 13; i++){
		str[i] = (char)data.numberArray[i];
	}
	str[i] = 0;

	printf("Disk contents first 13 characters: %s\n", str);

	free(data.numberArray);
}










