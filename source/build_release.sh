#!/usr/bin/env sh

# bundle up the .h and .c to create the actual library includes.

rm -rf ../release/txb*.h*

buildhdr --macro TXBABORT --intro LICENSE --pub ./inc/abort_if.h                   >../release/txbabort_if.h
buildhdr --macro TXBWARN  --intro LICENSE --pub ./inc/warn.h                       >../release/txbwarn.h

buildhdr --macro TXBALLOC --intro LICENSE --pub ./inc/alloc.h --priv ./src/alloc.c >../release/txballoc.h
buildhdr --macro TXBMD5   --intro LICENSE --pub ./inc/md5.h   --priv ./src/md5.c   >../release/txbmd5.h
buildhdr --macro TXBMISC  --intro LICENSE --pub ./inc/misc.h  --priv ./src/misc.c  >../release/txbmisc.h
buildhdr --macro TXBONE   --intro LICENSE --pub ./inc/one.h   --priv ./src/one.c   >../release/txbone.h
buildhdr --macro TXBPAT   --intro LICENSE --pub ./inc/pat.h   --priv ./src/pat.c   >../release/txbpat.h
buildhdr --macro TXBPMUTE --intro LICENSE --pub ./inc/pmute.h --priv ./src/pmute.c >../release/txbpmute.h
buildhdr --macro TXBRAND  --intro LICENSE --pub ./inc/rand.h  --priv ./src/rand.c  >../release/txbrand.h
buildhdr --macro TXBRS    --intro LICENSE --pub ./inc/rs.h    --priv ./src/rs.c    >../release/txbrs.h
buildhdr --macro TXBSB    --intro LICENSE --pub ./inc/sb.h    --priv ./src/sb.c    >../release/txbsb.h
buildhdr --macro TXBSTR   --intro LICENSE --pub ./inc/str.h   --priv ./src/str.c   >../release/txbstr.h
