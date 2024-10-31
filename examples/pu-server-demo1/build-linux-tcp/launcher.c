#include <ProcessingUnit.h>

#include "../../../drivers/pu-linux-tcp/pu-linux-tcp-server.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "../program.h"

int main(){
	int success;
	int returnCode;
	ProcessingUnitServerStructure *server;

	success = CreateLinuxTCPServer(&server, "127.0.0.2", 32343);

	if(success){
		Program(server);

		CloseLinuxTCPServer(server);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return returnCode;
}



