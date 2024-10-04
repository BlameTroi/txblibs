/*
 * single file header generated via:
 * buildhdr --macro TXBKV --intro LICENSE --pub inc/kv.h --priv src/kv.c 
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Troy Brumley 
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   *** end intro ***
 */

#ifndef TXBKV_SINGLE_HEADER
#define TXBKV_SINGLE_HEADER
/* *** begin pub *** */
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
	int (*key_compare)(const void *, const void *)
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
/* *** end pub *** */

#endif /* TXBKV_SINGLE_HEADER */

#ifdef TXBKV_IMPLEMENTATION
#undef TXBKV_IMPLEMENTATION
/* *** begin priv *** */
/* kv.c -- blametroi's key:value store library */

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

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
	kvcb *owner;
	void *key;
	void *value;
};

/*
 * the transparent key:value control block definition.
 */

#define PAIRS_SIZE_DEFAULT 100
#define PAIRS_INCREMENT_DEFAULT 100

#define KVCB_TAG "__KVCB__"
#define KVCB_TAG_LEN 8

#define ASSERT_KVCB(p, m) assert((p) && memcmp((p), KVCB_TAG, KVCB_TAG_LEN) == 0 && (m))
#define ASSERT_KVCB_OR_NULL(p) assert((p) == NULL || memcmp((p), KVCB_TAG, KVCB_TAG_LEN) == 0)

/*
 * we need to be able to compare kvpairs and that requires that
 * we have visibility to the key compare function carried in the
 * kvcb. kvpairs now carry a pointer to their owning kvcb,
 * increasing size by 50%.
 */

struct kvcb {
	char tag[KVCB_TAG_LEN];     /* eye catcher */
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
 * requires a function pointer to a function that will compare the
 * keys in the store via the < =0 > convention.
 *
 *     in: key comparison function pointer, as in qsort.
 *
 * return: the new kv instance
 */

kvcb *
kv_create(
	int (*key_compare)(const void *, const void *)
) {
	assert(key_compare &&
		"missing key compare function pointer for KVCB");
	kvcb *kv = malloc(sizeof(kvcb));
	assert(kv &&
		"failed to allocate KVCB");
	memset(kv, 0, sizeof(kvcb));
	memcpy(kv->tag, KVCB_TAG, KVCB_TAG_LEN);
	kv->pairs_increment = PAIRS_INCREMENT_DEFAULT;
	kv->pairs_size = PAIRS_SIZE_DEFAULT;
	kv->pairs = malloc(sizeof(kvpair) * (kv->pairs_size + 1));
	assert(kv->pairs &&
		"failed to allocate or initialize pair backing for KVCB");
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
 * return: how many pairs were deleted
 */

int
kv_reset(
	kvcb *kv
) {
	ASSERT_KVCB(kv, "invalid KVCB");

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
 * overwrites and then releases all key:value store related storage.
 * actual key and value storage is the responsibility of the client.
 *
 *     in: the kv instance
 *
 * return: NULL
 */

bool
kv_destroy(
	kvcb *kv
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	if (kv->num_pairs != 0)
		return false;

	memset(kv->pairs, 0xfd, sizeof(kvpair) * (kv->pairs_size + 1));
	free(kv->pairs);

	memset(kv, 0xfd, sizeof(kvcb));
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
	kvcb *kv,
	void *key
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
	kvcb *kv,
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
 * given key and value pointers, create a new key:value pair in the
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
	kvcb *kv,
	void *key,
	void *value
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
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	assert(key && "key may not be NULL");
	kvpair *p = am_find_key(kv, key);
	return p ? p->value : p;
}

/*
 * kv_put
 *
 * given pointers to a key value, store them in the key:value store.
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
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	assert(key && "key may not be NULL");
	assert(value && "value may not be NULL");
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
 * given a key, if it exists in the key:value store, delete its pair
 * in the store.
 *
 *     in: the kv instance
 *
 *     in: the key
 *
 * return: boolean true if key found and pair deleted
 */

bool
kv_delete(
	kvcb *kv,
	void *key
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	assert(key && "key may not be NULL");
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
	kvcb *kv
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	return kv->num_pairs == 0;
}

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
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	return kv->num_pairs;
}

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
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	if (kv->num_pairs == 0)
		return NULL;
	void **keys = malloc((kv->num_pairs + 1) * sizeof(void *));
	memset(keys, 0, (kv->num_pairs + 1) * sizeof(void *));
	for (int i = 0; i < kv->num_pairs; i++) {
		keys[i] = kv->pairs[i].key;
	}
	return keys;
}

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
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	if (kv->num_pairs == 0)
		return NULL;
	void **values = malloc((kv->num_pairs + 1) * sizeof(void *));
	memset(values, 0, (kv->num_pairs + 1) * sizeof(void *));
	for (int i = 0; i < kv->num_pairs; i++) {
		values[i] = kv->pairs[i].value;
	}
	return values;
}

/* kv.c ends here */
/* *** end priv *** */

#endif /* TXBKV_IMPLEMENTATION */
