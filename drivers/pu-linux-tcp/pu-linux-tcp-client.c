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
#include <semaphore.h>
#include <pthread.h>

#include <ProcessingUnit.h>

#include "pu-linux-tcp-client.h"

#include "socklib.c"

struct ProcessingUnitStructureTCP{
	int serverSockets;
	struct sockaddr_in local;
	int len;
	bool connected;
	bool sending;
	bool sendSuccess;
	pthread_t clientThread;
	sem_t sendrdy, recvrdy;
	uint8_t *message;
	size_t messageLength;
};

typedef struct ProcessingUnitStructureTCP ProcessingUnitStructureTCP;

void *TCPLoop(void *pu);

bool CreateLinuxTCPClient(ProcessingUnitStructure **pu, char *ip, int port){
	ProcessingUnitStructureTCP *puS;
	bool success;
	int ret;

	// Create Structure
	*pu = malloc(sizeof(ProcessingUnitStructure)); // Alloc #1
	puS = malloc(sizeof(ProcessingUnitStructureTCP)); // Alloc #1
	(*pu)->p = puS;

	sem_init(&puS->sendrdy, 0, 0);
	sem_init(&puS->recvrdy, 0, 0);

	puS->serverSockets = socket(AF_INET, SOCK_STREAM, 0);

	if(puS->serverSockets != -1){
    bzero(&puS->local, sizeof(puS->local));

    puS->local.sin_family = AF_INET;
    puS->local.sin_addr.s_addr = inet_addr(ip);
    puS->local.sin_port = htons(port);

		puS->len = sizeof(puS->local);

		puS->connected = false;
		puS->sending = false;
		puS->sendSuccess = false;

		// Start client thread
		ret = pthread_create(&puS->clientThread, NULL, TCPLoop, puS);
		if(ret == 0){
			success = true;
		}else{
				printf("pthread_create failed %d \"%s\"\n", errno, strerror(errno));
				success = false;
		}
	}else{
		success = false;
		printf("socket failed: %d \"%s\"\n", errno, strerror(errno));
	}

	return success;
}

void *TCPLoop(void *pu){
	ProcessingUnitStructureTCP *puS;
	bool done;
	int ret;
	char lengthString[20];
	bool success;

	puS = (ProcessingUnitStructureTCP*)pu;

	done = false;
	while(!done){
		// Not sending
		puS->sending = false;
		puS->sendSuccess = false;

		// Connect if not connected.
		if(!puS->connected){
			ret = connect(puS->serverSockets, (struct sockaddr *)&puS->local, puS->len);

			if(ret == 0){
				puS->connected = true;
			}else{
				puS->connected = false;
			}
		}

		if(puS->connected){
			// Send
			sem_wait(&puS->sendrdy);

			sprintf(lengthString, "%15Ld", (long long)puS->messageLength);

			sendAll(puS->serverSockets, (uint8_t*)lengthString, 15);
			sendAll(puS->serverSockets, puS->message, puS->messageLength);

			puS->sendSuccess = true;
			puS->sending = false;

			// Receive
			success = recvAll(puS->serverSockets, (uint8_t*)lengthString, 15);

			if(success){
				puS->messageLength = atof(lengthString);
			}

			puS->message = malloc(puS->messageLength);

			recvAll(puS->serverSockets, puS->message, puS->messageLength);

			sem_post(&puS->recvrdy);
		}
	}

	// If connected, disconnect.
	if(puS->connected){
		close(puS->serverSockets);
		puS->connected = false;

		sem_destroy(&puS->sendrdy);
		sem_destroy(&puS->recvrdy);
	}

	pthread_exit(NULL);
}

void CloseLinuxTCPClient(ProcessingUnitStructure *pu){
	ProcessingUnitStructureTCP *puS;

	puS = (ProcessingUnitStructureTCP*)pu->p;

	close(puS->serverSockets);

	free(pu); // Free #1
	free(puS); // Free #2
}

void Send(ProcessingUnitStructure *pu, uint8_t *message, size_t messageLength){
	ProcessingUnitStructureTCP *puS;

	puS = (ProcessingUnitStructureTCP*)pu->p;
	puS->message = message;
	puS->messageLength = messageLength;

	sem_post(&puS->sendrdy);
}

void Receive(ProcessingUnitStructure *pu, ByteArrayReference *message){
	ProcessingUnitStructureTCP *puS;

	puS = (ProcessingUnitStructureTCP*)pu->p;

	sem_wait(&puS->recvrdy);

	message->byteArray = puS->message;
	message->byteArrayLength = puS->messageLength;
}

bool Check(ProcessingUnitStructure *pu){
	bool available;
	ProcessingUnitStructureTCP *puS;
	int ret, val;

	puS = (ProcessingUnitStructureTCP*)pu->p;

	ret = sem_getvalue(&puS->recvrdy, &val);

	available = val > 0;

	return available;
}

bool CheckSend(ProcessingUnitStructure *pu){
	ProcessingUnitStructureTCP *puS;

	puS = (ProcessingUnitStructureTCP*)pu->p;

	return puS->sendSuccess;
}

void Call(ProcessingUnitStructure *pu, uint8_t *s, size_t sLength, ByteArrayReference *d){
	Send(pu, s, sLength);
	Receive(pu, d);
}






















