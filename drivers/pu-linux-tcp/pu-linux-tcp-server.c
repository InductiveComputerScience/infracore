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

#include "pu-linux-tcp-server.h"

struct ProcessingUnitServerStructureTCP{
	int listenSocket;
	int clientSocket;
	struct sockaddr_in local;
	struct sockaddr_in remote;
	int len;
	_Bool connected;
};

typedef struct ProcessingUnitServerStructureTCP ProcessingUnitServerStructureTCP;

_Bool EnableKeepalive(int sock);

void signalhandler(int sig);

void signalhandler(int sig){
}

_Bool CreateLinuxTCPServer(ProcessingUnitServerStructure **pu, char *ip, int port){
	ProcessingUnitServerStructureTCP *puS;
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
	puS = malloc(sizeof(ProcessingUnitServerStructureTCP)); // Alloc #2
	(*pu)->p = puS;
	puS->connected = false;

	puS->listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(puS->listenSocket != -1){
		bzero(&puS->local, sizeof(puS->local)); 
    puS->local.sin_family = AF_INET; 
    puS->local.sin_addr.s_addr = inet_addr(ip); 
    puS->local.sin_port = htons(port);

		puS->len = sizeof(struct sockaddr_in);

		// if(strlen(filename) <= 107){ // Check valid IP and port.
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
		//}else{
		//	success = false;
		//	fprintf(stderr, "Socket name too long, max 107 characters: %s\n", filename);
		//}
	}else{
		success = false;
		fprintf(stderr, "socket failed: %d\n", errno);
	}

	printf("Server success: %d tcp %s:%d\n", success, ip, port);

	return success;
}

void CloseLinuxTCPServer(ProcessingUnitServerStructure *pu){
	ProcessingUnitServerStructureTCP *puS;

	puS = pu->p;

	close(puS->listenSocket);

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

void ServerSend(ProcessingUnitServerStructure *pu, uint8_t *message, size_t messageLength){
	ProcessingUnitServerStructureTCP *puS = (ProcessingUnitServerStructureTCP*)pu->p;
	char lengthString[20];
	long i;

	sprintf(lengthString, "%15Ld", (long long)messageLength);

	sendAll(puS->clientSocket, (uint8_t*)lengthString, 15);
	sendAll(puS->clientSocket, message, messageLength);
}

_Bool DoAcceptConnect(ProcessingUnitServerStructureTCP *puS){
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

void ServerReceive(ProcessingUnitServerStructure *pu, ByteArrayReference *message){
	ProcessingUnitServerStructureTCP *puS;
	double length;
	char lengthStr[15];
	_Bool success;
	uint8_t *buffer;
	long i;

	puS = (ProcessingUnitServerStructureTCP*)pu->p;

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

	message->byteArrayLength = length;
	message->byteArray = malloc(message->byteArrayLength);

	recvAll(puS->clientSocket, message->byteArray, length);
}



bool ServerCheck(ProcessingUnitServerStructure *pu){
	ProcessingUnitServerStructureTCP *puS = (ProcessingUnitServerStructureTCP*)pu->p;
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
	ProcessingUnitServerStructureTCP *puS;

	fds = malloc(sizeof(struct pollfd) * puLength);
	nfds = puLength;

  memset(fds, 0, sizeof(struct pollfd) * puLength);

	for(i = 0; i < puLength; i++){
		puS = (ProcessingUnitServerStructureTCP*)pu[i]->p;

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













