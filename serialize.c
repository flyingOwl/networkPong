#include <arpa/inet.h>
#include <string.h>

#include "structs.h"
#include "serialize.h"

unsigned char * serialize_prePack(unsigned char * buffer, struct PACK_PRE * myStruct){
	if(buffer){
		int32_t b1 = htonl(myStruct->type);
		int32_t b2 = htonl(myStruct->dataType);
		int32_t b3 = htonl(myStruct->preLength);
		int size = sizeof(int32_t);
		memcpy(buffer,&b1,size);
		buffer += size;
		memcpy(buffer,&b2,size);
		buffer += size;
		memcpy(buffer,&b3,size);
		buffer +=size;
	}
	return buffer;
}

struct PACK_PRE * restore_prePack(unsigned char * data, struct PACK_PRE * buffer){
	if(buffer){
		int size = sizeof(int32_t);
		
		buffer->type = ntohl(*((int32_t *) data));
		data += size;
		buffer->dataType = ntohl(*((int32_t *) data));
		data += size;
		buffer->preLength = ntohl(*((int32_t *) data));
	}
	return buffer;
}

unsigned char * serialize_IPport(unsigned char * buffer, struct clientList * myStruct){
	if(buffer){
		int32_t  b1 = myStruct->ipAddress;
		uint16_t b2 = htons(myStruct->port);
		int size32 = sizeof(int32_t);
		int size16 = sizeof(uint16_t);
		memcpy(buffer,&b1,size32);
		buffer += size32;
		memcpy(buffer,&b2,size16);
		buffer += size16;
	}
	return buffer;
}

struct clientList * restore_IPport(unsigned char * data, struct clientList * buffer){
	if(buffer){
		buffer->ipAddress = *((int32_t *) data);
		data += sizeof(int32_t);
		buffer->port = ntohs(*((uint16_t *) data));
	}
	return buffer;
}

int serialize_size_IPport(){
	return (sizeof(int32_t) + sizeof(uint16_t));
}

