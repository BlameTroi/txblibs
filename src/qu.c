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

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/qu.h"

/*
 * transparent control block definitions.
 */
#define QUENTRY_TAG "__QUEN__"
typedef struct quentry {
	char tag[8];
	struct quentry *next;
	void *payload;
} quentry;

#define QUCB_TAG "__QUCB__"
struct qucb {
	char tag[8];
	quentry *first;
	quentry *last;
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
 * are there entries in the queue?
 */

bool
qu_empty(
	qucb *qu
) {
	assert(qu &&
		memcmp(qu->tag, QUCB_TAG, sizeof(qu->tag)) == 0 &&
		"invalid QUCB");
	return qu->first == NULL;
}

/*
 * how many entries are in the queue?
 */

int
qu_count(
	qucb *qu
) {
	assert(qu &&
		memcmp(qu->tag, QUCB_TAG, sizeof(qu->tag)) == 0 &&
		"invalid QUCB");

	if (qu->first == NULL)
		return 0;
	if (qu->first == qu->last)
		return 1;

	int i = 0;
	quentry *qe = qu->first;
	while (qe) {
		i += 1;
		qe = qe->next;
	}
	return i;
}

/*
 * create a new queue entry.
 */

static
quentry *
qu_new_entry(
	void *payload
) {
	quentry *qe = malloc(sizeof(*qe));
	memset(qe, 0, sizeof(*qe));
	memcpy(qe->tag, QUENTRY_TAG, sizeof(qe->tag));
	qe->payload = payload;
	qe->next = NULL;
	return qe;
}

/*
 * add an entry into the queue.
 */

void
qu_enqueue(
	qucb *qu,
	void *payload
) {
	assert(qu &&
		memcmp(qu->tag, QUCB_TAG, sizeof(qu->tag)) == 0 &&
		"invalid QUCB");
	quentry *new_qe = qu_new_entry(payload);
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
	assert(qu &&
		memcmp(qu->tag, QUCB_TAG, sizeof(qu->tag)) == 0 &&
		"invalid QUCB");
	if (qu->first == NULL)
		return NULL;
	quentry *qe = qu->first;
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
	assert(qu &&
		memcmp(qu->tag, QUCB_TAG, sizeof(qu->tag)) == 0 &&
		"invalid QUCB");
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
	assert(qu &&
		memcmp(qu->tag, QUCB_TAG, sizeof(qu->tag)) == 0 &&
		"invalid QUCB");
	if (qu->first == NULL) {
		memset(qu, 253, sizeof(*qu));
		free(qu);
		return true;
	}
	return false;
}
