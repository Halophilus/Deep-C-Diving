CC = gcc
CFLAGS = -Wall -g
OBJ = message.o msgtest.o

all: msgtest

msgtest: $(OBJ)
	$(CC) $(CFLAGS) -o msgtest $(OBJ)

msgtest.o: msgtest.c message.h
	$(CC) $(CFLAGS) -c msgtest.c

message.o: message.c message.h
	$(CC) $(CFLAGS) -c message.c

clean:
	rm -f *.o msgtest

