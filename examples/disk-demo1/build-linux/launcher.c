#include <Disk.h>

#include "../../../drivers/disk-linux-file/disk-linux-file.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int main(){
	int success;
	int returnCode;
	DiskStructure *disk1;

	success = CreateLinuxFile(&disk1, "../disk.dat", 2, 512);

	if(success){
		Program(disk1);

		CloseLinuxFile(disk1);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return !success;
}



