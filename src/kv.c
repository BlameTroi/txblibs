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

#include "../inc/kv.h"

/*
 * the transparent key value control block.
 */

#define KVCB_TAG "__KVCB__"
#define KVCB_TAG_LEN 8

#define ASSERT_KVCB(p, m) assert((p) && memcmp((p), KVCB_TAG, KVCB_TAG_LEN) == 0 && (m))
#define ASSERT_KVCB_OR_NULL(p) assert((p) == NULL || memcmp((p), KVCB_TAG, KVCB_TAG_LEN) == 0)

#define PAIRS_SIZE_DEFAULT 100
#define PAIRS_INCREMENT_DEFAULT 100

typedef struct kvpair kvpair;
struct kvpair {
	void *key;
	void *value;
};

struct kvcb {
	char tag[KVCB_TAG_LEN];     /* eye catcher */
	int (*key_compare)(void *, void *);
	int pairs_size;             /* how many can we store */
	int pairs_increment;        /* if we can increase buffer, by how much */
	int num_pairs;              /* how many are stored */
	kvpair *pairs;              /* where they are */
};

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
) {
	assert(key_compare && "missing key compare function pointer for KVCB");
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

	memset(kv->pairs, 253, sizeof(kvpair) * (kv->pairs_size + 1));
	free(kv->pairs);

	memset(kv, 253, sizeof(kvcb));
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
	for (int i = 0; i < kv->num_pairs; i++)
		if (kv->key_compare(key, kv->pairs[i].key) == 0)
			return &kv->pairs[i];
	return NULL;
}

/*
 * am_delete_key
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
am_delete_key(
	kvcb *kv,
	void *key
) {
	for (int i = 0; i < kv->num_pairs; i++)
		if (kv->key_compare((void *)key, kv->pairs[i].key) == 0) {
			/* we could swap with an empty slot at the end
			 * but that limits us to unordered, and that
			 * won't always be the case. */
			kv->pairs[i].key = NULL;
			kv->pairs[i].value = NULL;
			for (int j = i+1; j < kv->num_pairs; j++) {
				kv->pairs[i] = kv->pairs[j];
				i += 1;
			}
			kv->num_pairs -= 1;
			return;
		}
	assert(NULL && "error in am_delete_key, could not find key");
}

/*
 * am_new_pair
 *
 * given key and value pointers, create a new key:value pair in the
 * underlying store.
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

/* memmove (void *dest, const void *src, size_t len) overlap allowed */

static
kvpair *
am_new_pair(
	kvcb *kv,
	void *key,
	void *value
) {
	if (kv->num_pairs >= kv->pairs_size) {
		assert(kv->pairs_increment && "can not grow key:value store");
		int new_size = kv->pairs_size + kv->pairs_increment;
		kvpair *new_pairs = malloc((new_size + 1) * sizeof(kvpair));
		memset(new_pairs, 0, (new_size + 1) * sizeof(kvpair));
		memcpy(new_pairs, kv->pairs, (kv->pairs_size + 1) * sizeof(kvpair));
		memset(kv->pairs, 253, (kv->pairs_size + 1) * sizeof(kvpair));
		free(kv->pairs);
		kv->pairs = new_pairs;
		kv->pairs_size = new_size;
	}
	kvpair *p = &kv->pairs[kv->num_pairs];
	p->key = key;
	p->value = value;
	kv->num_pairs += 1;
	return p;
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
		am_delete_key(kv, key);
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
