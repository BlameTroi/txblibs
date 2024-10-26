/* da.h -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic dynamic array.
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
 * an instance of a dynamic array.
 */

typedef struct dacb dacb;

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

dacb *
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
	dacb *da
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
	dacb *da,
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
	dacb *da,
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
	dacb *da
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* da.h ends here */
