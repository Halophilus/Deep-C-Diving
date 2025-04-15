CC = gcc
CFLAGS = -Wall -g

all: server client

server: server.o messenger.o
	$(CC) $(CFLAGS) -o server server.o messenger.o

client: client.o messenger.o
	$(CC) $(CFLAGS) -o client client.o messenger.o

server.o: server.c messenger.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c messenger.h
	$(CC) $(CFLAGS) -c client.c

messenger.o: messenger.c messenger.h
	$(CC) $(CFLAGS) -c messenger.c

clean:
	rm -f *.o server client
