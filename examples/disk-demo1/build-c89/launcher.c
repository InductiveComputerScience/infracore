#include <Disk.h>

#include "../../../drivers/disk-c89-file/disk-c89-file.h"

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

	success = CreateC89File(&disk1, "../disk.dat", 2, 512);

	if(success){
		Program(disk1);

		CloseC89File(disk1);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return !success;
}



