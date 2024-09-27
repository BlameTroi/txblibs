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
 * ranging from standard types to structures the da holds void *
 * pointers. storage management of elements stored in the da is the
 * responsibility of the client. freeing the da by da_destroy only
 * removes the dacb and supporting structures.
 *
 * the da grows by doubling its current allocation.
 *
 * initially all elements of the da are NULL. gaps are allowed. so
 * after:
 *
 * dacb *da = da_create(10);
 * char *data = "1234";
 * da_put(da, 5, data);
 *
 * elements 0 through 5 are defined but only element 5 is non NULL.
 *
 * char *readdata = da_get(da, 1); <-- returns a NULL
 *       readdata = da_get(da, 5); <-- returns pointer to "1234"
 *       readdata = da_get(da, 8); <-- fails
 */

/*
 * an instance of a dynamic array.
 */

typedef struct dacb dacb;

/*
 * da_create
 *
 * create a new instance of a dynamic array. the lone argument is the
 * number of items in the initial allocation. if more are needed,
 * the allocation doubles.
 *
 *     in: initial size of 0 for a default value.
 *
 * return: the da instance.
 */

dacb *
da_create(
	int size_or_zero_for_default
);

/*
 * da_destroy
 *
 * overwrite and release all dynamically allocated memory for a da.
 *
 *     in: the da instance.
 *
 * return: nothing
 */

void
da_destroy(
	dacb *da
);

/*
 * da_get
 *
 * return the contents of array index n which will be NULL if nothing
 * has been put at that index.
 *
 * fails via an assert if n greater than the highest index of a da_put.
 *
 *     in: the da instance.
 *
 *     in: integer index of item.
 *
 * return: the item as a void *, NULL if never put.
 */

void *
da_get(
	dacb *da,
	int n
);

/*
 * da_put
 *
 * insert or overwrite the contents of array index n.
 *
 *     in: the da instance.
 *
 *     in: integer index of item.
 *
 *     in: address of item as a void *.
 *
 * return: nothing.
 */

void
da_put(
	dacb *da,
	int n,
	void *put);

/*
 * da_count
 *
 * how many items (null or otherwise) does the array hold. this will
 * be one more than the highest 'n' passed to da_put.
 *
 *     in: the da instance.
 *
 * return: integer number of possible items, one more than the
 *         highest 'n' passed to da_put.
 */

int
da_count(
	dacb *da
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* da.h ends here */
