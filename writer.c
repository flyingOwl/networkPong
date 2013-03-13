#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "writer.h"
#include "package.h"
#include "structs.h"
#include "core.h"
#include "serialize.h"

int writerSocket = 0;

int initWriter(){
	writerSocket = socket(AF_INET,SOCK_STREAM,0);
	return (writerSocket == 0);
}

int closeSession(){
	int temp = writerSocket;
	writerSocket = 0;
	return close(temp);
}

int connectDirect(int32_t IP, uint16_t port){
	initWriter();
	if(writerSocket){
		struct sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		server.sin_addr.s_addr = IP;
		int result = connect(writerSocket, (struct sockaddr *) &server, sizeof(server));
		if(result < 0){
			char * cIP = intToIP(IP);
			printf("connect() %s:%d: %s\n",cIP, port, strerror(errno));
			free(cIP);
		}
		return result;
	}
	return -3;
}

int connecting(char * IP, int sPort){
	if(sPort){
		struct in_addr * ipAddress;
		struct hostent * hostp;

		hostp = gethostbyname(IP);
		ipAddress = (struct in_addr*)hostp->h_addr_list[0];

		return connectDirect(ipAddress->s_addr, sPort);
	}
	return 1;
}

int sendData(int data_type, int data_length, void * data){
	/* send pre-package */
	if(!writerSocket){
		return -3;
	}
	struct PACK_PRE pPre;
	int size = sizeof(struct PACK_PRE), sended = 0;
	pPre.type      = PACKAGE_TYPE_PRE;
	pPre.dataType  = data_type;
	pPre.preLength = data_length;
	
	unsigned char * buffer = malloc(size);
	serialize_prePack(buffer,&pPre);

	if(send(writerSocket,buffer,size,0) > 0){
		// pre-package successfully send:
		/* send data-package */
		sended = send(writerSocket,data,data_length,0);
	}
	free(buffer);
	closeSession();
	return (sended != data_length);
}

