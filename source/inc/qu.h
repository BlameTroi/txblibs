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
 * qu_empty
 *
 * are there items in the queue?
 *
 *     in: the qu instance
 *
 * return: boolean, true if empty
 */

bool
qu_empty(
	qucb *
);

/*
 * qu_enqueue
 *
 * add an item to the queue.
 *
 *     in: the qu instance
 *
 *     in: void * client payload
 *
 * return: nothing.
 */

void
qu_enqueue(
	qucb *,
	void *
);

/*
 * qu_dequeue
 *
 * remove and return the first (oldest) item on the queue.
 *
 *     in: the qu instance
 *
 * return: void * client payload or NULL if queue is empty.
 */

void *
qu_dequeue(
	qucb *
);

/*
 * qu_peek
 *
 * return the first (oldet) item on the queue but leave
 * it on the queue.
 *
 *     in: the qu instance
 *
 * return: void * client payload or NULL if queue is empty.
 */

void *
qu_peek(
	qucb *
);

/*
 * qu_create
 *
 * create a new queue.
 *
 *     in: nothing
 *
 * return: the new qu instance.
 */

qucb *
qu_create(
	void
);

/*
 * qu_destroy
 *
 * free the queue control block if the queue is empty.
 *
 *     in: the qu instance
 *
 * return: boolean, true if successful, false if queue is not empty
 */

bool
qu_destroy(
	qucb *
);

/*
 * qu_reset
 *
 * remove all items from the queue.
 *
 *     in: the qu instance
 *
 * return: int number of items removed
 */

int
qu_reset(
	qucb *
);

/*
 * qu_count
 *
 * how many items are in the queue?
 *
 *     in: the qu instance.
 *
 * return: int, number of items.
 */

int
qu_count(
	qucb *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* qu.h ends here */
