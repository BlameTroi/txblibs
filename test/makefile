# just a makefile

.POSIX:

.PHONY: clean all run

CC=clang
# CFLAGS=-Wall -g3 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=

clean:
	rm -rf *.o *.dSYM/ unitpat unittest testlibs

all: unitpat unittest testlibs

abort.o: ../inc/abort.h ../src/abort.c
	$(CC) $(CFLAGS) -c ../src/abort.c

misc.o: ../inc/misc.h ../src/misc.c
	$(CC) $(CFLAGS) -c ../src/misc.c

str.o: ../inc/str.h ../src/str.c
	$(CC) $(CFLAGS) -c ../src/str.c

pat.o: ../inc/pat.h ../src/pat.c
	$(CC) $(CFLAGS) -c ../src/pat.c

pmute.o: ../inc/pmute.h ../src/pmute.c
	$(CC) $(CFLAGS) -c ../src/pmute.c

listd.o: ../inc/listd.h ../src/listd.c
	$(CC) $(CFLAGS) -c ../src/listd.c

testlibs.o: testlibs.c
	$(CC) $(CFLAGS) -c testlibs.c

testlibs: testlibs.o misc.o str.o pmute.o
	$(CC) $(CFLAGS) -o testlibs testlibs.o misc.o pmute.o str.o

unitpat.o: unitpat.c
	$(CC) $(CFLAGS) -c unitpat.c

unitpat: unitpat.o abort.o misc.o pat.o
	$(CC) $(CFLAGS) -o unitpat unitpat.o abort.o misc.o pat.o $(LDFLAGS)

unittest.o: unittest.c
	$(CC) $(CFLAGS) -c unittest.c

unittest: unittest.o misc.o listd.o str.o
	$(CC) $(CFLAGS) -o unittest unittest.o misc.o listd.o str.o $(LDFLAGS)
