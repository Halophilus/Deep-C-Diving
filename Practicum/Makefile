.PHONY: clean

all: clean msgtest

messagetest : message.c message.h msgtest.c
	gcc msgtest.c message.c -o msgtest

clean:
	rm -f *.o
	rm -f msgtest
