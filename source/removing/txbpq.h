/* txbpq.h -- blametroi's priority queue library -- */

/*
 * a header only implementation of a priority queue.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TXBPQ_H
#define TXBPQ_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * pqcb
 *
 * the priority queue control block.
 */

typedef struct pqcb hpq;

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
	hpq *,
	long,
	ppayload
);

/*
 * pq_get_highest
 *
 * remove and return the highest priority item from the queue.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: boolean was there an item
 */

bool
pq_get_highest(
	hpq *,
	long *,
	ppayload*
);

/*
 * pq_get_lowest
 *
 * remove and return the lowest priority item from the queue.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: boolean was there an item
 */

bool
pq_get_lowest(
	hpq *,
	long *,
	ppayload*
);

/*
 * pq_peek_highest
 *
 * return but do not remove the highest priority item from the queue.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: boolean was there an item
 */

bool
pq_peek_highest(
	hpq *,
	long *,
	ppayload*
);

/*
 * pq_peek_lowest
 *
 * return but do not remove the lowest priority item from the queue.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: boolean was there an item
 */

bool
pq_peek_lowest(
	hpq *,
	long *,
	ppayload*
);

/*
 * pq_create
 *
 * create a new priority queue.
 *
 * return: the new empty queue instance
 */

hpq *
pq_create(
	void
);

/*
 * pq_reset
 *
 * remove every item from the queue. pq item storage is freed
 * but payloads are the responsibility of the client.
 *
 *     in: the pq instance
 *
 * return: integer number of items removed from the queue.
 */

int
pq_reset(
	hpq *
);

/*
 * pq_destroy
 *
 * free all pq storage if the pq is empty.
 *
 *     in: the pq instance
 *
 * return: boolean was the queue destroyed
 */

bool
pq_destroy(
	hpq *
);

/*
 * pq_count
 *
 * how many items are in the queue?
 *
 *     in: the pq instance
 *
 * return: integer number of items
 */

int
pq_count(
	hpq *
);

/*
 * pq_empty
 *
 * are there items in the queue?
 *
 *     in: the pq instance
 *
 * return: boolean
 */

bool
pq_empty(
	hpq *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBPQ_H */

#ifdef TXBPQ_IMPLEMENTATION
#undef TXBPQ_IMPLEMENTATION

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "txbabort.h"

/*
 * transparent control block definitions.
 */

#define PQITEM_TAG "__PQIT__"
#define PQITEM_TAG_LEN 8

#define ASSERT_PQITEM(p, m) \
	abort_if(!(p) || memcmp((p), PQITEM_TAG, PQITEM_TAG_LEN) != 0, (m));

#define ASSERT_PQITEM_OR_NULL(p, m) \
	abort_if(p && memcmp((p), PQITEM_TAG, PQITEM_TAG_LEN) != 0, (m));

typedef struct pqitem pqitem;

struct pqitem {
	char tag[PQITEM_TAG_LEN];
	long priority;
	pqitem *bwd;
	pqitem *fwd;
	ppayload payload;
};

#define PQCB_TAG "__PQCB__"
#define PQCB_TAG_LEN 8

#define ASSERT_PQCB(p, m) \
	abort_if(!(p) || memcmp((p), PQCB_TAG, PQCB_TAG_LEN) != 0, (m));

#define ASSERT_PQCB_OR_NULL(p, m) \
	abort_if(p && memcmp((p), PQCB_TAG, PQCB_TAG_LEN) != 0, (m));

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
 * all functions except pq_create take a pqcb, the priority queue
 * control block.
 *
 * priorities are longs, and the data to manage in the queue is a
 * ppayload, usually a pointer to the client data.
 *
 * the order of retrieval of items of the same priority is neither
 * specified nor guaranteed to be consistent across different versions
 * of this library.
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
 * pq_peek_*  -- return but do not remove the item in the queue with
 *               the _highest or _lowest priority from the queue.
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
 * return: boolean true if empty
 */

bool
pq_empty(
	hpq *pq
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
 * return: integer number of items
 */

int
pq_count(
	hpq *pq
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
 * create a new queue item packaging the priority and payload.
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
	ppayload payload
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
	hpq *pq,
	long priority,
	ppayload payload
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
	abort_if(true,
		"pq_insert error in priority queue chaining");
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
 * return: boolean was there an item
 */

bool
pq_get_highest(
	hpq *pq,
	long *priority,
	ppayload *payload
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
 * return: boolean was there an item
 */

bool
pq_get_lowest(
	hpq *pq,
	long *priority,
	ppayload *payload
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
 * return but do not remove the highest priority item from the queue.
 *
 *     in: the pq instance
 *
 *    out: long priority
 *
 *    out: payload
 *
 * return: boolean was there an item
 */

bool
pq_peek_highest(
	hpq *pq,
	long *priority,
	ppayload *payload
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
 * return but do not remove the lowest priority item from the queue.
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
	hpq *pq,
	long *priority,
	ppayload *payload
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

hpq *
pq_create(
	void
) {
	hpq *pq = malloc(sizeof(*pq));
	abort_if(!pq,
		"pq_create could not allocate PQCB");
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
 * return: integer number of items removed from the queue.
 */

int
pq_reset(
	hpq *pq
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
 * return: boolean was the queue destroyed
 */

bool
pq_destroy(
	hpq *pq
) {
	ASSERT_PQCB(pq, "invalid PQCB");
	if (pq->first != NULL)
		return false;
	memset(pq, 253, sizeof(*pq));
	free(pq);
	return true;
}
#endif /* TXBPQ_IMPLEMENTATION */
/* txbpq.h ends here */
