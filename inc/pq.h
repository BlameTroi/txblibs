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

typedef struct pqcb pqcb;

bool
pq_empty(
	pqcb *
);

void
pq_put(
	pqcb *,
	long,
	void *
);

void *
pq_get(
	pqcb *
);

void *
pq_peek(
	pqcb *
);

pqcb *
pq_create(
	bool
);

bool
pq_destroy(
	pqcb *
);

int
pq_count(
	pqcb *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
