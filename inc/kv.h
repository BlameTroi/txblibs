/* kv.h -- blametroi's key:value store functions -- */

/*
 * a header only implementation of a key:value store. it's not
 * really a hash table or dictionary, but eventually its backing
 * store might be either.
 *
 * problems in the advent of code series present opportunities
 * to use various abstract data types and i've been using aoc as
 * a prompt to implement my own versions. i finally saw a need
 * for a better key:value system and started to implement a binary
 * search tree, but the particular problem would tend to load the
 * tree in an unbalancing manner. i really didn't want to do a
 * more complex implementation at that time.
 *
 * i finally settled on creating a 'good enough' access api that
 * could have any backing hidden behind it.
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
 * kv_create
 *
 * creates an instance of the key:value store.
 *
 * requires a function pointer to a function that will compare the
 * keys in the store via the < =0 > convention.
 *
 *     in: key comparison function pointer, as in qsort.
 *
 * return: the new kv instance
 */

kvcb *
kv_create(
	int (*key_compare)(void *, void *)
);

/*
 * kv_reset
 *
 * delets all key:value pairs from the store.
 *
 *     in: the kv instance
 *
 * return: how many pairs were deleted
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
 * return: NULL
 */

bool
kv_destroy(
	kvcb *kv
);

/*
 * kv_get
 *
 * if the key exists in the key:value store, return the pointer
 * to the value.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 * return: the value from the key:value pair or NULL if not found
 */

void *
kv_get(
	kvcb *kv,
	void *key
);

/*
 * kv_put
 *
 * given pointers to a key and associated value, store them in the
 * key:value store.
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

void *
kv_put(
	kvcb *kv,
	void *key,
	void *value
);

/*
 *
 */

bool
kv_delete(
	kvcb *kv,
	void *key
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
 * return: int number of pairs
 */

int
kv_count(
	kvcb *kv
);

/*
 * kv_keys
 *
 * returns a null terminated array of keys from the store.
 *
 *     in: the kv instance
 *
 * return: null terminated array of void *
 */

void *
kv_keys(
	kvcb *kv
);

/*
 * kv_values
 *
 * returns a null terminated array of values from the store.
 *
 *     in: the kv instance
 *
 * return: null terminated array of void *
 */

void *
kv_values(
	kvcb *kv
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* kv.h ends here */
