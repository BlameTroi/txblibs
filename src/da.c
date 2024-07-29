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

#include "../inc/da.h"

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
