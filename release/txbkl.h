/*
 * single file header generated via:
 * buildhdr --macro TXBKL --intro LICENSE --pub inc/kl.h --priv src/kv.c 
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

#ifndef TXBKL_SINGLE_HEADER
#define TXBKL_SINGLE_HEADER
/* *** begin pub *** */
/* kl.h -- blametroi's utility functions -- */

/*
 * a header only implementation of a keyed doubly linked list.
 *
 * the list is kept in order by a unique key using a client supplied
 * comparison function that returns an integer as memcmp would.
 *
 * keys and the values to add to the list are passed by reference
 * as void *. if the value would fit in sizeof(void *) it may be
 * passed directly.
 *
 * storage management for keys and values is the responsibility of
 * the client.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <pthread.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct klcb klcb;

/*
 * kl_create
 *
 * create an instance of a keyed linked list.
 *
 * takes a function pointer to a comparator for keys with an
 * interface similar to the memcmp function.
 *
 * returns a klcb, the keyed linked list instance.
 */

klcb *
kl_create(
	int (*fn_compare_keys)(void *, void *)
);

/*
 * kl_destroy
 *
 * destroy an instance of a keyed linked list if it is empty.
 *
 * takes the keyed list instance as input.
 *
 * returns true if destruction successful, false otherwise.
 */

bool
kl_destroy(
	klcb *kl
);

/*
 * kl_get_error
 *
 * get status of last command if there was an error.
 *
 * returns a char * or NULL.
 *
 */

const char *
kl_get_error(
	klcb *kl
);

/*
 * kl_count
 *
 * how many entries are on the list?
 *
 * takes the keyed list instance as input.
 *
 * returns an int.
 */

int
kl_count(
	klcb *kl
);

/*
 * kl_empty
 *
 * is the list empty?
 *
 * takes the keyed list instance as input.
 *
 * returns a bool.
 */

bool
kl_empty(
	klcb *kl
);

/*
 * kl_reset
 *
 * reset the keyed link list, deleting all entries.
 *
 * takes the keyed list instance as input.
 *
 * returns an int, the number of entries that were on the list.
 */

int
kl_reset(
	klcb *kl
);

/*
 * kl_insert
 *
 * insert an item with a particular key and value into the list.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the key, as a void *.
 *
 * the address of the value, as a void *.
 *
 * returns a bool, true if the insert succeeds, false if it failed.
 */

bool
kl_insert(
	klcb *kl,
	void *key,
	void *value
);

/*
 * kl_get
 *
 * get an entry with a particular key on the list.
 *
 * if the key is found in the list, return the associated value and
 * mark the list as positioned at that key. if not, clear list
 * positioning and return NULL.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
 */

bool
kl_get(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_first
 *
 * get the first entry on the list.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
 */

bool
kl_get_first(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_last
 *
 * get the last entry on the list.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
 */

bool
kl_get_last(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_next
 *
 * get the entry following the last entry read by one of the kl_get functions.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
 */

bool
kl_get_next(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_previous
 *
 * get the entry before the last entry read by one of the kl_get functions.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
 */

bool
kl_get_previous(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_update
 *
 * update an entry with a particular key and value on the list. the
 * entry key must match the key of the last entry retrieved via one of
 * the kl_get functions. the key may not be changed, but the value is
 * updated.
 *
 * note: as items are stored in memory, if you do not change the
 *       address of the value (ie, you updated its contents in place)
 *       there is no need to use kl_update.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the key, as a void *.
 *
 * the address of the value, as a void *.
 *
 * returns a bool, true if the update succeeds, false if it failed.
 */

bool
kl_update(
	klcb *kl,
	void *key,
	void *value
);

/*
 * kl_delete
 *
 * delete an entry with a particular key on the list. the entry key
 * must match the key of the last entry retrieved via one of the
 * kl_get functions.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the key, as a void *.
 *
 * returns a bool, true if the delete succeeds, false if it failed.
 */

bool
kl_delete(
	klcb *kl,
	void *key
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* kl.h ends here */
/* *** end pub *** */

#endif /* TXBKL_SINGLE_HEADER */

#ifdef TXBKL_IMPLEMENTATION
#undef TXBKL_IMPLEMENTATION
/* *** begin priv *** */
/* kv.c -- blametroi's key:value store library */

/*
 * a header only implementation of a very basic key:value store.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

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
	kvpair *pairs;                /* where they are */
};


/*
 * overhead for the backing store for key value pairs.
 */

#define KVIT_TAG "__KVIT__"
#define KVIT_TAG_LEN 8
#define ASSERT_KVIT(p) assert((p) && memcmp((p), KVIT_TAG, KVCB_TAG_LEN) == 0 && (m))
#define ASSERT_KVIT_OR_NULL(p) assert((p) == NULL || memcmp((p), KVIT_TAG, KVIT_TAG_LEN) == 0)

struct kvit {
	char tag[KVIT_TAG_LEN];     /* eye catcher */
	void *amov;                 /* overhead for the backing access method */
	void *key;                  /* whatever the key is */
	void *value;                /* whatever the payload is */
};


/*
 * kv_create
 *
 * creates an instance of the key:value store.
 *
 * requires a function pointer to a function that will compare the
 * keys in the store via the < =0 > convention.
 *
 * returns a pointer to the instance as a kvcb.
 */

kvcb *
kv_create(
	int (*key_compare)(void *, void *)
) {
	assert(key_compare && "missing key compare function pointer for KVCB");
	kvcb *kv = malloc(sizeof(kvcb));
	assert(kv && "failed to allocate KVCB");
	memset(kv, 0, sizeof(kvcb));
	memcpy(kv->tag, KVCB_TAG, KVCB_TAG_LEN);
	kv->pairs_increment = PAIRS_INCREMENT_DEFAULT;
	kv->pairs_size = PAIRS_SIZE_DEFAULT;
	kv->pairs = malloc(sizeof(kvpair) * (kv->pairs_size + 1));
	assert(kv->pairs && "failed to allocate or initialize pair backing for KVCB");
	memset(kv->pairs, 0, sizeof(void *) * 2 * (kv->pairs_size + 1));
	kv->key_compare = key_compare;
	kv->num_pairs = 0;
	return kv;
}


/*
 * kv_destroy
 *
 * destroy an instance of the key:value pair store.
 *
 * overwrites and then releases all key:value store related storage.
 * actual key and value storage is the responsibility of the client.
 *
 * return NULL.
 */

kvcb *
kv_destroy(
	kvcb *kv
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	if (kv->pairs != NULL) {
		memset(kv->pairs, 253, kv->num_pairs * sizeof(kvpair) * 2);
		free(kv->pairs);
	}
	memset(kv, 253, sizeof(kvcb));
	free(kv);
	return NULL;
}


/*
 * am_find_key
 *
 * find a matching key in the key:value store.
 *
 * returns the address of the key:value pair or NULL.
 */

static
void *
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
 * returns a pointer to the new pair.
 */

static
kvpair *
am_new_pair(kvcb *kv, void *key, void *value) {
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
 * returns NULL if the key is not found.
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
 * returns the value passed.
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
 * returns true if the key was found, false if not.
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
 * kv_exists
 *
 * does a key exist in the key:value store.
 */

bool
kv_exists(
	kvcb *kv,
	void *key
) {
	ASSERT_KVCB(kv, "invalid KVCB");
	assert(key && "key may not be NULL");
	kvpair *p = am_find_key(kv, key);
	return p != NULL;
}


/*
 * kv_empty
 *
 * is the key:value store empty.
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

#endif /* TXBKL_IMPLEMENTATION */
