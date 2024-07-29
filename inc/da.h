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
