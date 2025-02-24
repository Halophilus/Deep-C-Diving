.PHONY: clean

all: clean driver cipher

driver: queuedriver.c queue.c polybius.c genrand.c
	gcc queuedriver.c queue.c polybius.c genrand.c -o driver -lm

cipher: cipher.c polybius.c polybius.h
	gcc cipher.c polybius.c -o cipher -lm

clean:
	rm -f *.o
	rm -f *.txt
	rm -f cipher
	rm -f driver
