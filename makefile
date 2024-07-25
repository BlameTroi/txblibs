# makefile for my various c single file header libraries

.POSIX:

.PHONY: clean all


all: abort dl misc pat pmute pq str md5 rs sb


clean:
	rm release/txb*.h


abort: inc/abort.h src/abort.c
	buildhdr --macro TXBABORT --intro LICENSE --pub inc/abort.h --priv src/abort.c >release/txbabort.h

dl: inc/dl.h src/dl.c
	buildhdr --macro TXBDL --intro LICENSE --pub inc/dl.h --priv src/dl.c >release/txbdl.h

misc: inc/misc.h src/misc.c
	buildhdr --macro TXBMISC --intro LICENSE --pub inc/misc.h --priv src/misc.c >release/txbmisc.h

pat: inc/pat.h src/pat.c
	buildhdr --macro TXBPAT --intro LICENSE --pub inc/pat.h --priv src/pat.c >release/txbpat.h

pmute: inc/pmute.h src/pmute.c
	buildhdr --macro TXBPMUTE --intro LICENSE --pub inc/pmute.h --priv src/pmute.c >release/txbpmute.h

pq: inc/pq.h src/pq.c
	buildhdr --macro TXBPQ --intro LICENSE --pub inc/pq.h --priv src/pq.c >release/txbpq.h

str: inc/str.h src/str.c
	buildhdr --macro TXBSTR --intro LICENSE --pub inc/str.h --priv src/str.c >release/txbstr.h

md5: inc/md5.h src/md5.c
	buildhdr --macro TXBMD5 --intro LICENSE --pub inc/md5.h --priv src/md5.c >release/txbmd5.h

sb: inc/sb.h src/sb.c
	buildhdr --macro TXBSB --intro LICENSE --pub inc/sb.h --priv src/sb.c >release/txbsb.h

rs: inc/rs.h src/rs.c
	buildhdr --macro TXBRS --intro LICENSE --pub inc/rs.h --priv src/rs.c >release/txbrs.h
