# makefile for txblibs

# bundle up the .h and .c to create the actual library includes.

.PHONY: all clean build release


# i type 'make clean all' entirely too automatically

all:
	echo if you want this, make build


# scrub for full build

clean:
	rm -rf release/txb*.h*


# install the combined library headers

install:
	cp release/txb*.h ~/.local/include/



# all the things

build:  txbabort txbda txbdl txbkl txbkv txbmd5 txbmisc txbpat txbpmute txbpq txbrand txbqu txbrs txbsb txbst txbstr

PRIV = ./source/src
PUB = ./source/inc

# dependencies

$(PUB)/*.h:

$(PRIV)/*.c:

txbabort: $(PUB)/abort_if.h
	buildhdr --macro TXBABORT --intro LICENSE --pub $(PUB)/abort_if.h >release/txbabort_if.h

txbda: $(PUB)/da.h $(PRIV)/da.c
	buildhdr --macro TXBDA --intro LICENSE --pub $(PUB)/da.h --priv $(PRIV)/da.c >release/txbda.h

txbdl: $(PUB)/dl.h $(PRIV)/dl.c
	buildhdr --macro TXBDL --intro LICENSE --pub $(PUB)/dl.h --priv $(PRIV)/dl.c >release/txbdl.h

txbkl: $(PUB)/kl.h $(PRIV)/kl.c
	buildhdr --macro TXBKL --intro LICENSE --pub $(PUB)/kl.h --priv $(PRIV)/kl.c >release/txbkl.h

txbkv: $(PUB)/kv.h $(PRIV)/kv.c
	buildhdr --macro TXBKV --intro LICENSE --pub $(PUB)/kv.h --priv $(PRIV)/kv.c >release/txbkv.h

txbmd5: $(PUB)/md5.h $(PRIV)/md5.c
	buildhdr --macro TXBMD5 --intro LICENSE --pub $(PUB)/md5.h --priv $(PRIV)/md5.c >release/txbmd5.h

txbmisc: $(PUB)/misc.h $(PRIV)/misc.c
	buildhdr --macro TXBMISC --intro LICENSE --pub $(PUB)/misc.h --priv $(PRIV)/misc.c >release/txbmisc.h

txbpat: $(PUB)/pat.h $(PRIV)/pat.c
	buildhdr --macro TXBPAT --intro LICENSE --pub $(PUB)/pat.h --priv $(PRIV)/pat.c >release/txbpat.h

txbpmute: $(PUB)/pmute.h $(PRIV)/pmute.c
	buildhdr --macro TXBPMUTE --intro LICENSE --pub $(PUB)/pmute.h --priv $(PRIV)/pmute.c >release/txbpmute.h

txbpq: $(PUB)/pq.h $(PRIV)/pq.c
	buildhdr --macro TXBPQ --intro LICENSE --pub $(PUB)/pq.h --priv $(PRIV)/pq.c >release/txbpq.h

txbqu: $(PUB)/qu.h $(PRIV)/qu.c
	buildhdr --macro TXBQU --intro LICENSE --pub $(PUB)/qu.h --priv $(PRIV)/qu.c >release/txbqu.h

txbrand: $(PUB)/rand.h $(PRIV)/rand.c
	buildhdr --macro TXBRAND --intro LICENSE --pub $(PUB)/rand.h --priv $(PRIV)/rand.c >release/txbrand.h

txbrs: $(PUB)/rs.h $(PRIV)/rs.c
	buildhdr --macro TXBRS --intro LICENSE --pub $(PUB)/rs.h --priv $(PRIV)/rs.c >release/txbrs.h

txbsb: $(PUB)/sb.h $(PRIV)/sb.c
	buildhdr --macro TXBSB --intro LICENSE --pub $(PUB)/sb.h --priv $(PRIV)/sb.c >release/txbsb.h

txbst: $(PUB)/st.h $(PRIV)/st.c
	buildhdr --macro TXBST --intro LICENSE --pub $(PUB)/st.h --priv $(PRIV)/st.c >release/txbst.h

txbstr: $(PUB)/str.h $(PRIV)/str.c
	buildhdr --macro TXBSTR --intro LICENSE --pub $(PUB)/str.h --priv $(PRIV)/str.c >release/txbstr.h
