/* qu.c -- blametroi's simple queue -- */

/*
 * a header only implementation of a simple queue.
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

#include "../inc/qu.h"

/*
 * transparent control block definitions.
 */

#define QUITEM_TAG "__QUIT__"
#define QUITEM_TAG_LEN 8
#define ASSERT_QUITEM(p, m) assert((p) && memcmp((p), QUITEM_TAG, QUITEM_TAG_LEN) == 0 && (m))
#define ASSERT_QUITEM_OR_NULL(p) assert((p) == NULL || memcmp((p), QUITEM_TAG, QUITEM_TAG_LEN) == 0)

typedef struct quitem {
	char tag[QUITEM_TAG_LEN];
	struct quitem *next;
	void *payload;
} quitem;

#define QUCB_TAG "__QUCB__"
#define QUCB_TAG_LEN 8
#define ASSERT_QUCB(p, m) assert((p) && memcmp((p), QUCB_TAG, QUCB_TAG_LEN) == 0 && (m))
#define ASSERT_QUCB_OR_NULL(p) assert((p) == NULL || memcmp((p), QUCB_TAG, QUCB_TAG_LEN) == 0)

struct qucb {
	char tag[QUCB_TAG_LEN];
	quitem *first;
	quitem *last;
};


/*
 * this is a simple queue implementation.
 *
 * error checking:
 *
 * minimal via assertions. fatal errors such as passing an invalid
 * qucb will fail the assertion. qu_destroy will return a false
 * if it can not complete, but that shouldn't happen. i just haven't
 * worked out the threading around there.
 *
 * brief api overview:
 *
 * all functions except qu_create take a qucb*, the queue control
 * block.
 *
 * the data to manage in the queue is a void*, a pointer to the data
 * or if it will fit in a void*, the data itself.
 *
 * the order of retrieval of items is first in first out.
 *
 * qu_create  -- create a new queue, returns a qucb*
 *
 * qu_empty   -- is the queue empty? returns bool
 *
 * qu_count   -- how many items are in the queue? returns int
 *
 * qu_put     -- add an item to the queue.
 *
 * qu_get     -- remove and return the oldest item in the queue.
 *
 * qu_peek    -- return the oldest item in the queue, but leave
 *               it on the queue.
 *
 * qu_destroy -- if the queue is empty and not in use, release
 *               the qucb. returns true if successful.
 */

/*
 * are there items in the queue?
 */

bool
qu_empty(
	qucb *qu
) {
	ASSERT_QUCB(qu, "invalid QUCB");
	return qu->first == NULL;
}

/*
 * how many items are in the queue?
 */

int
qu_count(
	qucb *qu
) {
	ASSERT_QUCB(qu, "invalid QUCB");

	if (qu->first == NULL)
		return 0;
	if (qu->first == qu->last)
		return 1;

	int i = 0;
	quitem *qe = qu->first;
	while (qe) {
		i += 1;
		qe = qe->next;
	}
	return i;
}

/*
 * create a new queue item.
 */

static
quitem *
qu_new_item(
	void *payload
) {
	quitem *qe = malloc(sizeof(*qe));
	memset(qe, 0, sizeof(*qe));
	memcpy(qe->tag, QUITEM_TAG, sizeof(qe->tag));
	qe->payload = payload;
	qe->next = NULL;
	return qe;
}

/*
 * add an item into the queue.
 */

void
qu_enqueue(
	qucb *qu,
	void *payload
) {
	ASSERT_QUCB(qu, "invalid QUCB");
	quitem *new_qe = qu_new_item(payload);
	if (qu->first == NULL) {
		qu->first = new_qe;
		qu->last = new_qe;
		return;
	}
	qu->last->next = new_qe;
	qu->last = new_qe;
	return;
}

/*
 * remove and return the first item from the queue.
 */

void *
qu_dequeue(
	qucb *qu
) {
	ASSERT_QUCB(qu, "invalid QUCB");
	if (qu->first == NULL)
		return NULL;
	quitem *qe = qu->first;
	qu->first = qe->next;
	void *res = qe->payload;
	free(qe);
	return res;
}

/*
 * return the first item from the queue but do not remove it.
 */

void *
qu_peek(
	qucb *qu
) {
	ASSERT_QUCB(qu, "invalid QUCB");
	if (qu->first == NULL)
		return NULL;
	return qu->first->payload;
}

/*
 * create a new queue.
 */

qucb *
qu_create(
	void
) {
	qucb *qu = malloc(sizeof(*qu));
	assert(qu && "could not allocate QUCB");
	memset(qu, 0, sizeof(*qu));
	memcpy(qu->tag, QUCB_TAG, sizeof(qu->tag));
	qu->first = NULL;
	qu->last = NULL;
	return qu;
}

/*
 * free the queue control block if the queue is empty.
 */

bool
qu_destroy(
	qucb *qu
) {
	ASSERT_QUCB(qu, "invalid QUCB");
	if (qu->first == NULL) {
		memset(qu, 253, sizeof(*qu));
		free(qu);
		return true;
	}
	return false;
}
