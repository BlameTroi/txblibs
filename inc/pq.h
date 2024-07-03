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
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PQENTRY_TAG "--PQEN--"
typedef struct pqentry_t {
   char tag[8];
   long priority;
   struct pqentry_t *bwd;
   struct pqentry_t *fwd;
   void *payload;
} pqentry_t;

#define PQCB_TAG "--PQCB--"
typedef struct pqcb_t {
   char tag[8];
   pqentry_t *first;
   pqentry_t *last;
   bool threaded;
   pthread_mutex_t mutex;
} pqcb_t;

bool
pq_empty(
   pqcb_t *
);

void
pq_put(
   pqcb_t *,
   long,
   void *);

void *
pq_get(
   pqcb_t *
);

void *
pq_peek(
   pqcb_t *
);

pqcb_t *
pq_create(
   bool
);

bool
pq_destroy(
   pqcb_t *
);

int
pq_count(
   pqcb_t *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
