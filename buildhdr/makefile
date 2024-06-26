# just a makefile

.POSIX:

.PHONY: clean all run


CC=clang
# CFLAGS=-Wall -g3 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=


clean:
	rm -rf *.o *.dSYM/ buildhdr 


all: buildhdr


buildhdr.o: buildhdr.c
	$(CC) $(CFLAGS) -c buildhdr.c


buildhdr: buildhdr.o
	$(CC) $(CFLAGS) -o buildhdr buildhdr.o $(LDFLAGS)
