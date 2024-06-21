# just a makefile

.POSIX:

.PHONY: clean all run


CC=clang
# CFLAGS=-Wall -g3 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=


clean:
	rm -rf *.o *.dSYM/ unitpat


all: unitpat


run:
	./unitpat


unitpat.o: unitpat.c txbpat.h
	$(CC) $(CFLAGS) -c unitpat.c


unitpat: unitpat.o 
	$(CC) $(CFLAGS) -o unitpat unitpat.o $(LDFLAGS)
