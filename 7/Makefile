.PHONY: clean

all: clean multithread pqueuepmain

multithread: multithread.c
	gcc -o multithread multithread.c -lpthread

pqueuepmain: pqueuepmain.c
	gcc -o pqueuepmain pqueuepmain.c -lpthread

clean:
	rm -f multithread
	rm -f pqueuepmain
