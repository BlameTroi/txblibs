# test stand alone object versions of the libs

.POSIX:
.PHONY: clean all run

CC=clang
# CFLAGS=-Wall -g3 -std=c99
# CFLAGS=-Wall -O2 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=

all: testlibs unitda unitdl unitfs unitmd5 unitpat unitpq unitqu unitrs unitsb unittest

clean:
	rm -rf *.o *.dSYM/  testlibs unitda unitdl unitfs unitmd5 unitpat unitpq unitqu unitrs unitsb unittest

#####################################################
# these are the libs not as single file headers yet #
#####################################################

abort.o: ../inc/abort.h ../src/abort.c
	$(CC) $(CFLAGS) -c ../src/abort.c

dl.o: ../inc/dl.h ../src/dl.c ../inc/abort.h
	$(CC) $(CFLAGS) -c ../src/dl.c

md5.o: ../inc/md5.h ../src/md5.c
	$(CC) $(CFLAGS) -c ../src/md5.c

misc.o: ../inc/misc.h ../src/misc.c
	$(CC) $(CFLAGS) -c ../src/misc.c

pat.o: ../inc/pat.h ../src/pat.c ../inc/misc.h ../inc/abort.h ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/pat.c

pmute.o: ../inc/pmute.h ../src/pmute.c
	$(CC) $(CFLAGS) -c ../src/pmute.c

pq.o: ../inc/pq.h ../src/pq.c
	$(CC) $(CFLAGS) -c ../src/pq.c

rs.o: ../inc/rs.h ../src/rs.c ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/rs.c

sb.o: ../inc/sb.h ../src/sb.c ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/sb.c

str.o: ../inc/str.h ../src/str.c
	$(CC) $(CFLAGS) -c ../src/str.c

da.o: ../inc/da.h ../inc/misc.h ../src/da.c
	$(CC) $(CFLAGS) -c ../src/da.c

qu.o: ../inc/qu.h ../src/qu.c ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/qu.c

fs.o: ../inc/fs.h ../src/fs.c
	$(CC) $(CFLAGS) -c ../src/fs.c

#########################################################
# these are the test drivers for the various libs, most #
# but not all are done with minunit.                    #
#########################################################

unitqu.o: unitqu.c ../inc/qu.h ../src/qu.c ../inc/str.h
	$(CC) $(CFLAGS) -c unitqu.c

unitqu: unitqu.o qu.o str.o
	$(CC) $(CFLAGS) -o unitqu unitqu.o qu.o str.o

unitmd5.o: unitmd5.c ../inc/md5.h ../src/md5.c
	$(CC) $(CFLAGS) -c unitmd5.c

unitmd5: unitmd5.o md5.o
	$(CC) $(CFLAGS) -o unitmd5 unitmd5.o md5.o

unitpq.o: unitpq.c ../inc/pq.h ../inc/misc.h
	$(CC) $(CFLAGS) -c unitpq.c

unitpq: unitpq.o pq.o misc.o
	$(CC) $(CFLAGS) -o unitpq unitpq.o pq.o misc.o

unitdl.o: unitdl.c ../inc/dl.h ../inc/misc.h ../inc/abort.h
	$(CC) $(CFLAGS) -c unitdl.c

unitdl: unitdl.o dl.o misc.o abort.o str.o
	$(CC) $(CFLAGS) -o unitdl unitdl.o dl.o misc.o abort.o str.o

testlibs.o: testlibs.c ../inc/misc.h ../inc/str.h ../inc/pmute.h
	$(CC) $(CFLAGS) -c testlibs.c

testlibs: testlibs.o misc.o str.o pmute.o
	$(CC) $(CFLAGS) -o testlibs testlibs.o misc.o pmute.o str.o

unitpat.o: unitpat.c ../inc/abort.h ../inc/misc.h ../inc/pat.h ../inc/str.h
	$(CC) $(CFLAGS) -c unitpat.c

unitpat: unitpat.o abort.o misc.o pat.o str.o
	$(CC) $(CFLAGS) -o unitpat unitpat.o abort.o misc.o pat.o str.o $(LDFLAGS)

unittest.o: unittest.c ../inc/misc.h ../inc/str.h
	$(CC) $(CFLAGS) -c unittest.c

unittest: unittest.o misc.o str.o
	$(CC) $(CFLAGS) -o unittest unittest.o misc.o str.o $(LDFLAGS)

unitrs.o: unitrs.c ../inc/rs.h ../inc/str.h
	$(CC) $(CFLAGS) -c unitrs.c

unitrs: unitrs.o str.o rs.o
	$(CC) $(CFLAGS) -o unitrs unitrs.o rs.o str.o $(LDFLAGS)

unitsb.o: unitsb.c ../inc/sb.h ../inc/str.h
	$(CC) $(CFLAGS) -c unitsb.c

unitsb: unitsb.o str.o sb.o
	$(CC) $(CFLAGS) -o unitsb unitsb.o sb.o str.o $(LDFLAGS)

unitda.o: unitda.c ../inc/da.h ../inc/misc.h
	$(CC) $(CFLAGS) -c unitda.c

unitda: unitda.o misc.o da.o
	$(CC) $(CFLAGS) -o unitda unitda.o misc.o da.o $(LDFLAGS)

unitfs.o: unitfs.c ../inc/fs.h
	$(CC) $(CFLAGS) -c unitfs.c

unitfs: unitfs.o fs.o
	$(CC) $(CFLAGS) -o unitfs unitfs.o fs.o $(LDFLAGS)
