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
#include <sys/poll.h>
#include <signal.h>

#include <ProcessingUnit.h>

#include "pu-linux-sockets-server.h"

struct ProcessingUnitServerStructureSocket{
	int listenSocket;
	int clientSocket;
	struct sockaddr_un local;
	struct sockaddr_un remote;
	int len;
	_Bool connected;
};

typedef struct ProcessingUnitServerStructureSocket ProcessingUnitServerStructureSocket;

_Bool EnableKeepalive(int sock);

void signalhandler(int sig);

void signalhandler(int sig){
}

_Bool CreateLinuxSocketServer(ProcessingUnitServerStructure **pu, char *filename){
	ProcessingUnitServerStructureSocket *puS;
	bool success;
	int ret;

	success = true;

	// Catch SIGPIPE
	struct sigaction a;
	a.sa_handler = signalhandler;
	a.sa_flags = 0;
	sigemptyset(&a.sa_mask);
	sigaction(SIGPIPE, &a, NULL);

	*pu = malloc(sizeof(ProcessingUnitServerStructure)); // Alloc #1
	puS = malloc(sizeof(ProcessingUnitServerStructureSocket)); // Alloc #2
	(*pu)->p = puS;
	puS->connected = false;

	puS->listenSocket = socket(AF_LOCAL, SOCK_STREAM, 0);

	if(puS->listenSocket != -1){
		puS->local.sun_family = AF_LOCAL;
		if(strlen(filename) <= 107){
			strcpy(puS->local.sun_path, filename);
			puS->len = strlen(puS->local.sun_path) + sizeof(puS->local.sun_family);
			ret = bind(puS->listenSocket, (struct sockaddr *)&(puS->local), puS->len);

			if(ret == 0){
				ret = listen(puS->listenSocket, 1);
				if(ret == 0){
					success = true;
				}else{
					success = false;
					fprintf(stderr, "listen failed: %d\n", errno);
				}
			}else{
				success = false;
				fprintf(stderr, "bind failed: %d\n", errno);
			}
		}else{
			success = false;
			fprintf(stderr, "Socket name too long, max 107 characters: %s\n", filename);
		}
	}else{
		success = false;
		fprintf(stderr, "socket failed: %d\n", errno);
	}

	printf("Server success: %d socket %s\n", success, filename);

	return success;
}

void CloseLinuxSocketServer(ProcessingUnitServerStructure *pu){
	ProcessingUnitServerStructureSocket *puS;

	puS = pu->p;

	close(puS->listenSocket);
	unlink(puS->local.sun_path);

	free(pu); // Free #1
	free(puS); // Free #2
}

_Bool EnableKeepalive(int sock){
  int yes, r	;
	_Bool success;

  yes = 1;

	r = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));
  if(r != -1){
		success = true;
	}else{
		success = false;
	}

	return success;
}

#include "socklib.c"

void ServerSend(ProcessingUnitServerStructure *pu, double *message, size_t messageLength){
	ProcessingUnitServerStructureSocket *puS = (ProcessingUnitServerStructureSocket*)pu->p;
	double length = messageLength;
	char lengthString[20];
	uint8_t *buffer;
	long i;

	buffer = malloc(messageLength);

	for(i = 0; i < messageLength; i++){
		buffer[i] = message[i];
	}

	sprintf(lengthString, "%15Ld", (long long)messageLength);

	sendAll(puS->clientSocket, (uint8_t*)lengthString, 15);
	sendAll(puS->clientSocket, buffer, messageLength);

	free(buffer);
}

_Bool DoAcceptConnect(ProcessingUnitServerStructureSocket *puS){
	_Bool success;
	puS->clientSocket = accept(puS->listenSocket, (struct sockaddr *)&(puS->remote), &puS->len);
	if(puS->clientSocket != -1){
		success = EnableKeepalive(puS->clientSocket);
		if(success){
			puS->connected = true;
		}else{
			fprintf(stderr, "error enabling keep alive: %d\n", errno);
		}
	}else{
		success = false;
		fprintf(stderr, "accept failed: %d\n", errno);
	}
	return success;
}

void ServerReceive(ProcessingUnitServerStructure *pu, NumberArrayReference *message){
	ProcessingUnitServerStructureSocket *puS = (ProcessingUnitServerStructureSocket*)pu->p;
	double length;
	char lengthStr[15];
	_Bool success;
	uint8_t *buffer;
	long i;

	if(!puS->connected){
		success = DoAcceptConnect(puS);
	}

	success = false;
	while(!success){
		success = recvAll(puS->clientSocket, (uint8_t*)lengthStr, 15);
		if(!success){
			DoAcceptConnect(puS);
		}
	}

	if(success){
		length = atof(lengthStr);
	}

	message->numberArrayLength = length;
	message->numberArray = malloc(message->numberArrayLength * sizeof(double));

	buffer = malloc(length);
	recvAll(puS->clientSocket, buffer, length);

	for(i = 0; i < length; i++){
		message->numberArray[i] = buffer[i];
	}

	free(buffer);
}



bool ServerCheck(ProcessingUnitServerStructure *pu){
	ProcessingUnitServerStructureSocket *puS = (ProcessingUnitServerStructureSocket*)pu->p;
	int count, r;
	_Bool available, success, canAccept;
	struct pollfd fds[1];
	int nfds, timeout, rc;

	available = false;

	// Check if socket is closed
	if(puS->connected){
		char buf[1];
		r = write(puS->clientSocket, &buf, 0);
		if(r == -1){
			puS->connected = false;
		}
	}

	if(!puS->connected){
		// poll listening socket to see if a connection is ready.
	  memset(fds, 0 , sizeof(fds));
		nfds = 1;
		fds[0].fd = puS->listenSocket;
		fds[0].events = POLLIN;
		timeout = 0;
		rc = poll(fds, nfds, timeout);
		canAccept = rc == 1;

		if(canAccept){
			success = DoAcceptConnect(puS);
		}
	}

	if(puS->connected){
		r = ioctl(puS->clientSocket, FIONREAD, &count);
		//printf("ioctl: %d\n", r);
		available = count > 0;
	}

	return available;
}

void PUWaitForOneRecv(ProcessingUnitServerStructure **pu, size_t puLength){
	struct pollfd *fds;
	int nfds, timeout, rc;
	size_t i;
	ProcessingUnitServerStructureSocket *puS;

	fds = malloc(sizeof(struct pollfd) * puLength);
	nfds = puLength;

  memset(fds, 0, sizeof(struct pollfd) * puLength);

	for(i = 0; i < puLength; i++){
		puS = (ProcessingUnitServerStructureSocket*)pu[i]->p;

		if(puS->connected){
			fds[i].fd = puS->clientSocket;
		}else{
			fds[i].fd = puS->listenSocket;
		}

		fds[i].events = POLLIN;
	}


	timeout = 1 * 60 * 1000;
	poll(fds, nfds, timeout);

	free(fds);
}













