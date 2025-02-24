.PHONY: clean

all: clean genrand cipher

genrand: genrand.c
	gcc genrand.c -o genrand -lm

cipher: cipher.c polybius.c polybius.h
	gcc cipher.c polybius.c -o cipher -lm

clean:
	rm -f *.o
	rm -f string_file.txt
