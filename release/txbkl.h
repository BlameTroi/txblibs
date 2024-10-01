/*
 * single file header generated via:
 * buildhdr --macro TXBKL --intro LICENSE --pub inc/kl.h --priv src/kl.c 
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
 *     in: function pointer to a comparator for keys with an
 *         interface similar to the memcmp function
 *
 * return: the new kl instance
 */

klcb *
kl_create(
	int (*fn_compare_keys)(void *, void *)
);

/*
 * kl_clone
 *
 * create a copy of a kl instance.
 *
 *     in: the kl instance to copy
 *
 * return: the copy kl instance
 */

klcb *
kl_clone(
	klcb *kl
);

/*
 * kl_destroy
 *
 * destroy an instance of a keyed linked list if it is empty.
 *
 *     in: the kl instance
 *
 * return: true if successful, false if kl was not empty
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
 *     in: the kl instance
 *
 * return: constant string with a brief message or NULL
 *
 */

const char *
kl_get_error(
	klcb *kl
);

/*
 * kl_count
 *
 * how many items are on the list?
 *
 *     in: the kl instance
 *
 * return: int number of items on the list
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
 *     in: the kl instance
 *
 * return: bool
 */

bool
kl_empty(
	klcb *kl
);

/*
 * kl_reset
 *
 * reset the keyed link list, deleting all items.
 *
 *     in: the kl instance
 *
 * return: int number of items deleted
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
 *     in: the kl instance
 *
 *     in: pointer to the key as a void *
 *
 *     in: pointer to the value as a void *
 *
 * return: bool was the insert successful?
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
 * get an item with a particular key from the list.
 *
 * if the key is found in the list, return the associated value and
 * mark the list as positioned at that key. if not, clear list
 * positioning and return NULL.
 *
 *     in: the kl instance
 *
 *     in: pointer to the address of the key
 *
 * in/out: pointer to the address to store the value as a void *
 *
 * return: bool was the key found
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
 * get the first item on the list.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a first item
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
 * get the last item on the list.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a last item
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
 * get the item following the last item read by one of the kl_get functions.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a next item
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
 * get the item before the last item read by one of the kl_get functions.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a previous item
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
 * update an item with a particular key and value on the list. the
 * item key must match the key of the last item retrieved via one of
 * the kl_get functions. the key may not be changed, but the value can
 * be.
 *
 * as items are stored in memory, if you do not change the address of
 * the value (ie, you updated its contents in place) there is no need
 * to use kl_update.
 *
 *     in: the kl instance
 *
 *     in: pointer to the key
 *
 *     in: pointer to the value
 *
 * return: did the update succeed
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
 * delete an item with a particular key on the list. the item key
 * must match the key of the last item retrieved via one of the
 * kl_get functions.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the key
 *
 * in/out: pointer to the value
 *
 * return: did the delete succeed
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
/* kl.c -- blametroi's doubly linked list functions -- */

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

#undef NDEBUG
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


/*
 * a node of the doubly linked list. the key is used for ordering
 * and must be unique within a list.
 */

typedef struct klnode klnode;

struct klnode {
	klnode *fwd;
	klnode *bwd;
	void *key;
	void *value;
};

#define KLCB_TAG "__KLCB__"
#define KLCB_TAG_LEN 8

#define ASSERT_KLCB(p, m) assert((p) && memcmp((p), KLCB_TAG, KLCB_TAG_LEN) == 0 && (m))
#define ASSERT_KLCB_OR_NULL(p) assert((p) == NULL || memcmp((p), KLCB_TAG, KLCB_TAG_LEN) == 0)

struct klcb {
	char tag[KLCB_TAG_LEN];  /* eye catcher */
	klnode *head;            /* head and tail node pointers */
	klnode *tail;
	klnode *position;        /* the last node accessed. reset by ins del */
	int (*compare_keys)(void *, void *);    /* key compare like strcmp */
	int count;               /* how many items are on the list? */
	const char *error;       /* pointer to an error message */
};

static const char *error_duplicate_key    = "duplicate key";
static const char *error_key_not_found    = "key not found";
static const char *error_list_empty       = "list empty";
static const char *error_next_at_tail     = "get next reached tail of list";
static const char *error_previous_at_head = "get previos reached head of list";
static const char *error_bad_update_key   = "update not positioned or bad key";
static const char *error_bad_delete_key   = "delete not positioned or bad key";

/*
 * kl_create
 *
 * create an instance of a keyed linked list.
 *
 *     in: function pointer to a comparator for keys with an
 *         interface similar to the memcmp function
 *
 * return: the new kl instance
 */

klcb *
kl_create(
	int (*fn_compare_keys)(void *, void *)
) {
	klcb *kl = malloc(sizeof(*kl));
	assert(kl &&
		"could not allocate KLCB");
	assert(fn_compare_keys &&
		"missing fn_compare_keys function");
	memset(kl, 0, sizeof(*kl));
	memcpy(kl->tag, KLCB_TAG, sizeof(kl->tag));
	kl->count = 0;
	kl->position = NULL;
	kl->head = NULL;
	kl->tail = NULL;
	kl->compare_keys = fn_compare_keys;
	return kl;
}

/*
 * kl_clone
 *
 * create a copy of a kl instance.
 *
 *     in: the kl instance to copy
 *
 * return: the copy kl instance
 */

klcb *
kl_clone(
	klcb *old_kl
) {
	ASSERT_KLCB(old_kl, "invalid KLCB");
	klcb *new_kl = kl_create(old_kl->compare_keys);
	if (kl_empty(old_kl))
		return new_kl;
	/* copy items */
	void *key = NULL;
	void *value = NULL;
	if (!kl_get_first(old_kl, &key, &value))
		assert(NULL &&
			"impossible error while cloning");
	kl_insert(new_kl, key, value);
	while (kl_get_next(old_kl, &key, &value))
		kl_insert(new_kl, key, value);
	return new_kl;
}

/*
 * kl_destroy
 *
 * destroy an instance of a keyed linked list if it is empty.
 *
 *     in: the kl instance
 *
 * return: true if successful, false if kl was not empty
 */

bool
kl_destroy(
	klcb *kl
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	if (!kl_empty(kl))
		return false;
	memset(kl, 253, sizeof(*kl));
	free(kl);
	return true;
}

/*
 * kl_get_error
 *
 * get status of last command if there was an error.
 *
 *     in: the kl instance
 *
 * return: constant string with a brief message or NULL
 *
 */

const char *
kl_get_error(
	klcb *kl
) {
	return kl->error;
}

/*
 * kl_count
 *
 * how many items are on the list?
 *
 *     in: the kl instance
 *
 * return: int number of items on the list
 */

int
kl_count(
	klcb *kl
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	int n = 0;
	klnode *curr = kl->head;
	while (curr) {
		n += 1;
		curr = curr->fwd;
	}
	assert(n == kl->count &&
		"kl_count error in node count");
	return kl->count;
}

/*
 * kl_empty
 *
 * is the list empty?
 *
 *     in: the kl instance
 *
 * return: bool
 */

bool
kl_empty(
	klcb *kl
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	return kl->head == NULL;
}

/*
 * kl_reset
 *
 * reset the keyed link list, deleting all items.
 *
 *     in: the kl instance
 *
 * return: int number of items deleted
 */

int
kl_reset(
	klcb *kl
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	klnode *curr = kl->head;
	klnode *next = NULL;
	int deleted = 0;
	while (curr) {
		next = curr->fwd;
		memset(curr, 253, sizeof(*curr));
		free(curr);
		deleted += 1;
		curr = next;
	}
	kl->head = NULL;
	kl->tail = NULL;
	kl->position = NULL;
	kl->error = NULL;
	assert(kl->count == deleted &&
		"kl_reset mismatch between deleted and count");
	kl->count = 0;
	return deleted;
}

/*
 * kl_insert
 *
 * insert an item with a particular key and value into the list.
 *
 *     in: the kl instance
 *
 *     in: pointer to the key as a void *
 *
 *     in: pointer to the value as a void *
 *
 * return: bool was the insert successful?
 */

bool
kl_insert(
	klcb *kl,
	void *key,
	void *value
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	/* build new list item */
	klnode *new = NULL;
	new = malloc(sizeof(*new));
	memset(new, 0, sizeof(*new));
	new->key = key;
	new->value = value;
	kl->position = NULL;
	kl->error = NULL;
	/* if the list is empty, easy peasy */
	if (kl->head == NULL) {
		kl->head = new;
		kl->tail = new;
		kl->count += 1;
		return true;
	}
	/* work the ends of the list first */
	int rf = kl->compare_keys(new->key, kl->head->key);
	int rt = kl->compare_keys(new->key, kl->tail->key);
	/* can't have duplicate keys */
	if (rf == 0 || rt == 0) {
		memset(new, 253, sizeof(*new));
		free(new);
		kl->error = error_duplicate_key;
		return false;
	}
	/* insert at head or tail if that's the position */
	if (rf < 0) {
		new->fwd = kl->head;
		kl->head = new;
		new->fwd->bwd = new;
		kl->count += 1;
		return true;
	}
	if (rt > 0) {
		new->bwd = kl->tail;
		kl->tail = new;
		new->bwd->fwd = new;
		kl->count += 1;
		return true;
	}
	/* chase the link chain and insert where appropriate */
	klnode *curr = kl->head->fwd;
	while (curr) {
		int rc = kl->compare_keys(new->key, curr->key);
		/* duplicate key, discard */
		if (rc == 0) {
			memset(new, 253, sizeof(*new));
			free(new);
			kl->error = error_duplicate_key;
			return false;
		}
		/* found insertion point yet? */
		if (rc > 0) {
			curr = curr->fwd;
			continue;
		}
		/* insert in front of current */
		new->bwd = curr->bwd;
		new->fwd = curr;
		new->bwd->fwd = new;
		curr->bwd = new;
		kl->count += 1;
		return true;
	}
	/* if we fall out of the link chase loop, something is
	 * wrong with the chain, abort. */
	assert(NULL &&
		"invalid list chain detected in kl_insert");
	return false;
}

/*
 * kl_get
 *
 * get an item with a particular key from the list.
 *
 * if the key is found in the list, return the associated value and
 * mark the list as positioned at that key. if not, clear list
 * positioning and return NULL.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address of the key
 *
 * in/out: pointer to the address to store the value as a void *
 *
 * return: bool was the key found
 */

bool
kl_get(
	klcb *kl,
	void **key,
	void **value
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	kl->position = NULL;
	kl->error = NULL;
	*value = NULL;
	if (kl->head == NULL)
		return false;
	klnode *curr = kl->head;
	while (curr) {
		int rc = kl->compare_keys(*key, curr->key);
		if (rc == 0) {
			kl->position = curr;
			*key = curr->key;
			*value = curr->value;
			return true;
		}
		if (rc < 0)
			break;
		curr = curr->fwd;
	}
	kl->error = error_key_not_found;
	return false;
}

/*
 * kl_get_first
 *
 * get the first item on the list.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a first item
 */

bool
kl_get_first(
	klcb *kl,
	void **key,
	void **value
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	kl->position = NULL;
	kl->error = NULL;
	*key = NULL;
	*value = NULL;
	if (kl->head) {
		kl->position = kl->head;
		*key = kl->position->key;
		*value = kl->position->value;
	} else
		kl->error = error_list_empty;
	return kl->position != NULL;
}

/*
 * kl_get_last
 *
 * get the last item on the list.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a last item
 */

bool
kl_get_last(
	klcb *kl,
	void **key,
	void **value
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	kl->position = NULL;
	kl->error = NULL;
	*key = NULL;
	*value = NULL;
	if (kl->tail) {
		kl->position = kl->tail;
		*key = kl->position->key;
		*value = kl->position->value;
	} else
		kl->error = error_list_empty;
	return kl->position != NULL;
}

/*
 * kl_get_next
 *
 * get the item following the last item read by one of the kl_get functions.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a next item
 */

bool
kl_get_next(
	klcb *kl,
	void **key,
	void **value
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	if (!kl->position)
		return false;
	kl->position = kl->position->fwd;
	kl->error = NULL;
	*key = NULL;
	*value = NULL;
	if (kl->position) {
		*key = kl->position->key;
		*value = kl->position->value;
	} else
		kl->error = error_next_at_tail;
	return kl->position != NULL;
}

/*
 * kl_get_previous
 *
 * get the item before the last item read by one of the kl_get functions.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the address to store the key
 *
 * in/out: pointer to the address to store the value
 *
 * return: bool was there a previous item
 */

bool
kl_get_previous(
	klcb *kl,
	void **key,
	void **value
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	if (!kl->position)
		return false;
	kl->position = kl->position->bwd;
	kl->error = NULL;
	*key = NULL;
	*value = NULL;
	if (kl->position) {
		*key = kl->position->key;
		*value = kl->position->value;
	} else
		kl->error = error_previous_at_head;
	return kl->position != NULL;
}

/*
 * kl_update
 *
 * update an item with a particular key and value on the list. the
 * item key must match the key of the last item retrieved via one of
 * the kl_get functions. the key may not be changed, but the value can
 * be.
 *
 * as items are stored in memory, if you do not change the address of
 * the value (ie, you updated its contents in place) there is no need
 * to use kl_update.
 *
 *     in: the kl instance
 *
 *     in: pointer to the key
 *
 *     in: pointer to the value
 *
 * return: did the update succeed
 */

bool
kl_update(
	klcb *kl,
	void *key,
	void *value
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	kl->error = NULL;
	if (kl->position == NULL ||
		kl->compare_keys(key, kl->position->key) != 0) {
		kl->position = NULL;
		kl->error = error_bad_update_key;
		return false;
	}
	kl->position->value = value;
	return true;
}

/*
 * kl_delete
 *
 * delete an item with a particular key on the list. the item key
 * must match the key of the last item retrieved via one of the
 * kl_get functions.
 *
 *     in: the kl instance
 *
 * in/out: pointer to the key
 *
 * in/out: pointer to the value
 *
 * return: did the delete succeed
 */

bool
kl_delete(
	klcb *kl,
	void *key
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	kl->error = NULL;
	if (kl->position == NULL ||
		kl->compare_keys(key, kl->position->key) != 0) {
		kl->position = NULL;
		kl->error = error_bad_delete_key;
		return false;
	}
	/* deletes clear position */
	klnode *curr = kl->position;
	kl->position = NULL;
	if (curr->fwd == NULL && curr->bwd == NULL) {
		/* this is the only item */
		kl->head = NULL;
		kl->tail = NULL;
	} else if (curr->bwd == NULL) {
		/* this is the head */
		kl->head = curr->fwd;
		curr->fwd->bwd = NULL;
	} else if (curr->fwd == NULL) {
		/* is this the tail? */
		kl->tail = curr->bwd;
		curr->bwd->fwd = NULL;
	} else {
		/* somewhere in the middle */
		curr->bwd->fwd = curr->fwd;
		curr->fwd->bwd = curr->bwd;
	}
	memset(curr, 253, sizeof(*curr));
	free(curr);
	kl->count -= 1;
	return true;
}

/* kl.c ends here */
/* *** end priv *** */

#endif /* TXBKL_IMPLEMENTATION */
