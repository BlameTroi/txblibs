/*
 * single file header generated via:
 * buildhdr --macro TXBABORT --intro LICENSE --pub inc/abort.h --priv src/abort.c 
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Troy Brumley 
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   *** end intro ***
 */

#ifndef TXBABORT_SINGLE_HEADER
#define TXBABORT_SINGLE_HEADER
/* *** begin pub *** */
/* abort.h -- blametroi's common utility functions -- */

/*
 * a header only implementation of abort and report helper functions
 * and macros.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
do_abort(const char *msg, const char *func, const char *file, int line);

void
do_abort_if(const char *cond, const char *msg, const char *func, const char *file, int line);

#define abort(msg) do {\
   do_abort(msg, __func__, __FILE__, __LINE__);\
   } while(0)

#define abort_if(cond, msg) do {\
   if (cond) {\
   do_abort_if(#cond, msg, __func__, __FILE__, __LINE__);\
   }\
   } while(0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* *** end pub *** */

#endif /* TXBABORT_SINGLE_HEADER */

#ifdef TXBABORT_IMPLEMENTATION
#undef TXBABORT_IMPLEMENTATION
/* *** begin priv *** */
/* abort.c -- blametroi's common utility functions -- */

/*
 * a header only implementation of abort and report functions.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdio.h>
#include <stdlib.h>


void
do_abort_if(
   const char *cond,
   const char *msg,
   const char *func,
   const char *file,
   int line
) {
   fprintf(stderr, "abort:\n\tcondition: %s\n\t%s\n\t%s %s %d\n", cond, msg, func, file, line);
   exit(EXIT_FAILURE);
}

void
do_abort(
   const char *msg,
   const char *func,
   const char *file,
   int line
) {
   fprintf(stderr, "abort:\n\t%s\n\t%s %s %d\n", msg, func, file, line);
   exit(EXIT_FAILURE);
}
/* *** end priv *** */

#endif /* TXBABORT_IMPLEMENTATION */
