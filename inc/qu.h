/* pq.h -- blametroi's simple queue -- */

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
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct qucb qucb;

bool
qu_empty(
   qucb *
);

void
qu_enqueue(
   qucb *,
   void *
);

void *
qu_dequeue(
   qucb *
);

void *
qu_peek(
   qucb *
);

qucb *
qu_create(
   void
);

bool
qu_destroy(
   qucb *
);

int
qu_count(
   qucb *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
