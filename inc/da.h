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
 * pointers. storage management of elements stored in the da is
 * the responsibility of the client. freeing the da by da_destroy
 * only removes the dacb and supporting structures.
 *
 * initially all elements of the da are NULL. gaps are allowed. so,
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
 * create a new instance of a dynamic array with an initial size of
 * some number of entries. if 0, a default value from da.c is used.
 */

dacb *
da_create(
	int size_or_zero_for_default
);

/*
 *
 * da_destroy
 *
 * overwrite and release all dynamically allocated memory for a
 * da.
 */

void
da_destroy(
	dacb *da
);

/*
 * da_get
 *
 * return the contents of array position n.
 */

void *
da_get(
	dacb *da,
	int n
);

/*
 * da_put
 *
 * insert or overwrite the contents of array position n.
 */

void
da_put(
	dacb *da,
	int n,
	void *put);

/*
 * da_count
 *
 * how many entries (null or otherwise) does the array hold. this
 * will be one more than the highest 'n' pased on a da_get.
 */

int
da_count(
	dacb *da
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* da.h ends here */
