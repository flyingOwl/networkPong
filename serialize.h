
unsigned char * serialize_prePack(unsigned char * buffer, struct PACK_PRE * myStruct);

struct PACK_PRE * restore_prePack(unsigned char * data, struct PACK_PRE * buffer);

unsigned char * serialize_IPport(unsigned char * buffer, struct clientList * myStruct);

struct clientList * restore_IPport(unsigned char * data, struct clientList * buffer);

int serialize_size_IPport();
