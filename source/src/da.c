/* da.c -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic dynamic array.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdlib.h>
#include <string.h>
#include "../inc/abort_if.h"
#include "../inc/da.h"

/*
 * the da is a dynamically sized array. to deal with various data
 * types ranging from standard types to structures the da holds
 * 'payloads'. a payload is void * sized and typically holds a
 * pointer to some client data.
 *
 * storage management of items stored in the da is the responsibility
 * of the client.
 *
 * the da grows by doubling.
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
 * the transparent definition of the dacb. the default size for number
 * of elements is arbitrary and could be changed. the array storage
 * grows by doubling.
 */

#define DACB_TAG "__DACB__"
#define DACB_TAG_LEN 8

#define ASSERT_DACB(p, m) \
	abort_if(!(p) || memcmp((p), DACB_TAG, DACB_TAG_LEN) != 0, (m));

#define ASSERT_DACB_OR_NULL(p, m) \
	abort_if(p && memcmp((p), DACB_TAG, DACB_TAG_LEN) != 0, (m));

#define DACB_DEFAULT_SIZE 512

struct dacb {
	char tag[DACB_TAG_LEN];     /* eye catcher & verification */
	int length;                 /* last used (via put) item */
	int size;                   /* size of data in number of items */
	ppayload *data;            /* pointer to the item pointers */
};

/*
 * da_create
 *
 * create a new dynamic array instance. the lone argument is the
 * number of items in the initial allocation. if more are needed,
 * the allocation doubles.
 *
 *     in: initial size or 0 for a default value
 *
 * return: the da instance
 */

dacb *
da_create(
	int size_or_zero_for_default
) {
	dacb *da = malloc(sizeof(dacb));
	memset(da, 0, sizeof(dacb));
	memcpy(da->tag, DACB_TAG, sizeof(da->tag));
	da->size = size_or_zero_for_default ? size_or_zero_for_default :
		DACB_DEFAULT_SIZE;
	da->data = malloc(da->size * sizeof(void *));
	da->length = -1;
	memset(da->data, 0, da->size * sizeof(void *));
	return da;
}

/*
 * da_destroy
 *
 * overwrite and release da memory.
 *
 *     in: the da instance
 *
 * return: nothing
 */

void
da_destroy(
	dacb *da
) {
	ASSERT_DACB(da, "invalid DACB");
	memset(da->data, 253, da->size *sizeof(void *));
	free(da->data);
	memset(da, 253, sizeof(dacb));
	free(da);
}

/*
 * da_get
 *
 * return the item at array index n which will be NULL if nothing
 * has been put there yet.
 *
 * fails via an abort if n is greater than the highest index established
 * by a da_put.
 *
 *     in: the da instance
 *     in: integer index of item
 *
 * return: the item as a void * or NULL.
 */

ppayload
da_get(
	dacb *da,
	int n
) {
	ASSERT_DACB(da, "da_get invalid DACB");
	abort_if(n >= da->size, "da_get out of bounds request");
	return da->data[n];
}

/*
 * da_put
 *
 * insert or overwrite the item at index n. if the location is outside
 * the current buffer, repeatedly double the buffer size until it can
 * hold the location.
 *
 *     in: the da instance
 *     in: integer index of item
 *     in: address of item as a void *
 *
 * return: nothing
 */

void
da_put(
	dacb *da,
	int n,
	ppayload payload
) {
	ASSERT_DACB(da, "da_put invalid DACB");
	while (n >= da->size) {
		ppayload *old = da->data;
		da->data = malloc(2 * da->size * sizeof(void *));
		memset(da->data, 0, 2 * da->size * sizeof(void *));
		memcpy(da->data, old, da->size * sizeof(void *));
		memset(old, 253, da->size * sizeof(void *));
		free(old);
		da->size = 2 * da->size;
	}
	da->data[n] = payload;
	if (n > da->length)
		da->length = n;
}

/*
 * da_count
 *
 * how many items (null or otherwise) does the array hold. this will
 * be one more than the highest 'n' passed to da_put.
 *
 *     in: the da instance
 *
 * return: integer number of possible items, one more than the
 *         highest 'n' passed to da_put.
 */

int
da_count(
	dacb *da
) {
	ASSERT_DACB(da, "invalid DACB");
	return da->length + 1;
}

/* da.c ends here */
