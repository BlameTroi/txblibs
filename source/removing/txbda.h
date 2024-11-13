//* txbda.h -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic dynamic array.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TXBDA_H
#define TXBDA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
 * hda *da = da_create(10);
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
 * handle to a dynamic array instance.
 */

typedef struct dacb hda;

/*
 * ppayload, pkey, pvalue
 *
 * these libraries manage client 'payloads'. these are void * sized
 * and are generally assumed to be a pointer to client managed data,
 * but anything that will fit in a void * pointer (typically eight
 * bytes) is allowed.
 *
 * it is the client's responsibility to free any of its dynamically
 * allocated memory. library code provides 'destroy' methods to clear
 * and release library data structures.
 *
 * these type helpers are all synonyms for void *.
 */

typedef void * pkey;
typedef void * pvalue;
typedef void * ppayload;

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

hda *
da_create(
	int size_or_zero_for_default
);

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
	hda *da
);

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
 * return: a payload or NULL
 */

ppayload
da_get(
	hda *da,
	int n
);

/*
 * da_put
 *
 * insert or overwrite the item at index n. if the location is outside
 * the current buffer, repeatedly double the buffer size until it can
 * hold the location.
 *
 *     in: the da instance
 *     in: integer index of item
 *     in: a payload
 *
 * return: nothing
 */

void
da_put(
	hda *da,
	int n,
	ppayload payload
);

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
	hda *da
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBDA_H */

#ifdef TXBDA_IMPLEMENTATION
#undef TXBDA_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include "txbabort.h"

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
 * hda *da = da_create(10);
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

#define HDA_TAG "__HDA___"
#define HDA_TAG_LEN 8

#define ASSERT_HDA(p, m) \
	abort_if(!(p) || memcmp((p), HDA_TAG, HDA_TAG_LEN) != 0, (m));

#define ASSERT_HDA_OR_NULL(p, m) \
	abort_if(p && memcmp((p), HDA_TAG, HDA_TAG_LEN) != 0, (m));

#define HDA_DEFAULT_SIZE 512

struct dacb {
	char tag[HDA_TAG_LEN];      /* eye catcher & verification */
	int length;                 /* last used (via put) item */
	int size;                   /* size of data in number of items */
	ppayload *data;             /* pointer to the item pointers */
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

hda *
da_create(
	int size_or_zero_for_default
) {
	hda *da = malloc(sizeof(*da));
	memset(da, 0, sizeof(*da));
	memcpy(da->tag, HDA_TAG, sizeof(da->tag));
	da->size = size_or_zero_for_default ? size_or_zero_for_default :
		HDA_DEFAULT_SIZE;
	da->data = malloc(da->size * sizeof(ppayload));
	da->length = -1;
	memset(da->data, 0, da->size * sizeof(ppayload));
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
	hda *da
) {
	ASSERT_HDA(da, "invalid HDA");
	memset(da->data, 253, da->size * sizeof(ppayload));
	free(da->data);
	memset(da, 253, sizeof(*da));
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
	hda *da,
	int n
) {
	ASSERT_HDA(da, "da_get invalid HDA");
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
	hda *da,
	int n,
	ppayload payload
) {
	ASSERT_HDA(da, "da_put invalid HDA");
	while (n >= da->size) {
		ppayload *old = da->data;
		da->data = malloc(2 * da->size * sizeof(ppayload));
		memset(da->data, 0, 2 * da->size * sizeof(ppayload));
		memcpy(da->data, old, da->size * sizeof(ppayload));
		memset(old, 253, da->size * sizeof(ppayload));
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
	hda *da
) {
	ASSERT_HDA(da, "invalid HDA");
	return da->length + 1;
}
#endif /* TXBDA_IMPLEMENTATION */
/* txbda.h ends here */
