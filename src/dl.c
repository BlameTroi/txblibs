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
#include "../inc/dl.h"

/*
 * a node of the doubly linked list. keying for ordering can use
 * either the id or results from the compare_payload function. node
 * keys must be unique within a list.
 */

#define DLNODE_TAG "--DLNO--"
typedef struct dlnode {
	char tag[8];
	dlcb *dlcb;
	struct dlnode *fwd;
	struct dlnode *bwd;
	long id;
	void *payload;
} dlnode;

#define DLCB_TAG "--DLCB--"
struct dlcb {
	char tag[8];                   /* eye catcher */

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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
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
	assert(dl &&
		memcmp(dl->tag, DLCB_TAG, sizeof(dl->tag)) == 0 &&
		"invalid DLCB");
	if (dl->threaded)
		pthread_mutex_lock(&dl->mutex);
	dl->odometer += 1;
	bool res = atomic_get_previous(dl, id, payload);
	if (dl->threaded)
		pthread_mutex_unlock(&dl->mutex);
	return res;
}
