/* kv.h -- blametroi's key:value store functions -- */

/*
 * a header only implementation of a key:value store. it's not really
 * a hash table or dictionary, but eventually its backing store might
 * be either.
 *
 * problems in the advent of code series present opportunities to use
 * various abstract data types and i've been using aoc as a prompt to
 * implement my own versions. i finally saw a need for a better
 * key:value system and started to implement a binary search tree, but
 * the problem that inspired this effort would tend to load the tree
 * in an unbalancing manner.
 *
 * not wanting to do a more complex implementation, i finally settled
 * on creating a 'good enough' access api that could have any backing
 * store hidden behind it. when I feel like doing a proper hash or
 * binary search tree, it will help out here.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * the opaque key value control block.
 */

typedef struct kvcb kvcb;

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
 * kv_create
 *
 * creates an instance of the key:value store.
 *
 * this requires a function pointer to a function that will compare
 * the keys using the <0 =0 >0 convention as for qsort.
 *
 *     in: key comparison function pointer
 *
 * return: the new kv instance
 */

kvcb *
kv_create(
	int (*key_compare)(const void *, const void *)
);

/*
 * kv_reset
 *
 * delets all key:value pairs from the store.
 *
 *     in: the kv instance
 *
 * return: integer how many pairs were deleted
 */

int
kv_reset(
	kvcb* kv
);

/*
 * kv_destroy
 *
 * destroy an instance of the key:value pair store.
 *
 *     in: the kv instance
 *
 * return: boolean was the kv destroyed?
 */

bool
kv_destroy(
	kvcb *kv
);

/*
 * kv_get
 *
 * if the key exists in the key:value store, return the value.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 * return: the value from the key:value pair or NULL if not found
 */

pvalue
kv_get(
	kvcb *kv,
	pkey key
);

/*
 * kv_put
 *
 * given a key and associated value, store the pair in the key:value
 * store.
 *
 * if the key exists in the store, its value is overwritten. if the
 * key does not exist in the store, a new key:value pair is created.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 *     in: the value
 *
 * return: the value passed as input
 */

pvalue
kv_put(
	kvcb *kv,
	pkey key,
	pvalue value
);

/*
 * kv_delete
 *
 * given a key, remove the associated key:value pair from the store.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 * return: boolean was anything deleted?
 */

bool
kv_delete(
	kvcb *kv,
	pkey key
);

/*
 * kv_empty
 *
 * is the key:value store empty?
 *
 *     in: the kv instance
 *
 * return: boolean true if empty
 */

bool
kv_empty(
	kvcb *kv
);

/*
 * kv_count
 *
 * how many pairs are in the key:value store.
 *
 *     in: the kv instance
 *
 * return: integer number of pairs
 */

int
kv_count(
	kvcb *kv
);

/*
 * kv_keys
 *
 * returns a list of all the keys in the store.
 *
 *     in: the kv instance
 *
 * return: NULL terminated list (array) of keys.
 */

pkey *
kv_keys(
	kvcb *kv
);

/*
 * kv_values
 *
 * returns a list of all the values in the store.
 *
 *     in: the kv instance
 *
 * return: NULL terminated list (array) of values.
 */

pvalue *
kv_values(
	kvcb *kv
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* kv.h ends here */
