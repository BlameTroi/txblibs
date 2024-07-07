# test stand alone object versions of the libs

.POSIX:
.PHONY: clean all run

CC=clang
# CFLAGS=-Wall -g3 -std=c99
# CFLAGS=-Wall -O2 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=

all: unitpq unitpat testlibs unittest

clean:
	rm -rf *.o *.dSYM/ unitpat unittest testlibs unitpq


#####################################################
# these are the libs not as single file headers yet #
#####################################################

abort.o: ../inc/abort.h ../src/abort.c
	$(CC) $(CFLAGS) -c ../src/abort.c

misc.o: ../inc/misc.h ../src/misc.c
	$(CC) $(CFLAGS) -c ../src/misc.c

dl.o: ../inc/dl.h ../src/dl.c ../inc/abort.h
	$(CC) $(CFLAGS) -c ../src/dl.c

pat.o: ../inc/pat.h ../src/pat.c ../inc/misc.h ../inc/abort.h ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/pat.c

pmute.o: ../inc/pmute.h ../src/pmute.c
	$(CC) $(CFLAGS) -c ../src/pmute.c

pq.o: ../inc/pq.h ../src/pq.c
	$(CC) $(CFLAGS) -c ../src/pq.c

str.o: ../inc/str.h ../src/str.c
	$(CC) $(CFLAGS) -c ../src/str.c


##########################################################
# these are the test drivers for the various libs, mostl #
# but not all are done with minunit.                     #
##########################################################

unitpq.o: unitpq.c ../inc/pq.h ../inc/misc.h
	$(CC) $(CFLAGS) -c unitpq.c

unitpq: unitpq.o pq.o misc.o
	$(CC) $(CFLAGS) -o unitpq unitpq.o pq.o misc.o

unitdl.o: unitdl.c ../inc/dl.h ../inc/misc.h
	$(CC) $(CFLAGS) -c unitdl.c

unitdl: unitdl.o dl.o misc.o abort.o
	$(CC) $(CFLAGS) -o unitdl unitdl.o dl.o misc.o abort.o

testlibs.o: testlibs.c ../inc/misc.h ../inc/str.h ../inc/pmute.h
	$(CC) $(CFLAGS) -c testlibs.c

testlibs: testlibs.o misc.o str.o pmute.o
	$(CC) $(CFLAGS) -o testlibs testlibs.o misc.o pmute.o str.o

unitpat.o: unitpat.c ../inc/abort.h ../inc/misc.h ../inc/pat.h ../inc/str.h
	$(CC) $(CFLAGS) -c unitpat.c

unitpat: unitpat.o abort.o misc.o pat.o str.o
	$(CC) $(CFLAGS) -o unitpat unitpat.o abort.o misc.o pat.o $(LDFLAGS)

unittest.o: unittest.c ../inc/misc.h ../inc/str.h
	$(CC) $(CFLAGS) -c unittest.c

unittest: unittest.o misc.o str.o
	$(CC) $(CFLAGS) -o unittest unittest.o misc.o str.o $(LDFLAGS)
