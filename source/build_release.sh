#!/usr/bin/env sh

# bundle up the .h and .c to create the actual library includes.

rm -rf ../release/txb*.h*

buildhdr --macro TXBABORT --intro LICENSE --pub ./inc/abort_if.h >../release/txbabort_if.h
buildhdr --macro TXBDA --intro LICENSE --pub ./inc/da.h --pub ./src/da.c >../release/txbda.h
buildhdr --macro TXBDL --intro LICENSE --pub ./inc/dl.h --pub ./src/dl.c >../release/txbdl.h
buildhdr --macro TXBKL --intro LICENSE --pub ./inc/kl.h --pub ./src/kl.c >../release/txbkl.h
buildhdr --macro TXBKV --intro LICENSE --pub ./inc/kv.h --pub ./src/kv.c >../release/txbkv.h
buildhdr --macro TXBMD5 --intro LICENSE --pub ./inc/md5.h --pub ./src/md5.c >../release/txbmd5.h
buildhdr --macro TXBMISC --intro LICENSE --pub ./inc/misc.h --pub ./src/misc.c >../release/txbmisc.h
buildhdr --macro TXBPAT --intro LICENSE --pub ./inc/pat.h --pub ./src/pat.c >../release/txbpat.h
buildhdr --macro TXBPMUTE --intro LICENSE --pub ./inc/pmute.h --pub ./src/pmute.c >../release/txbpmute.h
buildhdr --macro TXBPQ --intro LICENSE --pub ./inc/pq.h --pub ./src/pq.c >../release/txbpq.h
buildhdr --macro TXBQU --intro LICENSE --pub ./inc/qu.h --pub ./src/qu.c >../release/txbqu.h
buildhdr --macro TXBRAND --intro LICENSE --pub ./inc/rand.h --pub ./src/rand.c >../release/txbrand.h
buildhdr --macro TXBRS --intro LICENSE --pub ./inc/rs.h --pub ./src/rs.c >../release/txbrs.h
buildhdr --macro TXBSB --intro LICENSE --pub ./inc/sb.h --pub ./src/sb.c >../release/txbsb.h
buildhdr --macro TXBST --intro LICENSE --pub ./inc/st.h --pub ./src/st.c >../release/txbst.h
buildhdr --macro TXBSTR --intro LICENSE --pub ./inc/str.h --pub ./src/str.c >../release/txbstr.h
