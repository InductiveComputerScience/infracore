#include <ProcessingUnit.h>

#include "../../../drivers/pu-linux-sockets/pu-linux-sockets-client.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int main(){
	int success;
	int returnCode;
	ProcessingUnitStructure *client;

	success = CreateLinuxSocketClient(&client, "socketfile");

	if(success){
		Program(client);

		CloseLinuxSocketClient(client);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return !success;
}



