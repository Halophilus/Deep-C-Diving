.PHONY: clean

all: clean allocmem loadmem

allocmem: allocmem.c
	gcc allocmem.c -o allocmem

loadmem: loadmem.c
	gcc loadmem.c -o loadmem

clean:
	rm -f loadmem
	rm -f allocmem


