struct clientList {
	int32_t ipAddress;
	uint16_t port;
	struct clientList * nextClient;
};


struct PACK_PRE {
	int type;
	int dataType;
	int preLength;
};
