
void printList(struct clientList * cListe);

void destroyList(struct clientList * cListe);

struct clientList * lastClient(struct clientList * liste);

struct clientList * createClient(int32_t ip, uint16_t port, struct clientList * next);

struct clientList * addToList(int32_t ip, uint16_t port, struct clientList * cListe);

int addBehind(int32_t ip, uint16_t port, struct clientList * element);

struct clientList * containsElement(int32_t ip, uint16_t port, struct clientList * cListe);

struct clientList * removeElement(struct clientList * cListe, struct clientList * element);

struct clientList * concatList(struct clientList * list1, struct clientList * list2);

int countList(struct clientList * cListe);
