#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "listener.h"
#include "writer.h"
#include "package.h"
#include "core.h"

char * sIP;
int sPort;
int lPort;

char * readLine(char * buffer, size_t length){
	char * p = fgets(buffer, length, stdin);
	if(p){
		size_t last = strlen(buffer) - 1;
		if(buffer[last] == '\n'){
			buffer[last] = '\0';
		} else {
			//buffer full, but no "new line" ->
			//discard all following input:
			fscanf(stdin,"%*[^\n]");
			(void) fgetc(stdin);
		}
	}
	return p;
}

int prompt(int askListenPort, int askIP, int askPort){
	if(askListenPort){
		char * temp = malloc(sizeof(char) * 6);
		fputs("Enter port number for listening: ",stdout);
		temp = readLine(temp,6);
		if(temp){
			sscanf(temp,"%d",&lPort);
			free(temp);
		} else {
			return 1;
		}
	}
	if(askIP){
		sIP = malloc(sizeof(char) * 16); // 123.567.901.345 /0 
		fputs("Enter the IP of any other client: ",stdout);
		sIP = readLine(sIP,16);
		if(!sIP){
			// error with fgets:
			return 1;
		}
	}
	if(askPort){
		char * temp = malloc(sizeof(char) * 6);
		printf("Enter port number of %s: ",sIP);
		temp = readLine(temp,6);
		if(temp){
			sscanf(temp,"%d",&sPort);
			free(temp);
		} else {
			return 1;
		}
	}
	return 0;
}

int startListening(pthread_t * thread){
	return 0;
}

int main(int argc, char ** argv){
	pthread_t pListener;
	switch(argc){
		case 0: /* should never happen... */
		case 1: {
			/* init and ask for IP and port */
			prompt(1,1,1);
			if(initListener(lPort)){
				puts("Cant build socket...");
				return 1;
			}
			pthread_create(&pListener, NULL, listening, NULL);
			sendHello(sIP,sPort,lPort);
			pthread_join(pListener,NULL);
			break;
		}
		case 2: {
			/* init and wait for connections (no prompt) */
			if(initListener(1717)){
				puts("Cant build socket...");
				return 1;
			}
			pthread_create(&pListener, NULL, listening, NULL);
			pthread_join(pListener,NULL);
			break;
		}
		default: {
			/* exit */
			puts("wrong usage!");
			return 1;
			break;
		}
	}
	return 0;
}

