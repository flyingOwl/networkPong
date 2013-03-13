#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "core.h"
#include "writer.h"
#include "package.h"
#include "structs.h"
#include "serialize.h"
#include "clientList.h"

struct clientList * nListe = 0;
struct clientList * lListe = 0;

int32_t  myIP   = 0;
uint16_t myPort = 0;

int virgin = 1;


int32_t charToIP(char * sIP){
	struct in_addr * ipAddress;
	struct hostent * hostp;
	hostp = gethostbyname(sIP);
	ipAddress = (struct in_addr*)hostp->h_addr_list[0];
	return ipAddress->s_addr;
}

char * intToIP(int32_t ip){
	unsigned char * temp = (unsigned char *) &ip,  i = 0;
	char * buffer = malloc(sizeof(char)*16);
	char * ptr = buffer;
	for(; i < 4; i++){
		ptr += sprintf(ptr,"%d",temp[i]);
		*(ptr) = (i < 3) ? '.' : '\0';
		ptr++;
	}
	return buffer;
}

int sendHello(char * sIP, int sPort, int myPort){
	/* sends an dummy package to specified client to get in pong loop */
	connecting(sIP,sPort);
	int32_t bIP = charToIP(sIP);
	int bufferSize = sizeof(int32_t) + 2*sizeof(uint16_t);
	unsigned char * buffer = malloc(bufferSize);
	
	*((uint16_t*) buffer) = htons((uint16_t) myPort);
	buffer += sizeof(uint16_t);
	struct clientList temp;
	temp.ipAddress = bIP;
	temp.port = htons((uint16_t) sPort);
	serialize_IPport(buffer,&temp);
	buffer -= sizeof(uint16_t);
	sendData(PACKAGE_TYPE_HELLO,bufferSize,buffer);
	free(buffer);

	return 0;
}



int setMyData(int32_t ip, uint16_t port){
	if(!myIP && !myPort){
		myIP = ip;
		myPort = port;
		/* ouput IP and port */
		char * sIP = intToIP(ip);
		printf("Recieving public address: %s:%d\n",sIP,port);
		free(sIP);
	}
	return 0;
}




int helloClient(int32_t ipAddress, uint16_t port){
	/* add client to client-list */
	if(!connectDirect(ipAddress, port)){
		/* connection succeed */
		int size = sizeof(int32_t) + sizeof(uint16_t);
		unsigned char * buffer = malloc(size);
		struct clientList * temp = createClient(ipAddress, port, 0);
		serialize_IPport(buffer, temp);
		if(sendData(PACKAGE_TYPE_HELLO_OK, size, buffer) == 0){
			char * cIP = intToIP(ipAddress);
			printf("Adding new client: %s:%d\n",cIP,port);
			free(cIP);
			nListe = addToList(ipAddress, port,nListe);
			if(virgin){ /* rape the shit out of the client */
				virgin = 0; /* hehe */
				uint32_t bigOne = htonl(0);
				puts("Calling loop()");
				loop(&bigOne);
			}
			free(buffer);
			free(temp);				
			return 0;
		}
		free(buffer);
		free(temp);
		return -5;
	} else {
		char * sIP = intToIP(ipAddress);
		printf("Can't connect to: %s:%d\n",sIP,port);
		free(sIP);
		return -3;
	}
}

int loop(void * dList){
	/* read each item from data-field and check if in list */
	virgin = 0;
	
	uint32_t nItems = ntohl(*((uint32_t *) dList));
	dList += sizeof(uint32_t);
	uint32_t i = 0;
	struct clientList * tItem = malloc(sizeof(struct clientList));
	struct clientList * tempPtr;
	for(; i < nItems; i++){
		tItem = restore_IPport(dList,tItem);
		tempPtr = containsElement(tItem->ipAddress,tItem->port, nListe);
		if(tempPtr){
			/* if item is already in the recieved list, remove it from newList */
			nListe = removeElement(nListe, tempPtr);
		}
		lListe = addToList(tItem->ipAddress,tItem->port,lListe);
		/* maybe we should move dList deeper into memory, instead of crying */
		dList += serialize_size_IPport();
	}

	printf("Recieved a list with %d elements\n",nItems);

	free(tItem);
	lListe = concatList(lListe, nListe); /* nList an lList anhängen */
	nListe = NULL; /* don't point somewhere in lListe */

	/* Waiting */
	usleep(DEFAULT_LOOP_PAUSE);
	
	/* try to send our list to next one in list */
	int connectOK;
	do {
		/* get first element in list and try connect */
		connectOK = connectDirect(lListe->ipAddress, lListe->port);
		if(!connectOK){ /* connect succeeds */
			char * cIP = intToIP(lListe->ipAddress);
			printf("Connected to: %s:%d\n",cIP, lListe->port);
			free(cIP);
		}
		lListe = removeElement(lListe, lListe); /* removes first element */		
	} while(lListe && connectOK);

	if(connectOK){
		puts("Can't connect to anyone in the list...");
		virgin = 1;
		destroyList(lListe);
		lListe = NULL;
		return -1;
	}

	lListe = addToList(myIP,myPort,lListe);
	/* we're connected to someone and have a working list... */
	
	/* put list in buffer */
	int listSize = countList(lListe);
	i = 0;
	int bufferSize = sizeof(uint32_t) + listSize*(sizeof(int32_t) + sizeof(uint16_t));
	unsigned char * buffer = malloc(bufferSize);
	unsigned char * dataPtr = buffer; // because buffer-ptr will grow
	/* write listSize in first bytes */
	*((uint32_t *) buffer) = htonl(listSize);
	buffer += sizeof(uint32_t);
	for(; i < listSize; i++){
		buffer = serialize_IPport(buffer,lListe);
		lListe = removeElement(lListe,lListe);
		/* ^ we don't need these elements any more... */
	}
	if(sendData(PACKAGE_TYPE_LOOP,bufferSize,dataPtr) == 0){
		/* send successfull */
		puts("List send successfully!");
	} else {
		puts("Loop send not successfull...");
		return 2;
	}
	free(dataPtr);
	destroyList(lListe);
	lListe = NULL;
	return 0;
}

