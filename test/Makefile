# test stand alone object versions of the libs

.POSIX:
.PHONY: clean all run

CC=clang
# CFLAGS=-Wall -g3 -std=c18
# CFLAGS=-Wall -O2 -std=c18
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c18 --pedantic-errors
# LDFLAGS=-lm
LDFLAGS=

all: unitda unitdl unitkl unitkv unitmd5 unitmute unitpat unitpq unitqu unitrs unitsb unitst unittest

clean:
	rm -rf *.o *.dSYM/ unitda unitdl unitkl unitkv unitmd5 unitmute unitpat unitpq unitqu unitrs unitsb unitst unittest

#####################################################
# these are the libs not as single file headers yet #
#####################################################

da.o: ../inc/da.h ../src/da.c
	$(CC) $(CFLAGS) -c ../src/da.c

dl.o: ../inc/dl.h ../src/dl.c
	$(CC) $(CFLAGS) -c ../src/dl.c

kl.o: ../inc/kl.h ../src/kl.c
	$(CC) $(CFLAGS) -c ../src/kl.c

kv.o: ../inc/kv.h ../src/kv.c
	$(CC) $(CFLAGS) -c ../src/kv.c

md5.o: ../inc/md5.h ../src/md5.c
	$(CC) $(CFLAGS) -c ../src/md5.c

misc.o: ../inc/misc.h ../src/misc.c
	$(CC) $(CFLAGS) -c ../src/misc.c

pat.o: ../inc/pat.h ../src/pat.c
	$(CC) $(CFLAGS) -c ../src/pat.c

pmute.o: ../inc/pmute.h ../src/pmute.c
	$(CC) $(CFLAGS) -c ../src/pmute.c

pq.o: ../inc/pq.h ../src/pq.c
	$(CC) $(CFLAGS) -c ../src/pq.c

qu.o: ../inc/qu.h ../src/qu.c ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/qu.c

rand.o: ../inc/rand.h ../src/rand.c
	$(CC) $(CFLAGS) -c ../src/rand.c

rs.o: ../inc/rs.h ../src/rs.c ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/rs.c

sb.o: ../inc/sb.h ../src/sb.c ../inc/str.h
	$(CC) $(CFLAGS) -c ../src/sb.c

st.o: ../inc/st.h ../src/st.c ../inc/dl.h
	$(CC) $(CFLAGS) -c ../src/st.c

str.o: ../inc/str.h ../src/str.c
	$(CC) $(CFLAGS) -c ../src/str.c


#########################################################
# these are the test drivers for the various libs, most #
# but not all are done with minunit.                    #
#########################################################

unitda.o: unitda.c
	$(CC) $(CFLAGS) -c unitda.c

unitda: unitda.o misc.o da.o rand.o
	$(CC) $(CFLAGS) -o unitda unitda.o misc.o da.o rand.o $(LDFLAGS)

unitdl.o: unitdl.c
	$(CC) $(CFLAGS) -c unitdl.c

unitdl: unitdl.o dl.o misc.o str.o rand.o
	$(CC) $(CFLAGS) -o unitdl unitdl.o dl.o misc.o str.o rand.o

unitkl.o: unitkl.c
	$(CC) $(CFLAGS) -c unitkl.c

unitkl: unitkl.o kl.o misc.o str.o rand.o
	$(CC) $(CFLAGS) -o unitkl unitkl.o kl.o misc.o str.o rand.o

unitkv.o: unitkv.c
	$(CC) $(CFLAGS) -c unitkv.c

unitkv: unitkv.o kv.o rand.o
	$(CC) $(CFLAGS) -o unitkv unitkv.o kv.o rand.o $(LDFLAGS)

unitmd5.o: unitmd5.c
	$(CC) $(CFLAGS) -c unitmd5.c

unitmd5: unitmd5.o md5.o
	$(CC) $(CFLAGS) -o unitmd5 unitmd5.o md5.o

unitmute.o: unitmute.c
	$(CC) $(CFLAGS) -c unitmute.c

unitmute: unitmute.o pmute.o
	$(CC) $(CFLAGS) -o unitmute unitmute.o pmute.o $(LDFLAGS)

unitpat.o: unitpat.c
	$(CC) $(CFLAGS) -c unitpat.c

unitpat: unitpat.o misc.o pat.o str.o
	$(CC) $(CFLAGS) -o unitpat unitpat.o misc.o pat.o str.o $(LDFLAGS)

unitpq.o: unitpq.c
	$(CC) $(CFLAGS) -c unitpq.c

unitpq: unitpq.o pq.o misc.o rand.o
	$(CC) $(CFLAGS) -o unitpq unitpq.o pq.o misc.o rand.o

unitqu.o: unitqu.c
	$(CC) $(CFLAGS) -c unitqu.c

unitqu: unitqu.o qu.o str.o
	$(CC) $(CFLAGS) -o unitqu unitqu.o qu.o str.o

unitrand.o: unitrand.c
	$(CC) $(CFLAGS) -c unitrand.c

unitrand: unitrand.o rand.o
	$(CC) $(CFLAGS) -o unitrand unitrand.o rand.o $(LDFLAGS)

unitrs.o: unitrs.c
	$(CC) $(CFLAGS) -c unitrs.c

unitrs: unitrs.o str.o rs.o
	$(CC) $(CFLAGS) -o unitrs unitrs.o rs.o str.o $(LDFLAGS)

unitsb.o: unitsb.c
	$(CC) $(CFLAGS) -c unitsb.c

unitsb: unitsb.o str.o sb.o
	$(CC) $(CFLAGS) -o unitsb unitsb.o sb.o str.o $(LDFLAGS)

unitst.o: unitst.c
	$(CC) $(CFLAGS) -c unitst.c

unitst: unitst.o st.o dl.o rand.o
	$(CC) $(CFLAGS) -o unitst unitst.o st.o dl.o rand.o $(LDFLAGS)

unittest.o: unittest.c
	$(CC) $(CFLAGS) -c unittest.c

unittest: unittest.o misc.o str.o
	$(CC) $(CFLAGS) -o unittest unittest.o misc.o str.o $(LDFLAGS)
