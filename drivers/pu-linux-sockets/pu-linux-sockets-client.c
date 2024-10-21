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

struct ProcessingUnitStructureSocket{
	int serverSockets;
	struct sockaddr_un local;
	int len;
};

typedef struct ProcessingUnitStructureSocket ProcessingUnitStructureSocket;

void **devices;

int FOUT();

int FIN(){
	ProcessingUnitStructure *pu;
	ProcessingUnitStructureSocket *puS;
	bool success;
	int ret;

	success = true;

	#ifdef DEVINIT
	devices = malloc(DEVINIT * sizeof(void*)); // Alloc #1
	#endif

	pu = malloc(sizeof(ProcessingUnitStructure)); // Alloc #2
	devices[DEVICENR] = pu;
	puS = malloc(sizeof(ProcessingUnitStructureSocket)); // Alloc #3
	pu->p = puS;

	puS->serverSockets = socket(AF_LOCAL, SOCK_STREAM, 0);

	if(puS->serverSockets != -1){
		puS->local.sun_family = AF_LOCAL;
		strcpy(puS->local.sun_path, SOCKETNAME);
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

	printf("Client success: %d\n", success);

	if(success){
		FOUT();
	}

	close(puS->serverSockets);

	#ifdef DEVINIT
	free(devices); // Free #1
	#endif

	free(pu); // Alloc #2
	free(puS); // Alloc #3

	return 0;
}

#include "socklib.c"

void Send(ProcessingUnitStructure *pu, double *d, size_t dLength){
	ProcessingUnitStructureSocket *puS = (ProcessingUnitStructureSocket*)pu->p;
	int ret;
	double length = dLength;

	sendAll(puS->serverSockets, (uint8_t*)&length, sizeof(double));
	sendAll(puS->serverSockets, (uint8_t*)d, dLength * sizeof(double));
}

void Receive(ProcessingUnitStructure *pu, NumberArrayReference *d){
	ProcessingUnitStructureSocket *puS = (ProcessingUnitStructureSocket*)pu->p;
	double length;

	recvAll(puS->serverSockets, (uint8_t*)&length, sizeof(double));
	d->numberArrayLength = length;
	d->numberArray = malloc(d->numberArrayLength * sizeof(double));
	recvAll(puS->serverSockets, (uint8_t*)d->numberArray, d->numberArrayLength * sizeof(double));
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






















