/* pq.h -- blametroi's simple priority queue -- */

/*
 * a header only implementation of a priority queue.
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

/*
 * pqcb
 *
 * the priority queue control block.
 */

typedef struct pqcb pqcb;

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
	pqcb *,
	long,
	void *
);

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
	pqcb *,
	long *,
	void **
);

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
	pqcb *,
	long *,
	void **
);

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
	pqcb *,
	long *,
	void **
);

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
	pqcb *,
	long *,
	void **
);

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
);

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
	pqcb *
);

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
	pqcb *
);

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
	pqcb *
);

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
	pqcb *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* pq.h ends here */
