/*
 * single file header generated via:
 * buildhdr --macro TXBDL --intro LICENSE --pub inc/dl.h --priv src/dl.c 
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

#ifndef TXBDL_SINGLE_HEADER
#define TXBDL_SINGLE_HEADER
/* *** begin pub *** */
/* dl.h -- blametroi's doubly linked list functions -- */

/*
 * a header only implementation of a doubly linked list.
 *
 * the list is kept in order by a key, which can be either an
 * identifying long integer, or by some unique value in the payload
 * that each list node carries.
 *
 * each list will have a control block containing the approriate
 * counters, links, configuration information, and when function
 * pointers for routines to compare payload key values and to
 * dynamically free payload storage when a node is freed.
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

/*
 * double list control block holding chain pointers and other control
 * information. if a payload is carried, function pointers to free the
 * payload and also perform comparisons for ordering are put here. an
 * id field is provided for ordering and is set from an odometer
 * value.
 *
 * all functions other than list creation take as their first argument
 * a pointer to an instance of this control block.
 */

typedef struct dlcb dlcb;

/*
 * forward declarations for all functions.
 *
 * i hope that argument names are sufficiently wordy to aid in
 * understanding. more complete comment blocks are included with
 * the implementations.
 *
 * error checking and handling is limited. if things look bad,
 * execution is halted via an assert(). this is only done in cases
 * where continued execution is unwise, as in the list control block
 * is not initialized, or it appears that the link chain is broken.
 */

/*
 * create a new doubly linked list instance. nodes in the list are
 * uniquely identified by either an id field (a positive long integer)
 * or by some part of the payload that can be compared with other
 * payloads using a helper function.
 *
 * if a payload key is used, the id argument can be any value and is
 * ignored. internally it is set to an odometer value.
 *
 * if an id is used, it must be a long greater than zero. when
 * inserting a node, an id of zero indicates that an id should be
 * assigned automatically.
 *
 * if successful a doubly linked list control block (dlcb) pointer
 * is returned.
 */

dlcb *
dl_create_by_id(
	bool threaded,
	void (*free_payload)(void *)
);

dlcb *
dl_create_by_key(
	bool threaded,
	int (*compare_payload_key)(void *, void *),
	void (*free_payload)(void *));

/*
 * destroy a doubly linked list if it is empty, releasing all
 * allocated memory blocks
 */

bool
dl_destroy(
	dlcb *dl
);

/*
 * return the number of nodes in the list.
 */

int
dl_count(
	dlcb *dl
);

/*
 * is the list empty?
 */

bool
dl_empty(
	dlcb *dl
);

/*
 * delete all the nodes on the list, freeing allocated memory.
 */

int
dl_delete_all(
	dlcb *dl
);

/*
 * updating the list or its items. these functions return true on success or
 * false on failure.
 *
 * dl_insert fails if there is already a node on the list with the id
 * or payload key.
 *
 * dl_delete fails if there is no node on the list with the id or payload key.
 *
 * dl_update fails if there is no node on the list with the id or payload key.
 */

bool
dl_insert(
	dlcb *dl,
	long id,
	void *payload
);

bool
dl_delete(
	dlcb *dl,
	long id,
	void *payload
);

bool
dl_update(
	dlcb *dl,
	long id,
	void *payload
);

/*
 * get a node (by key, first, or last), or a preceeding or trailing
 * node, from the list.
 *
 * dl_get fails if there is no node on the list with the id or
 * payload key. dl_get_first and dl_get_last fail if the list is
 * empty.
 *
 * if dl_get succeeds, position state is stored in the dlcb and
 * dl_get_next and dl_get_previous can be used to move forward or
 * backward through the list.
 *
 * dl_get_next and dl_get_previous fail if there are no more next
 * or previous nodes.
 *
 * for dl_get, the id and payload arguments are in-out. you must
 * provide the correct keying information, and if a node was found
 * the id and payload pointers are updated.
 *
 * for dl_get_next and dl_get_previous ignore these values when
 * called but update them on successful return.
 *
 * there is no locking mechanism, but calling dl_insert, dl_delete,
 * or dl_delete_all will clear position state and a subsequent
 * dl_get_next or dl_get_previous return false.
 */

bool
dl_get(
	dlcb *dl,
	long *id,
	void *(*payload)
);

bool
dl_get_first(
	dlcb *dl,
	long *id,
	void *(*payload)
);

bool
dl_get_last(
	dlcb *dl,
	long *id,
	void *(*payload)
);

bool
dl_get_next(
	dlcb *dl,
	long *id,
	void *(*payload)
);

bool
dl_get_previous(
	dlcb *dl,
	long *id,
	void *(*payload)
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* *** end pub *** */

#endif /* TXBDL_SINGLE_HEADER */

#ifdef TXBDL_IMPLEMENTATION
#undef TXBDL_IMPLEMENTATION
/* *** begin priv *** */
/* dl.c -- blametroi's doubly linked list functions -- */

/*
 * a header only implementation of a doubly linked list.
 *
 * the list is kept in order by a key, which can be either an
 * identifying long integer, or by some unique value in the payload
 * that each list node carries.
 *
 * each list will have a control block containing the approriate
 * counters, links, configuration information, and function pointers
 * for routines to compare payload key values and to dynamically free
 * payload storage when a node is freed.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "txbabort.h"

/*
 * a node of the doubly linked list. keying for ordering can use
 * either the id or results from the compare_payload function. node
 * keys must be unique within a list.
 */

#define DLCB_TAG "__DLCB__"
#define DLCB_TAG_LEN 8
#define ASSERT_DLCB(p, m) assert((p) && memcmp((p), DLCB_TAG, DLCB_TAG_LEN) == 0 && (m))
#define ASSERT_DLCB_OR_NULL(p) assert((p) == NULL || memcmp((p), DLCB_TAG, DLCB_TAG_LEN) == 0)

#define DLNODE_TAG "__DLNO__"
#define DLNODE_TAG_LEN 8
#define ASSERT_DLNODE(p, m) assert((p) && memcmp((p), DLNODE_TAG, DLNODE_TAG_LEN) == 0 && (m))
#define ASSERT_DLNODE_OR_NULL(p) assert((p) == NULL || memcmp((p), DLNODE_TAG, DLNODE_TAG_LEN) == 0)

typedef struct dlnode {
	char tag[DLNODE_TAG_LEN];
	dlcb *dlcb;
	struct dlnode *fwd;
	struct dlnode *bwd;
	long id;
	void *payload;
} dlnode;

struct dlcb {
	char tag[DLCB_TAG_LEN];                   /* eye catcher */

	dlnode *head;                  /* head and tail node pointers */
	dlnode *tail;

	dlnode *work;                  /* preallocated node storage, not yet used but
                                   * a potential optimization. */
	dlnode *position;              /* the last node accessed. this is needed by
                                   * get_next and get_previous. operations that
                                   * invalidate the position should set this to
                                   * NULL. */

	void (*payload_free)(void
		*);  /* if a payload needs to be freed, function pointer here */
	int (*payload_compare)(void *,
		void *);  /* key compare for ordering, a la strcmp */

	bool use_id;                   /* use the id field for ordering and finding */
	bool dynamic_payload;          /* the payload should be freed when the node is freed */
	bool threaded;                 /* protect operations with a mutex */

	long odometer;                 /* just a counter of calls to the api */
	long count;                    /* how many items are on the list? */

	pthread_mutex_t mutex;         /* if threaded, block other threads when calling
                                   * atomic code */
};


/*
 * functions prefixed by dl_ are the api for the doubly linked list.
 *
 * functions prefixed by atomic_ are viewed as atomic by this library
 * and can't be logically interrupted by other calls. if threading is
 * enabled, they are run under a mutex lock.
 *
 */

/*
 * compare the id or key of a (potential) new entry to an existing entry
 * on the list.
 *
 * superficially the return looks like that of the various xxxcmp functions,
 * but specific values have meaning, not just negative, zero, or positive.
 *
 * +/-1 for greater than or less than, and +2 for a compare against a null
 * entry.
 */

#define ID_KEY_LT   -1
#define ID_KEY_EQ    0
#define ID_KEY_GT   +1
#define ID_KEY_NULL +2

static
int
atomic_compare_id_or_key(
	dlcb *dl,
	long id,
	void *payload,
	dlnode *existing
) {
	assert((id || payload) &&
		"error missing id or key");

	if (existing == NULL ||
		(dl->use_id && existing->id < 1) ||
		(!dl->use_id && existing->payload == NULL))
		return ID_KEY_NULL;

	long r = 0;
	if (dl->use_id)
		r = id - existing->id;

	else
		r = dl->payload_compare(payload, existing->payload);

	if (r == 0)
		return ID_KEY_EQ;
	if (r < 0)
		return ID_KEY_LT;
	return ID_KEY_GT;
}

/*
 * insert a new node into the list.
 */

static
bool
atomic_insert(
	dlcb *dl,
	long id,
	void *payload
) {
	/* build new list entry */
	dlnode *new = NULL;
	new = malloc(sizeof(dlnode));
	memset(new, 0, sizeof(dlnode));
	memcpy(new->tag, DLNODE_TAG, sizeof(new->tag));
	new->dlcb = dl;
	new->payload = payload;
	if (dl->use_id)
		new->id = id;

	else
		new->id = dl->odometer;

	/* if the list is empty, easy peasy */
	if (dl->head == NULL) {
		dl->head = new;
		dl->tail = new;
		return true;
	}

	/* work the ends of the list first */
	int rf = atomic_compare_id_or_key(dl, new->id, new->payload, dl->head);
	int rl = atomic_compare_id_or_key(dl, new->id, new->payload, dl->tail);

	/* can't have duplicate id/key */
	if (rf == ID_KEY_EQ || rl == ID_KEY_EQ) {
		memset(new, 254, sizeof(dlnode));
		free(new);
		return false;
	}

	/* insert at head or tail if that's the position */
	if (rf == ID_KEY_LT) {
		new->fwd = dl->head;
		dl->head = new;
		new->fwd->bwd = new;
		return true;
	}
	if (rl == ID_KEY_GT) {
		new->bwd = dl->tail;
		dl->tail = new;
		new->bwd->fwd = new;
		return true;
	}

	/* chase the link chain and insert where appropriate */

	dlnode *curr = dl->head->fwd;

	while (curr) {
		int r = atomic_compare_id_or_key(dl, new->id, new->payload, curr);

		/* duplicate key, discard */
		if (r == ID_KEY_EQ) {
			memset(new, 254, sizeof(dlnode));
			free(new);
			return false;
		}

		/* found insertion point yet? */
		if (r == ID_KEY_GT) {
			curr = curr->fwd;
			continue;
		}

		/* insert in front of current */
		new->bwd = curr->bwd;
		new->fwd = curr;
		new->bwd->fwd = new;
		curr->bwd = new;
		return true;
	}

	/* if we fall out of the link chase loop, something is
	 * wrong with the chain, abort. */
	abort_if(true,
		"invalid list chain detected in dl_add");
	return false;
}

/*
 * create a doubly linked list control block, there are mutiple api
 * functions that call this function with some default argument
 * values, hopefully simplifying list creation for clients.
 */

static
dlcb *
atomic_create(
	bool threaded,
	bool use_id,
	void (*payload_free)(void *),
	int (*payload_compare)(void *, void *)
) {
	dlcb *dl = malloc(sizeof(*dl));
	assert(dl &&
		"could not allocate DLCB");
	memset(dl, 0, sizeof(*dl));

	/* the basics */
	memcpy(dl->tag, DLCB_TAG, sizeof(dl->tag));
	dl->odometer = 0;
	dl->count = 0;
	dl->work = NULL;
	dl->position = NULL;
	dl->head = NULL;
	dl->tail = NULL;

	/* threading */
	dl->threaded = threaded;
	if (threaded) {
		assert(pthread_mutex_init(&dl->mutex, NULL) == 0 &&
			"error initializing mutx for DLCB");
	}

	/* is the payload dynamic? */
	dl->dynamic_payload = payload_free != NULL;
	dl->payload_free = payload_free;

	/* if a comparator is provided, use it for ordering, otherwise
	 * use the odometer as an id. */

	dl->use_id = payload_compare == NULL;
	dl->payload_compare = payload_compare;

	return dl;
}

/*
 * report on the list. is it empty? how many nodes does it hold?
 */

static
bool
atomic_empty(
	dlcb *dl
) {
	return dl->head == NULL;
}

/*
 * returns the count of the items on the link list by chasing the
 * link chain.
 */

static
int
atomic_count(
	dlcb *list
) {
	int n = 0;
	dlnode *curr = list->head;
	while (curr) {
		n += 1;
		curr = curr->fwd;
	}
	return n;
}

/*
 * remove and free all of the items linked on the list. this is
 * equivalent to repeatedly calling delete on each item in the list.
 */

static
int
atomic_delete_all(
	dlcb *dl
) {
	dlnode *curr = dl->head;
	dlnode *next = NULL;
	int deleted = 0;
	while (curr) {
		if (dl->dynamic_payload)
			dl->payload_free(curr->payload);
		next = curr->fwd;
		memset(curr, 254, sizeof(dlnode));
		free(curr);
		deleted += 1;
		curr = next;
	}
	dl->head = NULL;
	dl->tail = NULL;
	dl->count = 0;
	return deleted;
}

bool
atomic_get(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	dl->position = NULL;
	if (dl->head == NULL)
		return false;

	/* we need an id or key in payload */
	if (*id < 1 && dl->use_id)
		return false;
	if (*payload == NULL && !dl->use_id)
		return false;

	/* search */
	dlnode *curr = dl->head;
	while (curr) {
		int r = atomic_compare_id_or_key(dl, *id, *payload, curr);
		if (r == ID_KEY_GT) {
			curr = curr->fwd;
			continue;
		} else if (r == ID_KEY_EQ) {
			dl->position = curr;
			*id = curr->id;
			*payload = curr->payload;
			return true;
		}
		break;
	}
	return false;
}

bool
atomic_get_next(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	if (dl->head == NULL)
		return false;
	/* we need an id or key in payload */
	if (*id < 1 && dl->use_id)
		return false;
	if (*payload == NULL && !dl->use_id)
		return false;

	/* we must have a prior position and it must equal our argument key */
	if (dl->position == 0 ||
		atomic_compare_id_or_key(dl, *id, *payload, dl->position) != ID_KEY_EQ)
		return false;

	/* move to next node and set this as the current position in the dl.
	 * if the node exists, update the caller's id and payload. */
	dlnode *curr = dl->position->fwd;
	dl->position = curr;
	if (curr) {
		*id = curr->id;
		*payload = curr->payload;
	}
	return curr != NULL;
}

bool
atomic_get_previous(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	if (dl->head == NULL)
		return false;
	/* we need an id or key in payload */
	if (*id < 1 && dl->use_id)
		return false;
	if (*payload == NULL && !dl->use_id)
		return false;

	/* we must have a prior position and it must equal our argument key */
	if (dl->position == 0 ||
		atomic_compare_id_or_key(dl, *id, *payload, dl->position) != ID_KEY_EQ)
		return false;

	/* move to prior node and set this as the current position in the dl.
	 * if the node exists, update the caller's id and payload. */
	dlnode *curr = dl->position->bwd;
	dl->position = curr;
	if (curr) {
		*id = curr->id;
		*payload = curr->payload;
	}
	return curr != NULL;
}

bool
atomic_get_first(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	dl->position = NULL;
	if (dl->head == NULL)
		return false;
	dl->position = dl->head;
	*id = dl->head->id;
	*payload = dl->head->payload;
	return true;
}

bool
atomic_get_last(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	dl->position = NULL;
	if (dl->tail == NULL)
		return false;
	dl->position = dl->tail;
	*id = dl->tail->id;
	*payload = dl->tail->payload;
	return true;
}

static
bool
atomic_delete(
	dlcb *dl,
	long id,
	void *payload
) {

	/* find where this item is in the list. */

	dlnode *curr = dl->head;
	int r = 0;

	/* TODO optimization by checking last in dlcb */

	while (curr) {

		r = atomic_compare_id_or_key(dl, id, payload, curr);
		if (r == ID_KEY_GT) {
			curr = curr->fwd;
			continue;
		} else if (r == ID_KEY_LT)
			return false;

		/* deletes clear position */
		dl->position = NULL;

		if (curr->fwd == NULL && curr->bwd == NULL) {

			/* this is the only item */
			dl->head = NULL;
			dl->tail = NULL;

		} else if (curr->bwd == NULL) {

			/* this is the head */
			dl->head = curr->fwd;
			curr->fwd->bwd = NULL;

		} else if (curr->fwd == NULL) {

			/* is this the tail? */
			dl->tail = curr->bwd;
			curr->bwd->fwd = NULL;

		} else {

			/* somewhere in the middle */
			curr->bwd->fwd = curr->fwd;
			curr->fwd->bwd = curr->bwd;
		}

		memset(curr, 254, sizeof(dlnode));
		free(curr);
		return true;
	}

	/* it's not there */
	return false;
}

static
bool
atomic_update(
	dlcb *dl,
	long id,
	void *payload
) {

	/* find where this item is in the list. */

	dlnode *curr = dl->head;
	int r = 0;

	/* TODO optimization by checking last in dlcb */

	while (curr) {

		r = atomic_compare_id_or_key(dl, id, payload, curr);
		if (r == ID_KEY_GT) {
			curr = curr->fwd;
			continue;
		} else if (r == ID_KEY_LT)
			return false;

		/* updates clear position */
		dl->position = NULL;

		/* free the old payload. while we know the new payload will have an
		 * equal key, if the key is only part of the payload we allow it
		 * to be updated along witht he rest of the payload. the id can
		 * not be updated at this point. */

		if (dl->dynamic_payload)
			dl->payload_free(curr->payload);

		curr->payload = payload;

		return true;
	}

	/* it's not there */
	return false;
}

/*
 * wrapper functions to default arguments when creating a new linked
 * list depending on keying choice.
 */

dlcb *
dl_create_by_id(
	bool threaded,
	void (*free_payload)(void *)
) {
	return atomic_create(
		threaded,
		true,
		free_payload,
		NULL
		);
}

dlcb *
dl_create_by_key(
	bool threaded,
	int (*compare_payload_key)(void *, void *),
	void (*free_payload)(void *)
) {
	return atomic_create(
		threaded,
		false,
		free_payload,
		compare_payload_key
		);
}

/*
 * if the list is empty, release its storage.
 */

bool
dl_destroy(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl_empty(dl)) {
		if (dl->threaded) {
			while (EBUSY == pthread_mutex_destroy(&dl->mutex))
				;
		}
		if (dl->work) {
			memset(dl->work, 254, sizeof(dlnode));
			free(dl->work);
		}
		memset(dl, 254, sizeof(dlcb));
		free(dl);
		return true;
	}
	return false;
}

bool
dl_empty(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_empty(dl);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

int
dl_count(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	int res = atomic_count(dl);
	int chk = dl->count;
	abort_if(chk != res,
		"error calculated DLCB entry count does not match running count");
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

int
dl_delete_all(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	int res = atomic_delete_all(dl);
	dl->count = 0;
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_insert(
	dlcb *dl,
	long id,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_insert(dl, id, payload);
	if (res)
		dl->count += 1;
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_delete(
	dlcb *dl,
	long id,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	int res = atomic_delete(dl, id, payload);
	if (res)
		dl->count -= 1;
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_update(
	dlcb *dl,
	long id,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	int res = atomic_update(dl, id, payload);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_get(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_get(dl, id, payload);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_get_first(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_get_first(dl, id, payload);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_get_last(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_get_last(dl, id, payload);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_get_next(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_get_next(dl, id, payload);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}

bool
dl_get_previous(
	dlcb *dl,
	long *id,
	void *(*payload)
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_get_previous(dl, id, payload);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}
/* *** end priv *** */

#endif /* TXBDL_IMPLEMENTATION */
