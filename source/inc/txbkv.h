/* txbkv.h -- blametroi's key:value store library -- */

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

#ifndef TXBKV_H
#define TXBKV_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * a handle to a key:value store instance.
 */

typedef struct kvcb hkv;

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

hkv *
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
	hkv* kv
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
	hkv *kv
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
	hkv *kv,
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
	hkv *kv,
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
	hkv *kv,
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
	hkv *kv
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
	hkv *kv
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
	hkv *kv
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
	hkv *kv
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBKV_H */

#ifdef TXBKV_IMPLEMENTATION
#undef TXBKV_IMPLEMENTATION

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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "txbabort.h"

/*
 * a key:value store contains an ordered array of key:value pairs. the
 * key:value pairs contain poiters to the key and value for an item in
 * the key:value store.
 *
 * while it is *very* redundant, each kvpair carries a reference to
 * its owning kvcb. without using extensions for blocks/lambdas, which
 * are very different between gcc and clang, there's no way to get the
 * actual key sort function when sorting pairs without a global
 * variable.
 *
 * actually, there may be a way, but i would need to give up the
 * memcmp/strcmp compatability of the key compare functions. let's do
 * this for now.
 *
 * to work with bsearch(), the first part of the pair must be the
 * key, or does it?
 */

typedef struct kvpair kvpair;
struct kvpair {
	hkv *owner;
	pkey key;
	pvalue value;
};

/*
 * the transparent key:value control block definition.
 */

#define PAIRS_SIZE_DEFAULT 100
#define PAIRS_INCREMENT_DEFAULT 100

#define HKV_TAG "__HKV___"
#define HKV_TAG_LEN 8

#define ASSERT_HKV(p, m) \
	abort_if(!(p) || memcmp((p), HKV_TAG, HKV_TAG_LEN) != 0, (m));

#define ASSERT_HKV_OR_NULL(p, m) \
	abort_if(p && memcmp((p), HKV_TAG, HKV_TAG_LEN) != 0, (m));

/*
 * we need to be able to compare kvpairs and that requires that
 * we have visibility to the key compare function carried in the
 * kvcb. kvpairs now carry a pointer to their owning kvcb,
 * increasing size by 50%.
 */

struct kvcb {
	char tag[HKV_TAG_LEN];     /* eye catcher */
	int (*key_compare)(const void *, const void *);
	int pairs_size;             /* how many can we store */
	int pairs_increment;        /* if we can increase buffer, by how much */
	int num_pairs;              /* how many are stored */
	kvpair *pairs;              /* where they are */
};

/*
 * the client provides a comparator function to compare two distinct
 * keys. the fn_kvpair_compare_keys extracts the keys from the kvpair
 * for the client comparator.
 */

static
int
fn_kvpair_compare_keys(const void *a, const void *b) {
	return ((kvpair *)a)->owner->key_compare(
		((kvpair *)a)->key, ((kvpair *)b)->key);
}

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

hkv *
kv_create(
	int (*key_compare)(const void *, const void *)
) {
	abort_if(!key_compare,
		"kv_create missing key compare function for HKV");

	hkv *kv = malloc(sizeof(*kv));
	memset(kv, 0, sizeof(*kv));
	memcpy(kv->tag, HKV_TAG, HKV_TAG_LEN);

	kv->pairs_increment = PAIRS_INCREMENT_DEFAULT;
	kv->pairs_size = PAIRS_SIZE_DEFAULT;
	kv->pairs = malloc(sizeof(kvpair) * (kv->pairs_size + 1));
	memset(kv->pairs, 0, sizeof(kvpair) * (kv->pairs_size + 1));

	kv->key_compare = key_compare;
	kv->num_pairs = 0;
	return kv;
}

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
	hkv *kv
) {
	ASSERT_HKV(kv, "invalid HKV");

	if (kv->pairs != NULL)
		memset(kv->pairs, 0, sizeof(kvpair) * (kv->pairs_size + 1));

	int i = kv->num_pairs;
	kv->num_pairs = 0;
	return i;
}

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
	hkv *kv
) {
	ASSERT_HKV(kv, "invalid HKV");
	if (kv->num_pairs != 0)
		return false;

	memset(kv->pairs, 253, sizeof(kvpair) * (kv->pairs_size + 1));
	free(kv->pairs);

	memset(kv, 253, sizeof(*kv));
	free(kv);

	return true;
}

/*
 * am_find_key
 *
 * find a matching key in the key:value store.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 * return: key:value pair or NULL
 */

static
kvpair *
am_find_key(
	hkv *kv,
	pkey key
) {
	kvpair search_key = (kvpair) { kv, key, NULL };
	kvpair *p = bsearch(&search_key, kv->pairs, kv->num_pairs, sizeof(kvpair),
		fn_kvpair_compare_keys);
	return p;
}

/*
 * am_delete_pair
 *
 * delete the key:value pair for a specific key. fails via an assert
 * if the key is not in the store.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 * return: nothing
 */

static
void
am_delete_pair(
	hkv *kv,
	kvpair *p
) {
	int ix = p - kv->pairs;
	memmove(p, p+1, (kv->num_pairs - ix - 1) * sizeof(kvpair));
	kv->num_pairs -= 1;
	return;
}

/*
 * am_new_pair
 *
 * given a key and value, create a new
 * key:value pair in the
 * underlying store. when called it should be known that key does
 * not alrady exist in the store.
 *
 * if the store is full and growth is allowed, a new store is
 * allocated.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 *     in: the value
 *
 * return: the newly created kvpair
 */

static
kvpair *
am_new_pair(
	hkv *kv,
	pkey key,
	pvalue value
) {
	if (kv->num_pairs >= kv->pairs_size) {

		int new_size = kv->pairs_size + kv->pairs_increment;
		kvpair *new_pairs = malloc((new_size + 1) * sizeof(kvpair));
		memset(new_pairs, 0, (new_size + 1) * sizeof(kvpair));
		memcpy(new_pairs, kv->pairs, (kv->pairs_size + 1) * sizeof(kvpair));

		memset(kv->pairs, 0xfd, (kv->pairs_size + 1) * sizeof(kvpair));
		free(kv->pairs);

		kv->pairs = new_pairs;
		kv->pairs_size = new_size;
	}

	/* if pairs are empty, easy peasy */
	if (kv->num_pairs == 0) {
		kv->pairs[0] = (kvpair) { kv, key, value };
		kv->num_pairs += 1;
		return &kv->pairs[0];
	}

	/* a few quick checks before resorting to a sort. remember:
	 * this function is not called if key is already in pairs!
	 *
	 * if the new key is outside the current range, append or
	 * shift and insert depending on which end. as i believe new
	 * keys could be between existing 0 & 1, or existing n-1 & n,
	 * deal with those similarly.
	 *
	 * stdlib provides mergesort as an alternative to qsort.
	 * mergesort will perform better than qsort since the existing
	 * keys are already sorted. */

	/* if new comes after the current last, append */
	if (kv->key_compare(key, kv->pairs[kv->num_pairs-1].key) > 0) {
		kv->pairs[kv->num_pairs] = (kvpair) { kv, key, value };
		kv->num_pairs += 1;
		return &kv->pairs[kv->num_pairs-1];
	}

	/* if new comes before the current first, shift */
	if (kv->key_compare(key, kv->pairs[0].key) < 0) {
		memmove(&kv->pairs[1], &kv->pairs[0], (kv->num_pairs) * sizeof(kvpair));
		kv->pairs[0] = (kvpair) { kv, key, value };
		kv->num_pairs += 1;
		return &kv->pairs[0];
	}

	/* if new comes before current last but after current penultimate,
	 * shift current last and insert */
	if (kv->key_compare(key, kv->pairs[kv->num_pairs-2].key) > 0) {
		kv->pairs[kv->num_pairs] = kv->pairs[kv->num_pairs-1];
		kv->pairs[kv->num_pairs-1] = (kvpair) { kv, key, value };
		kv->num_pairs += 1;
		return &kv->pairs[kv->num_pairs-2];
	}

	/* if new comes after current first but before current second,
	 * shift and insert */
	if (kv->key_compare(key, &kv->pairs[0]) < 0) {
		memmove(&kv->pairs[2], &kv->pairs[1], (kv->num_pairs-1) * sizeof(kvpair));
		kv->pairs[1] = (kvpair) { kv, key, value };
		kv->num_pairs += 1;
		return &kv->pairs[1];
	}

	/* that's enough cuteness, just append and mergesort. mergesort
	 * works best on already mostly ordered data. */
	kv->pairs[kv->num_pairs] = (kvpair) { kv, key, value };
	kv->num_pairs += 1;
	mergesort(kv->pairs, kv->num_pairs, sizeof(kvpair), fn_kvpair_compare_keys);
	return am_find_key(kv, key);
}

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
	hkv *kv,
	pkey key
) {
	ASSERT_HKV(kv, "kv_get invalid HKV");
	abort_if(!key,
		"kv_get key may not be NULL");

	kvpair *p = am_find_key(kv, key);
	return p ? p->value : p;
}

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
	hkv *kv,
	pkey key,
	pvalue value
) {
	ASSERT_HKV(kv, "kv_put invalid HKV");
	abort_if(!key,
		"kv_put key may not be NULL");

	kvpair *p = am_find_key(kv, key);
	if (p)
		p->value = value;
	else
		p = am_new_pair(kv, key, value);

	return value;
}

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
	hkv *kv,
	pkey key
) {
	ASSERT_HKV(kv, "kv_delete invalid HKV");
	abort_if(!key,
		"kv_delete key may not be NULL");

	kvpair *p = am_find_key(kv, key);
	if (p)
		am_delete_pair(kv, p);

	return p != NULL;
}

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
	hkv *kv
) {
	ASSERT_HKV(kv, "invalid HKV");
	return kv->num_pairs == 0;
}

/*
 * kv_count
 *
 * how many pairs are in the key:value store.
 *
 *     in: the kv instance
 *
 * return: intger number of pairs
 */

int
kv_count(
	hkv *kv
) {
	ASSERT_HKV(kv, "invalid HKV");
	return kv->num_pairs;
}

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
	hkv *kv
) {
	ASSERT_HKV(kv, "invalid HKV");
	if (kv->num_pairs == 0)
		return NULL;

	pkey *keys = malloc((kv->num_pairs + 1) * sizeof(void *));
	memset(keys, 0, (kv->num_pairs + 1) * sizeof(void *));

	for (int i = 0; i < kv->num_pairs; i++)
		keys[i] = kv->pairs[i].key;

	return keys;
}

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
	hkv *kv
) {
	ASSERT_HKV(kv, "invalid HKV");
	if (kv->num_pairs == 0)
		return NULL;
	pvalue *values = malloc((kv->num_pairs + 1) * sizeof(void *));
	memset(values, 0, (kv->num_pairs + 1) * sizeof(void *));

	for (int i = 0; i < kv->num_pairs; i++)
		values[i] = kv->pairs[i].value;

	return values;
}

#endif /* TXBKV_IMPLEMENTATION */
/* txbkv.h ends here */
