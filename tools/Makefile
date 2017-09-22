CC=gcc
LDFLAGS=-L../libmff
CFLAGS=-I../libmff
LIBS=-lmff

all: convert Makefile

convert: convert.o Makefile
	$(CC) $(LDFLAGS) -o convert convert.o $(LIBS)

convert.o: convert.c Makefile
	$(CC) $(CFLAGS) -o convert.o -c convert.c
