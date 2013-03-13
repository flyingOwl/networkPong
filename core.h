#define DEFAULT_LOOP_PAUSE 500000

char * intToIP(int32_t ip);
int sendHello(char * sIP, int sPort, int myPort);
int helloClient(int32_t ipAddress, uint16_t port);
int setMyData(int32_t ip, uint16_t port);
int loop(void * dList);
