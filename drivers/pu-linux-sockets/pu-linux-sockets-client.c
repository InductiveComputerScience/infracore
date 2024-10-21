#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <ProcessingUnit.h>

#include "pu-linux-sockets-client.h"

struct ProcessingUnitStructureSocket{
	int serverSockets;
	struct sockaddr_un local;
	int len;
};

typedef struct ProcessingUnitStructureSocket ProcessingUnitStructureSocket;

_Bool CreateLinuxSocketClient(ProcessingUnitStructure **pu, char *filename){
	ProcessingUnitStructureSocket *puS;
	bool success;
	int ret;

	success = true;

	*pu = malloc(sizeof(ProcessingUnitStructure)); // Alloc #1
	puS = malloc(sizeof(ProcessingUnitStructureSocket)); // Alloc #1
	(*pu)->p = puS;

	puS->serverSockets = socket(AF_LOCAL, SOCK_STREAM, 0);

	if(puS->serverSockets != -1){
		puS->local.sun_family = AF_LOCAL;
		strcpy(puS->local.sun_path, filename);
		puS->len = strlen(puS->local.sun_path) + sizeof(puS->local.sun_family);

	  ret = connect(puS->serverSockets, (struct sockaddr *)&puS->local, puS->len);

		if(ret == 0){
		}else{
			success = false;
			printf("connect failed: %d\n", errno);
		}
	}else{
		success = false;
		printf("socket failed: %d\n", errno);
	}

	//printf("Client success: %d\n", success);

	return success;
}

void CloseLinuxSocketClient(ProcessingUnitStructure *pu){
	ProcessingUnitStructureSocket *puS;

	puS = (ProcessingUnitStructureSocket*)pu->p;

	close(puS->serverSockets);

	free(pu); // Free #1
	free(puS); // Free #2
}

#include "socklib.c"

void Send(ProcessingUnitStructure *pu, double *message, size_t messageLength){
	ProcessingUnitStructureSocket *puS = (ProcessingUnitStructureSocket*)pu->p;
	double length = messageLength;
	char lengthString[20];
	uint8_t *buffer;
	long i;

	buffer = malloc(messageLength);

	for(i = 0; i < messageLength; i++){
		buffer[i] = message[i];
	}

	sprintf(lengthString, "%15Ld", (long long)messageLength);

	sendAll(puS->serverSockets, (uint8_t*)lengthString, 15);
	sendAll(puS->serverSockets, buffer, messageLength);

	free(buffer);
}

void Receive(ProcessingUnitStructure *pu, NumberArrayReference *message){
	ProcessingUnitStructureSocket *puS = (ProcessingUnitStructureSocket*)pu->p;
	double length;
	char lengthStr[15];
	_Bool success;
	uint8_t *buffer;
	long i;

	success = recvAll(puS->serverSockets, (uint8_t*)lengthStr, 15);

	if(success){
		length = atof(lengthStr);
	}

	message->numberArrayLength = length;
	message->numberArray = malloc(message->numberArrayLength * sizeof(double));

	buffer = malloc(length);
	recvAll(puS->serverSockets, buffer, length);

	for(i = 0; i < length; i++){
		message->numberArray[i] = buffer[i];
	}

	free(buffer);
}

bool Check(ProcessingUnitStructure *pu){
	ProcessingUnitStructureSocket *puS = (ProcessingUnitStructureSocket*)pu->p;
	int count;
	ioctl(puS->serverSockets, FIONREAD, &count);
	return count > 0;
}

void Call(ProcessingUnitStructure *pu, double *s, size_t sLength, NumberArrayReference *d){
	Send(pu, s, sLength);
	Receive(pu, d);
}






















