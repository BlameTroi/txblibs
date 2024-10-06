/* da.c -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic dynammic array. the
 * array stores 'payloads', void * sized items that are typically
 * pointers to dynamically allocated memory.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/da.h"

/*
 * the trasnparent definition of the dacb. the default size for number
 * of elements is arbitrary and could be changed. the array storage
 * grows by doubling the current size.
 */

#define DACB_TAG "__DACB__"
#define DACB_TAG_LEN 8
#define ASSERT_DACB(p, m) assert((p) && memcmp((p), DACB_TAG, DACB_TAG_LEN) == 0 && (m))
#define ASSERT_DACB_OR_NULL(p) assert((p) == NULL || memcmp((p), DACB_TAG, DACB_TAG_LEN) == 0)

#define DACB_DEFAULT_SIZE 512

struct dacb {
	char tag[DACB_TAG_LEN];     /* eye catcher & verification */
	int length;                 /* last used (via put) item */
	int size;                   /* size of data in number of items */
	void **data;                /* pointer to the item pointers */
};

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
) {
	dacb *da = malloc(sizeof(dacb));
	memset(da, 0, sizeof(dacb));
	memcpy(da->tag, DACB_TAG, sizeof(da->tag));
	da->size = size_or_zero_for_default ? size_or_zero_for_default :
		DACB_DEFAULT_SIZE;
	da->data = malloc(da->size *sizeof(void *));
	da->length = -1;
	memset(da->data, 0, da->size *sizeof(void *));
	return da;
}

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
) {
	ASSERT_DACB(da, "invalid DACB");
	memset(da->data, 0, da->size *sizeof(void *));
	free(da->data);
	memset(da, 253, sizeof(dacb));
	free(da);
}

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
) {
	ASSERT_DACB(da, "invalid DACB");
	assert(n < da->size);
	void *res = da->data[n];
	return res;
}

/*
 * da_put
 *
 * insert or overwrite the contents of array index n. if the location
 * is outside the current buffer, repeatedly double the buffer size
 * until it can hold the location.
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
	void *put
) {
	ASSERT_DACB(da, "invalid DACB");
	assert(put);
	while (n >= da->size) {
		void **old = da->data;
		da->data = malloc(2 * da->size * sizeof(void *));
		memset(da->data, 0, 2 * da->size * sizeof(void *));
		memcpy(da->data, old, da->size * sizeof(void *));
		memset(old, 253, da->size *sizeof(void *));
		free(old);
		da->size = 2 * da->size;
	}
	da->data[n] = put;
	if (n > da->length)
		da->length = n;
}

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
) {
	ASSERT_DACB(da, "invalid DACB");
	return da->length + 1;
}

/* da.c ends here */
