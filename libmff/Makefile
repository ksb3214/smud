CC=gcc

all: libmff.a Makefile

libmff.a: libmff.o Makefile
	ar r libmff.a libmff.o

libmff.o: libmff.c Makefile
	$(CC) -o libmff.o -ggdb3 -c libmff.c

install: libmff.a Makefile
	install libmff.a /usr/local/lib
	install mff.h /usr/local/include
