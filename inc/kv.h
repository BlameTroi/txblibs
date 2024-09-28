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


typedef struct kvcb kvcb;

kvcb *
kv_create(
	int (*key_compare)(void *, void *)
);

kvcb *
kv_destroy(
	kvcb *kv
);

void *
kv_insert(
	kvcb *kv,
	void *key
);

void *
kv_put(
	kvcb *kv,
	void *key,
	void *value
);

bool
kv_delete(
	kvcb *kv,
	void *key
);

bool
kv_exists(
	kvcb *kv,
	void *key
);

bool
kv_empty(
	kvcb *kv
);

int
kv_count(
	kvcb *kv
);

void *
kv_keys(
	kvcb *kv
);

void *
kv_values(
	kvcb *kv
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* kv.h ends here */
