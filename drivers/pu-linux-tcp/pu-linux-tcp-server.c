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
#include <signal.h>

#include <ProcessingUnit.h>

#include "pu-linux-tcp-server.h"

#include "socklib.c"

struct ProcessingUnitServerStructureTCP{
	int listenSocket;
	int clientSocket;
	struct sockaddr_in local;
	struct sockaddr_in remote;
	int len;
	bool connected;
	bool sending;
	bool sendSuccess;
	pthread_t serverThread;
	sem_t sendrdy, recvrdy;
	uint8_t *message;
	size_t messageLength;
	bool done;
};

bool EnableKeepalive(int sock);

void signalhandler(int sig){
}

typedef struct ProcessingUnitServerStructureTCP ProcessingUnitServerStructureTCP;

void *ServerThread(void *pu);

bool CreateLinuxTCPServer(ProcessingUnitServerStructure **pu, char *ip, int port){
	ProcessingUnitServerStructureTCP *puS;
	bool success;
	int ret;

	// Catch SIGPIPE
	struct sigaction a;
	a.sa_handler = signalhandler;
	a.sa_flags = 0;
	sigemptyset(&a.sa_mask);
	sigaction(SIGPIPE, &a, NULL);

	// Create Structure
	*pu = malloc(sizeof(ProcessingUnitServerStructure)); // Alloc #1
	puS = malloc(sizeof(ProcessingUnitServerStructureTCP)); // Alloc #2
	(*pu)->p = puS;

	sem_init(&puS->sendrdy, 0, 0);
	sem_init(&puS->recvrdy, 0, 0);

	puS->listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	int val = 1;
	setsockopt(puS->listenSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

	if(puS->listenSocket != -1){
    bzero(&puS->local, sizeof(puS->local));

    puS->local.sin_family = AF_INET;
    puS->local.sin_addr.s_addr = inet_addr(ip);
    puS->local.sin_port = htons(port);

		puS->len = sizeof(puS->local);

		puS->connected = false;
		puS->sending = false;
		puS->sendSuccess = false;

		// if(IsValidIP(ip) && IsValidPort(port)){ // Check valid IP and port.
			ret = bind(puS->listenSocket, (struct sockaddr *)&(puS->local), puS->len);

			if(ret == 0){
				ret = listen(puS->listenSocket, 1);
				if(ret == 0){
					// Start server thread
					ret = pthread_create(&puS->serverThread, NULL, ServerThread, puS);
					if(ret == 0){
						success = true;
					}else{
							printf("pthread_create failed %d \"%s\"\n", errno, strerror(errno));
							success = false;
					}

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
		//	fprintf(stderr, "IP or port number not valid: %s, %d\n", ip, port);
		//}
	}else{
		success = false;
		printf("socket failed: %d \"%s\"\n", errno, strerror(errno));
	}

	return success;
}

void *ServerThread(void *pu){
	ProcessingUnitServerStructureTCP *puS;
	int ret;
	char lengthString[20];
	bool success;

	puS = (ProcessingUnitServerStructureTCP*)pu;

	puS->done = false;
	while(!puS->done){
		// Accept new connection.
		if(!puS->connected){
			puS->clientSocket = accept(puS->listenSocket, (struct sockaddr *)&(puS->remote), &puS->len);
			if(puS->clientSocket != -1){
				success = true;
				puS->connected = true;
				EnableKeepalive(puS->clientSocket);
			}else{
				success = false;
				fprintf(stderr, "accept failed: %d\n", errno);
				puS->connected = false;
				sleep(1);
			}
		}

		if(puS->connected && !puS->done){
			// Receive
			success = recvAll(puS->clientSocket, (uint8_t*)lengthString, 15);

			if(success){
				puS->messageLength = atof(lengthString);

				puS->message = malloc(puS->messageLength);

				success = recvAll(puS->clientSocket, puS->message, puS->messageLength);
				if(success){
					sem_post(&puS->recvrdy);

					// Send
					sem_wait(&puS->sendrdy);
					puS->sending = true;
					puS->sendSuccess = false;

					if(!puS->done){
						sprintf(lengthString, "%15Ld", (long long)puS->messageLength);

						success = sendAll(puS->clientSocket, (uint8_t*)lengthString, 15);
						if(success){
							success = sendAll(puS->clientSocket, puS->message, puS->messageLength);
						}

						puS->sending = false;
						puS->sendSuccess = true;
					}
				}
			}
		}
	}

	// If connected, disconnect.
	if(puS->connected){
		close(puS->clientSocket);
		puS->connected = false;
	}
}

void CloseLinuxTCPServer(ProcessingUnitServerStructure *pu){
	ProcessingUnitServerStructureTCP *puS;

	puS = (ProcessingUnitServerStructureTCP*)pu->p;

	puS->done = true;

	sem_post(&puS->recvrdy);
	sem_post(&puS->sendrdy);

	// If connected, shut down connection.
	if(puS->connected){
		shutdown(puS->clientSocket, SHUT_RDWR);
	}

	close(puS->listenSocket);

	pthread_join(puS->serverThread, NULL);

	sem_destroy(&puS->sendrdy);
	sem_destroy(&puS->recvrdy);

	free(pu); // Free #1
	free(puS); // Free #2
}

void ServerSend(ProcessingUnitServerStructure *pu, uint8_t *message, size_t messageLength){
	ProcessingUnitServerStructureTCP *puS;

	puS = (ProcessingUnitServerStructureTCP*)pu->p;
	puS->message = message;
	puS->messageLength = messageLength;

	sem_post(&puS->sendrdy);
}

void ServerReceive(ProcessingUnitServerStructure *pu, ByteArrayReference *message){
	ProcessingUnitServerStructureTCP *puS;

	puS = (ProcessingUnitServerStructureTCP*)pu->p;

	sem_wait(&puS->recvrdy);

	message->byteArray = puS->message;
	message->byteArrayLength = puS->messageLength;
}

bool ServerCheck(ProcessingUnitServerStructure *pu){
	bool available;
	ProcessingUnitServerStructureTCP *puS;
	int ret, val;

	puS = (ProcessingUnitServerStructureTCP*)pu->p;

	ret = sem_getvalue(&puS->recvrdy, &val);

	available = val > 0;

	return available;
}

bool ServerCheckSend(ProcessingUnitServerStructure *pu){
	ProcessingUnitServerStructureTCP *puS;

	puS = (ProcessingUnitServerStructureTCP*)pu->p;

	return puS->sendSuccess;
}

bool EnableKeepalive(int sock){
  int yes, r	;
	bool success;

  yes = 1;

	r = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));
  if(r != -1){
		success = true;
	}else{
		success = false;
	}

	return success;
}

/*

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
	bool connected;
};

typedef struct ProcessingUnitServerStructureTCP ProcessingUnitServerStructureTCP;

bool EnableKeepalive(int sock);

void signalhandler(int sig){
}

bool CreateLinuxTCPServer(ProcessingUnitServerStructure **pu, char *ip, int port){
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

		// if(IsValidIP(ip) && IsValidPort(port)){ // Check valid IP and port.
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
		//	fprintf(stderr, "IP or port number not valid: %s, %d\n", ip, port);
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

#include "socklib.c"

void ServerSend(ProcessingUnitServerStructure *pu, uint8_t *message, size_t messageLength){
	ProcessingUnitServerStructureTCP *puS = (ProcessingUnitServerStructureTCP*)pu->p;
	char lengthString[20];
	long i;

	sprintf(lengthString, "%15Ld", (long long)messageLength);

	sendAll(puS->clientSocket, (uint8_t*)lengthString, 15);
	sendAll(puS->clientSocket, message, messageLength);
}

bool DoAcceptConnect(ProcessingUnitServerStructureTCP *puS){
	bool success;
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
	bool success;
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

bool ServerCheckSend(ProcessingUnitServerStructure *pu){
	bool available;

	return available;
}

bool ServerCheck(ProcessingUnitServerStructure *pu){
	ProcessingUnitServerStructureTCP *puS = (ProcessingUnitServerStructureTCP*)pu->p;
	int count, r;
	bool available, success, canAccept;
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

*/













