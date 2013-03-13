objects = listener.o main.o writer.o core.o serialize.o clientList.o
options = -lpthread -g
cOpt = -Wall -g

all: netPong vacuum

netPong: $(objects)
	gcc $(cOpt) -o netPong $(objects) $(options)
	
listener.o: listener.c
	gcc $(cOpt) -c listener.c

main.o: main.c
	gcc $(cOpt) -c main.c
	
writer.o: writer.c
	gcc $(cOpt) -c writer.c
	
core.o: core.c
	gcc $(cOpt) -c core.c
	
serialize.o: serialize.c
	gcc $(cOpt) -c serialize.c

clientList.o: clientList.c
	gcc $(cOpt) -c clientList.c

vacuum:
	rm -f *.o

clean: vacuum
	rm -f netPong
