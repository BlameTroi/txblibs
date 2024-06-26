# just a makefile

.POSIX:

.PHONY: clean all run


CC=clang
# CFLAGS=-Wall -g3 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=


clean:
	rm -rf *.o *.dSYM/ buildhdr unitbuild


all: unitbuild buildhdr


run:
	./unitbuild


buildhdr.o: buildhdr.c buildhdr.h
	$(CC) $(CFLAGS) -c buildhdr.c


unitbuild.o: unitbuild.c
	$(CC) $(CFLAGS) -c unitbuild.c


unitbuild: unitbuild.o
	$(CC) $(CFLAGS) -o unitbuild unitbuild.o $(LDFLAGS)


buildhdr: buildhdr.o
	$(CC) $(CFLAGS) -o buildhdr buildhdr.o $(LDFLAGS)
