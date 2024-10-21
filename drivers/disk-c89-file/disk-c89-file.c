#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <Disk.h>

struct DiskStructureC89{
	FILE *diskFD;
	char *diskname;
	int64_t entries;
	int64_t entrySize;
};

typedef struct DiskStructureC89 DiskStructureC89;

int CreateC89File(DiskStructure **disk, char *disksrc, int64_t entries, int64_t entrySize){
	int success;
	DiskStructureC89 *diskS;

	*disk = malloc(sizeof(DiskStructure));
	diskS = malloc(sizeof(DiskStructureC89));
	(*disk)->p = diskS;

	diskS->diskname = disksrc;
	diskS->entries = entries;
	diskS->entrySize = entrySize;

	diskS->diskFD = fopen(diskS->diskname, "r+b");

	if(diskS->diskFD != NULL){
		success = 1;
	}else{
		success = 0;
	}

	return success;
}

int CloseC89File(DiskStructure *disk){
	DiskStructureC89 *diskS;

	diskS = (DiskStructureC89 *)disk->p;

	fclose(diskS->diskFD);

	free(diskS);
	free(disk);
}

void Entries(DiskStructure *disk, NumberReference *rentries, NumberReference *rentrysize){
	DiskStructureC89 *diskS = (DiskStructureC89 *)disk->p;

	rentries->numberValue = diskS->entries;
	rentrysize->numberValue = diskS->entrySize;
}

bool Write(DiskStructure *disk, double xentry, double *xdata, size_t dataLength){
	DiskStructureC89 *diskS = (DiskStructureC89 *)disk->p;
	bool success;
	int r;
	size_t entry, i, entrySize;
	uint8_t *data;

	entrySize = diskS->entrySize;

	entry = xentry;
	data = malloc(sizeof(uint8_t) * dataLength);
	for(i = 0; i < dataLength; i++){
		data[i] = xdata[i];
	}

	success = false;

	r = fseek(diskS->diskFD, entry * entrySize, SEEK_SET);
	if(r == 0){
		r = fwrite(data, entrySize, 1, diskS->diskFD);
		if(r == 1){
			r = fflush(diskS->diskFD);
			if(r == 0){
				success = true;
			}
		}
	}

	free(data);

	return success;
}

bool Read(DiskStructure *disk, double xentry, NumberArrayReference *xdata){
	DiskStructureC89 *diskS = (DiskStructureC89 *)disk->p;
	bool success;
	int r;
	uint8_t *data;
	size_t entry, i, entrySize;

	entrySize = diskS->entrySize;

	entry = xentry;
	data = malloc(sizeof(uint8_t) * xdata->numberArrayLength);

	success = false;

	r = fseek(diskS->diskFD, entry * entrySize, SEEK_SET);
	if(r == 0){
		r = fread(data, entrySize, 1, diskS->diskFD);
		if(r == 1){
			success = true;
		}
	}

	for(i = 0; i < xdata->numberArrayLength; i++){
		xdata->numberArray[i] = data[i];
	}

	free(data);

	return success;
}








