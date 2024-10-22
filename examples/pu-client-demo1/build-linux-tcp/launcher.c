#include <ProcessingUnit.h>

#include "../../../drivers/pu-linux-tcp/pu-linux-tcp-client.h"

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

	success = CreateLinuxTCPClient(&client, "127.0.0.2", 32343);

	if(success){
		Program(client);

		CloseLinuxTCPClient(client);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return !success;
}



