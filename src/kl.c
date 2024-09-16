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

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/kl.h"

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
 * takes a function pointer to a comparator for keys with an
 * interface similar to the memcmp function.
 *
 * returns a klcb, the keyed linked list instance.
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
 * returns a char * or NULL.
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
 * how many entries are on the list?
 *
 * takes the keyed list instance as input.
 *
 * returns an int.
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
 * takes the keyed list instance as input.
 *
 * returns a bool.
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
 * reset the keyed link list, deleting all entries.
 *
 * takes the keyed list instance as input.
 *
 * returns an int, the number of entries that were on the list.
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
};

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
) {
	ASSERT_KLCB(kl, "invalid KLCB");
	/* build new list entry */
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
