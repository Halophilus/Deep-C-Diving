.PHONY: clean

all: clean msgtest

messagetest : message.c message.h msgtest.c
	gcc message.c msgtest.c -o msgtest

clean:
	rm -f *.o
	rm -f msgtest
