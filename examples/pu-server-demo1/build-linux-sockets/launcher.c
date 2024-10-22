#include <ProcessingUnit.h>

#include "../../../drivers/pu-linux-sockets/pu-linux-sockets-server.h"

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

	success = CreateLinuxSocketServer(&server, "socketfile");

	if(success){
		Program(server);

		CloseLinuxSocketServer(server);
	}

	if(success){
		returnCode = 0;
	}else{
		returnCode = 1;
	}

	return !success;
}



