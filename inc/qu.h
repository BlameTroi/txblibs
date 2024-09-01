/* qu.h -- blametroi's simple queue -- */

/*
 * a header only implementation of a queue.
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
 * an instance of a queue.
 */

typedef struct qucb qucb;

/*
 * is the queue empty?
 */

bool
qu_empty(
	qucb *
);

/*
 * add an item to the end of the queue.
 */

void
qu_enqueue(
	qucb *,
	void *
);

/*
 * remove an item from the front of the queue.
 */

void *
qu_dequeue(
	qucb *
);

/*
 * return the item from the front of the queue without removing it.
 */

void *
qu_peek(
	qucb *
);

/*
 * create a new queue instance.
 */

qucb *
qu_create(
	void
);

/*
 * if the queue is empty, release its resources.
 */

bool
qu_destroy(
	qucb *
);

/*
 * how many items are on the queue?
 */

int
qu_count(
	qucb *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
