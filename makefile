# just a makefile

.POSIX:

.PHONY: clean all run


CC=clang
# CFLAGS=-Wall -g3 -std=c99
CFLAGS=-fsanitize=address -Wall -g3 -O0 -std=c99
# LDFLAGS=-lm
LDFLAGS=


clean:
	rm release/txb*.h


all: abort listd misc pat pmute str

abort: inc/abort.h src/abort.c
	buildhdr --macro TXBABORT --intro LICENSE --pub inc/abort.h --priv src/abort.c >release/txbabort.h

listd: inc/listd.h src/listd.c
	buildhdr --macro TXBLISTD --intro LICENSE --pub inc/listd.h --priv src/listd.c >release/txblistd.h

misc: inc/misc.h src/misc.c
	buildhdr --macro TXBMISC --intro LICENSE --pub inc/misc.h --priv src/misc.c >release/txbmisc.h

pat: inc/pat.h src/pat.c
	buildhdr --macro TXBPAT --intro LICENSE --pub inc/pat.h --priv src/pat.c >release/txbpat.h

pmute: inc/pmute.h src/pmute.c
	buildhdr --macro TXBPMUTE --intro LICENSE --pub inc/pmute.h --priv src/pmute.c >release/txbpmute.h

str: inc/str.h src/str.c
	buildhdr --macro TXBSTR --intro LICENSE --pub inc/str.h --priv src/str.c >release/txbstr.h
