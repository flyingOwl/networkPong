#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "structs.h"

void printList(struct clientList * cListe){
	for(; cListe; cListe = cListe->nextClient){
		printf("Element %p: IP = %x Port = %d Next = %p\n",
			cListe,cListe->ipAddress,cListe->port,cListe->nextClient);
	}
}

void destroyList(struct clientList * cListe){
	struct clientList * temp;
	for(;(temp = cListe); cListe = temp->nextClient){
		free(cListe);
	}
}

struct clientList * lastClient(struct clientList * liste){
	struct clientList * temp = liste;
	for(; temp; temp = temp->nextClient){
		if(0 == temp->nextClient){
			return temp;
		}
	}
	return 0;
}

struct clientList * createClient(int32_t ip, uint16_t port, struct clientList * next){
	struct clientList * temp = malloc(sizeof(struct clientList));
	if(temp){
		temp->ipAddress = ip;
		temp->port = port;
		temp->nextClient = next;
	}
	return temp;
}

struct clientList * addToList(int32_t ip, uint16_t port, struct clientList * cListe){
	if(cListe){
		/* go to last element... */
		struct clientList * lClient = lastClient(cListe);
		if(lClient && !(lClient->nextClient)){
			lClient->nextClient = createClient(ip,port,0);
		}
		return cListe;
	} else {
		/* create new head... */
		cListe = createClient(ip,port,0);
		return cListe;
	}
	/* should never happen */
	return NULL;
}

int addBehind(int32_t ip, uint16_t port, struct clientList * element){
	if(element){
		struct clientList * newE = createClient(ip,port,element->nextClient);
		element->nextClient = newE;
		return 0;
	} else {
		return 1;
	}
}


struct clientList * containsElement(int32_t ip, uint16_t port, struct clientList * cListe){
	if(cListe){
		struct clientList * temp = cListe;
		for(; temp; temp = temp->nextClient){
			/* check each list item */
			if(temp->ipAddress == ip && temp->port == port){
				return temp;
			} /* else search next... */
		}
	}
	return 0;
}

struct clientList * removeElement(struct clientList * cListe, struct clientList * element){
	/* get previous element */
	if(cListe && cListe == element){
		/* element is 1st element */
		cListe = element->nextClient;
		free(element);
		return cListe;
	}
	if(cListe && element){
		struct clientList * temp = cListe;
		for(; temp; temp = temp->nextClient){
			if(temp->nextClient == element){
				temp->nextClient = element->nextClient;
				free(element);
				return cListe;
			}
		}
		return cListe;
		/* element not found... */
	}
	return cListe;
	/* liste or element was NULL */
}

struct clientList * concatList(struct clientList * list1, struct clientList * list2){
	if(list1){
		struct clientList * last1 = lastClient(list1);
		last1->nextClient = list2;
		return list1;
	} else {
		return list2;
	}
}

int countList(struct clientList * cListe){
	int temp = 0;
	for(; cListe; cListe = cListe->nextClient){
		temp++;
	}
	return temp;
}
