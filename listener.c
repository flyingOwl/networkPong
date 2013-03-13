#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "listener.h"
#include "package.h"
#include "core.h"
#include "serialize.h"

int listenerSocket;
int lPort = 0;

int initListener(int port){
	listenerSocket = socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in server;

	memset( &server, 0, sizeof (server));
	// IPv4-Adresse
	server.sin_family = AF_INET;
	// Jede IP-Adresse ist gültig
	server.sin_addr.s_addr = htonl( INADDR_ANY );

	server.sin_port = htons(port);
	lPort = port;

	if(bind( listenerSocket, (struct sockaddr*)&server, sizeof( server)) < 0) {
		return 1;
	}

	if(listen(listenerSocket,2)){
		return 1;
	}
	printf("Socket established: Listening on port %d\n",port);
	return 0;
}

int doData(int dataType, void * datas, struct sockaddr_in * cAddress){
	/* returns 0 or:
		1 - not specifies
		2 - wrong data format
	*/
	switch(dataType) {
		case PACKAGE_TYPE_PRE: {
			return 1;
		}
		case PACKAGE_TYPE_HELLO: {
			/* set new client inside core for next loop call... */
			uint16_t mPort = ntohs(*((uint16_t *) datas));
			struct clientList tempC;
			datas += sizeof(uint16_t);
			restore_IPport(datas, &tempC);
			setMyData(tempC.ipAddress, lPort);
			return helloClient(cAddress->sin_addr.s_addr,mPort);
		}
		case PACKAGE_TYPE_HELLO_OK: {
			struct clientList tempC;
			restore_IPport(datas, &tempC);
			setMyData(tempC.ipAddress, tempC.port);
			return 0;
		}
		case PACKAGE_TYPE_LOOP: {
			/* do some funny stuff... */
			return loop(datas);
		}
		default: {
			return 1;
		}
	}
	/* should never happen...: */
	return 1;
}

int recieveData(int cSocket, struct sockaddr_in * address){
	int preType, preLength, dataType;
	/* first packet... */
	int sizePre = sizeof(struct PACK_PRE);
	int result = 0;
	unsigned char * buffer = malloc(sizePre);
	if(buffer && recv(cSocket, buffer, sizePre,MSG_WAITALL)){
		// we recieved something...
		struct PACK_PRE preData;
		restore_prePack(buffer,&preData);
		// type should be "pre type"
		preType   = preData.type;
		dataType  = preData.dataType;
		preLength = preData.preLength;
		if(preType != PACKAGE_TYPE_PRE){
			free(buffer);
			return 1;
		}
	} else {
		return 2;
	}
	free(buffer);
	buffer = malloc(preLength);
	if(!buffer){
		return 3; //can't allocate memory
	}
	printf("Recieving %d bytes...\n",preLength);
	if(recv(cSocket, buffer, preLength, MSG_WAITALL) > 0){
		result = doData(dataType,buffer,address) + 10;
	} else {
		return 4;
	}
	free(buffer);
	return result;
}

void * listening(void * arg){
	struct sockaddr_in address;
	unsigned int addrlen, cSocket; 
	for (;;) {
	        addrlen = sizeof(struct sockaddr_in);
	        cSocket = accept(listenerSocket,(struct sockaddr *) &address, &addrlen);
	        if (cSocket < 0) {
			puts("Error in accept()... exit!");
			break;
	        }
		// Hier beginnt der Datenaustausch:
		if(recieveData(cSocket,&address) > 10){
			puts("Error in recieveData()... exit!");
			return NULL;
		}
		putchar('\n');
	}
	return NULL;
}
