/*
 * single file header generated via:
 * buildhdr --macro TXBDA --intro LICENSE --pub inc/da.h --priv src/da.c 
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

#ifndef TXBDA_SINGLE_HEADER
#define TXBDA_SINGLE_HEADER
/* *** begin pub *** */
/* da.h -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic and somewhat leaky
 * dynamic array.
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

#include <stdlib.h>

/*
 * the da is a dynmically sized array. to deal with various datatypes
 * ranging from standard types to structures the day holds pointers to
 * specific instances. in any case where the data would fit in a
 * void * pointer, the data can be passed as the pointer.
 *
 * management of the actual data and its storage is left to the
 * client code. a da will free its own managed storage, but it has
 * no safe way to free client data at this time. assuming the data
 * is all in separately allocated blocks, the following example
 * would work:
 *
 * void *item = NULL;
 * for (int i = 0; i < da->length; i++) {
 *    item = da_get(da, i);
 *    free(item);
 *    da_put(da, i, NULL);
 * }
 * da_destroy(da);
 */

/*
 * an instance of a dynamic array.
 */

typedef struct dacb dacb;

/*
 * create a new dynamic array with an initial size of some number of
 * entries. if 0, a default value is used.
 */

dacb *
da_create(
   int size_or_zero_for_default
);

/*
 * free resources for of the dynamic array that are under control of
 * this library: the dacb and the buffer holding entry pointers.
 */

void
da_destroy(
   dacb *da
);

/*
 * return the pointer in the array at position n.
 */

void *
da_get(
   dacb *da,
   int n
);

/*
 * put a reference to the data you want to store in the array at
 * position n. if n is greater than the current maximum number of
 * entries, the buffer is doubled in size until n fits.
 */

void
da_put(
   dacb *da,
   int n,
   void *put);

/*
 * how many entries does the array hold if entries 0 .. n were added.
 * the answer should be n+1.
 */

int
da_length(
   dacb *da
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* *** end pub *** */

#endif /* TXBDA_SINGLE_HEADER */

#ifdef TXBDA_IMPLEMENTATION
#undef TXBDA_IMPLEMENTATION
/* *** begin priv *** */
/* da.c -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic and somewhat leaky
 * dynamic array.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*
 * the trasnparent definition of the dacb. the default size
 * for number of elements is arbitrary and could be changed.
 * the array storage grows by doubling, not in default
 * sized increments.
 */

#define DACB_TAG "__DACB__"
#define DACB_DEFAULT_SIZE 512
struct dacb {
   char tag[8];                /* eye catcher & verification */
   int length;                 /* last used (via put) entry */
   int size;                   /* size of data in number of entries */
   void **data;                /* pointer to the entry pointers */
};

/*
 * create a new instance of a dynamic array. the lone argument is the
 * number of entries in the initial allocation. if more are needed,
 * the allocation doubles.
 */

dacb *
da_create(
   int size_or_zero_for_default
) {
   dacb *da = malloc(sizeof(dacb));
   memset(da, 0, sizeof(dacb));
   memcpy(da->tag, DACB_TAG, sizeof(da->tag));
   da->size = size_or_zero_for_default ? size_or_zero_for_default : DACB_DEFAULT_SIZE;
   da->data = malloc(da->size * sizeof(void *));
   da->length = -1;
   memset(da->data, 0, da->size * sizeof(void *));
   return da;
}

/*
 * destroy an instance of a dynamic array by releasing resources that
 * were directly allocated by da_create and da_put: the dacb itself
 * and the current entries buffer.
 */

void
da_destroy(
   dacb *da
) {
   assert(da && memcmp(da->tag, DACB_TAG, sizeof(da->tag)) == 0);
   memset(da->data, 0, da->size * sizeof(void *));
   free(da->data);
   memset(da, 0, sizeof(dacb));
   free(da);
}

/*
 * returns the entry at n, but will fail if n is greater than the
 * maximum entry stored by da_put.
 */

void *
da_get(
   dacb *da,
   int n
) {
   assert(da && memcmp(da->tag, DACB_TAG, sizeof(da->tag)) == 0);
   assert(n < da->size);
   void *res = da->data[n];
   return res;
}

/*
 * store an entry in the array. if the location is outside the current
 * buffer, repeatedly double the buffer size until it can hold the
 * location.
 */

void
da_put(
   dacb *da,
   int n,
   void *put
) {
   assert(da && memcmp(da->tag, DACB_TAG, sizeof(da->tag)) == 0);
   assert(put);
   while (n >= da->size) {
      void **old = da->data;
      da->data = malloc(2 * da->size * sizeof(void *));
      memcpy(da->data, old, da->size * sizeof(void *));
      memset(old, 0, da->size * sizeof(void *));
      free(old);
      da->size = 2 * da->size;
   }
   da->data[n] = put;
   if (n > da->length) {
      da->length = n;
   }
}

/*
 * report the number of elements in the array if all 0 .. n were
 * added. i hate zero based indices, but they are the norm so +1
 * here.
 */

int
da_length(
   dacb *da
) {
   assert(da && memcmp(da->tag, DACB_TAG, sizeof(da->tag)) == 0);
   return da->length + 1;
}
/* *** end priv *** */

#endif /* TXBDA_IMPLEMENTATION */
