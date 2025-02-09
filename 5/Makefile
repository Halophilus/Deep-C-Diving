.PHONY: clean

all: clean queuetest

queuetest: queuetest.c queue.c queue.h
	gcc queuetest.c queue.c -o queuetest

clean:
	rm -f queuetest

