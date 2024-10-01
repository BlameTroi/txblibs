/* pq.c -- blametroi's simple priority queue -- */

/*
 * a header only implementation of a simple priority queue.
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

#include "../inc/pq.h"

/*
 * transparent control block definitions.
 */
#define PQITEM_TAG "__PQIT__"
#define PQITEM_TAG_LEN 8
#define ASSERT_PQITEM(p, m) assert((p) && memcmp((p), PQITEM_TAG, PQITEM_TAG_LEN) == 0 && (m))
#define ASSERT_PQITEM_OR_NULL(p) assert((p) == NULL || memcmp((p), PQITEM_TAG, PQITEM_TAG_LEN) == 0)

typedef struct pqitem pqitem;

struct pqitem {
	char tag[PQITEM_TAG_LEN];
	long priority;
	pqitem *bwd;
	pqitem *fwd;
	void *payload;
};

#define PQCB_TAG "__PQCB__"
#define PQCB_TAG_LEN 8
#define ASSERT_PQCB(p, m) assert((p) && memcmp((p), PQCB_TAG, PQCB_TAG_LEN) == 0 && (m))
#define ASSERT_PQCB_OR_NULL(p) assert((p) == NULL || memcmp((p), PQCB_TAG, PQCB_TAG_LEN) == 0)

struct pqcb {
	char tag[PQCB_TAG_LEN];
	pqitem *first;
	pqitem *last;
};

/*
 * this is a simple priority queue implementation.
 *
 * error checking:
 *
 * minimal via assertions. fatal errors such as passing an invalid
 * pqcb will fail the assertion. pq_destroy will return a false
 * if it can not complete, but that shouldn't happen. i just haven't
 * worked out the threading around there.
 *
 * brief api overview:
 *
 * all functions except pq_create take a pqcb_t*, the priority queue
 * control block.
 *
 * priorities are longs, and the data to manage in the queue is a
 * void*, a pointer to the data or if it will fit in a void*, the data
 * itself.
 *
 * the order of retrieval of items of the same priority is not
 * specified.
 *
 * pq_create  -- create a new queue, returns a pqcb_t*
 *
 * pq_empty   -- is the queue empty? returns bool
 *
 * pq_count   -- how many items are in the queue? returns int
 *
 * pq_insert  -- add an item at the requested priority.
 *
 * pq_get_*   -- remove and return the item with the _highest
 *               or _lowest priority from the queue.
 *
 * pq_peek_*  -- return the item in the queue with the _highest
 *               or _lowest priority from the queue.
 *
 * pq_destroy -- if the queue is empty and not in use, release
 *               the pqcb. returns true if successful.
 *
 * pq_reset   -- delete all items from the queue.
 */

/*
 * pq_empty
 *
 * are there items in the queue?
 *
 *     in: the pq instance
 *
 * return: bool
 */

bool
pq_empty(
	pqcb *pq
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	return pq->first == NULL;
}

/*
 * pq_count
 *
 * how many items are in the queue?
 *
 *     in: the pq instance
 *
 * return: int number of items
 */

int
pq_count(
	pqcb *pq
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	int i = 0;
	pqitem *qi = pq->first;
	while (qi) {
		i += 1;
		qi = qi->fwd;
	}
	return i;
}

/*
 * pq_create_item
 *
 * create a new queue item wrapping priority and payload
 * to store on the queue.
 *
 *     in: long priority
 *
 *     in: payload
 *
 * return: the item
 */

static
pqitem *
pq_create_item(
	long priority,
	void *payload
) {
	pqitem *qi = malloc(sizeof(*qi));
	memset(qi, 0, sizeof(*qi));
	memcpy(qi->tag, PQITEM_TAG, sizeof(qi->tag));
	qi->priority = priority;
	qi->payload = payload;
	qi->fwd = NULL;
	qi->bwd = NULL;
	return qi;
}

/*
 * pq_insert
 *
 * add an item to the queue with the specified priority.
 *
 *     in: the pq instance
 *
 *     in: long priority
 *
 *     in: payload
 *
 * return: nothing
 */

void
pq_insert(
	pqcb *pq,
	long priority,
	void *payload
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	pqitem *qi = pq_create_item(priority, payload);

	/* empty is easy.  */
	if (pq->first == NULL) {
		pq->first = qi;
		pq->last = qi;
		return;
	}

	/* if the priority puts it at either end of the list,
	 * it's still easy. ordering within priority is not
	 * guaranteed. */
	if (qi->priority <= pq->first->priority) {
		qi->fwd = pq->first;
		qi->fwd->bwd = qi;
		pq->first = qi;
		return;
	} else if (qi->priority > pq->last->priority) {
		qi->bwd = pq->last;
		qi->bwd->fwd = qi;
		pq->last = qi;
		return;
	}

	/* find an insertion point. */
	pqitem *p = pq->first;
	while (p) {
		if (p->priority < qi->priority) {
			p = p->fwd;
			continue;
		}
		qi->bwd = p->bwd;
		p->bwd = qi;
		qi->bwd->fwd = qi;
		qi->fwd = p;
		return;
	}

	/* if we get here, the queue is broken. */
	assert(NULL &&
		"error in priority queue chaining");
}

/*
 * pq_get_highest
 *
 * remove and return the highest priority item from the
 * queue.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: bool was there an item
 */

bool
pq_get_highest(
	pqcb *pq,
	long *priority,
	void **payload
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	if (pq->first == NULL)
		return false;
	pqitem *qi = pq->last;
	*priority = qi->priority;
	*payload = qi->payload;
	pq->last = qi->bwd;
	memset(qi, 253, sizeof(*qi));
	free(qi);
	if (pq->last == NULL)
		pq->first = NULL;
	else
		pq->last->fwd = NULL;
	return true;
}

/*
 * pq_get_lowest
 *
 * remove and return the lowest priority item from the
 * queue.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: bool was there an item
 */

bool
pq_get_lowest(
	pqcb *pq,
	long *priority,
	void **payload
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	if (pq->first == NULL)
		return false;
	pqitem *qi = pq->first;
	*priority = qi->priority;
	*payload = qi->payload;
	pq->first = qi->fwd;
	memset(qi, 253, sizeof(*qi));
	free(qi);
	if (pq->last == NULL)
		pq->first = NULL;
	else
		pq->last->fwd = NULL;
	return true;
}

/*
 * pq_peek_highest
 *
 * return the highest priority item from the queue while leaving the
 * item in place.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: bool was there an item
 */

bool
pq_peek_highest(
	pqcb *pq,
	long *priority,
	void **payload
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	if (pq->last == NULL)
		return false;
	*priority = pq->last->priority;
	*payload = pq->last->payload;
	return true;
}

/*
 * pq_peek_lowest
 *
 * remove and return the lowest priority item from the queue while
 * leaving the item in place.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: bool was there an item
 */

bool
pq_peek_lowest(
	pqcb *pq,
	long *priority,
	void **payload
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	if (pq->first == NULL)
		return false;
	*priority = pq->first->priority;
	*payload = pq->first->payload;
	return true;
}

/*
 * pq_create
 *
 * create a new priority queue.
 *
 * return: the new empty queue instance
 */

pqcb *
pq_create(
	void
) {
	pqcb *pq = malloc(sizeof(*pq));
	assert(pq &&
		"could not allocate PQCB");
	memset(pq, 0, sizeof(*pq));
	memcpy(pq->tag, PQCB_TAG, sizeof(pq->tag));
	pq->first = NULL;
	pq->last = NULL;
	return pq;
}

/*
 * pq_reset
 *
 * remove every item from the queue. pq item storage is freed
 * but payloads are the responsibility of the client.
 *
 *     in: the pq instance
 *
 * return: int number of items removed from the queue.
 */

int
pq_reset(
	pqcb *pq
) {
	int i = 0;
	pqitem *qi = NULL;
	while (qi = pq->first, qi) {
		i += 1;
		pq->first = qi->fwd;
		memset(qi, 253, sizeof(*qi));
		free(qi);
	}
	return i;
}

/*
 * pq_destroy
 *
 * free all pq storage if the pq is empty.
 *
 *     in: the pq instance
 *
 * return: bool was the queue destroyed
 */

bool
pq_destroy(
	pqcb *pq
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	if (pq->first != NULL)
		return false;
	memset(pq, 253, sizeof(*pq));
	free(pq);
	return true;
}
/* pq.c ends here */
