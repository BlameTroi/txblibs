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

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/pq.h"

/*
 * transparent control block definitions.
 */

#define PQENTRY_TAG "--PQEN--"
typedef struct pqentry {
   char tag[8];
   long priority;
   struct pqentry *bwd;
   struct pqentry *fwd;
   void *payload;
} pqentry;

#define PQCB_TAG "--PQCB--"
struct pqcb {
   char tag[8];
   pqentry *first;
   pqentry *last;
   bool threaded;
   pthread_mutex_t mutex;
};


/*
 * this is a simple priority queue implementation with some thread
 * safety via pthread mutex.
 *
 * thread safety:
 *
 * functions that must be thread safe are implemented in pairs. a
 * prim_pq_x function that does the actual work, and an externally
 * visible pq_x function that brackets a call to prim_pq_x with a
 * mutex lock and unlock.
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
 * pq_put     -- add an item at the requested priority.
 *
 * pq_get     -- remove and return the last item in the queue,
 *               the one with the highest priority. returns a
 *               void *
 *
 * pq_peek    -- return the last item in the queue, but leave
 *               it on the queue.
 *
 * pq_destroy -- if the queue is empty and not in use, release
 *               the pqcb. returns true if successful.
 */

/*
 * are there entries in the queue?
 */

static
bool
prim_pq_empty(
   pqcb *pq
) {
   return pq->first == NULL;
}

bool
pq_empty(
   pqcb *pq
) {
   assert(pq &&
          memcmp(pq->tag, PQCB_TAG, sizeof(pq->tag)) == 0 &&
          "invalid PQCB");
   if (pq->threaded) {
      pthread_mutex_lock(&pq->mutex);
   }
   bool ret = prim_pq_empty(pq);
   if (pq->threaded) {
      pthread_mutex_unlock(&pq->mutex);
   }
   return ret;
}

/*
 * how many entries are in the queue?
 */

static
int
prim_pq_count(
   pqcb *pq
) {
   int i = 0;
   pqentry *qe = pq->first;
   while (qe) {
      i += 1;
      qe = qe->fwd;
   }
   return i;
}

int
pq_count(
   pqcb *pq
) {
   assert(pq &&
          memcmp(pq->tag, PQCB_TAG, sizeof(pq->tag)) == 0 &&
          "invalid PQCB");
   if (pq->threaded) {
      pthread_mutex_lock(&pq->mutex);
   }
   int ret = prim_pq_count(pq);
   if (pq->threaded) {
      pthread_mutex_unlock(&pq->mutex);
   }
   return ret;
}

/*
 * create a new queue entry.
 */

static
pqentry *
pq_new_entry(
   long priority,
   void *payload
) {
   pqentry *qe = malloc(sizeof(*qe));
   memset(qe, 0, sizeof(*qe));
   memcpy(qe->tag, PQENTRY_TAG, sizeof(qe->tag));
   qe->priority = priority;
   qe->payload = payload;
   qe->fwd = NULL;
   qe->bwd = NULL;
   return qe;
}

/*
 * add an entry into the queue with the specified priority.
 */

static
void
prim_pq_put(
   pqcb *pq,
   long priority,
   void *payload
) {
   pqentry *qe = pq_new_entry(priority, payload);

   /* empty is easy.  */
   if (prim_pq_empty(pq)) {
      pq->first = qe;
      pq->last = qe;
      return;
   }

   /* if the priority puts it at either end of the list,
    * it's still easy. ordering within priority is not
    * guaranteed. */
   if (qe->priority <= pq->first->priority) {
      qe->fwd = pq->first;
      qe->fwd->bwd = qe;
      pq->first = qe;
      return;
   } else if (qe->priority > pq->last->priority) {
      qe->bwd = pq->last;
      qe->bwd->fwd = qe;
      pq->last = qe;
      return;
   }

   /* find an insertion point. */
   pqentry *p = pq->first;
   while (p) {
      if (p->priority < qe->priority) {
         p = p->fwd;
         continue;
      }
      qe->bwd = p->bwd;
      p->bwd = qe;
      qe->bwd->fwd = qe;
      qe->fwd = p;
      return;
   }

   /* if we get here, the queue is broken. */
   assert(NULL && "error in priority queue chaining");
}

void
pq_put(
   pqcb *pq,
   long priority,
   void *payload
) {
   assert(pq &&
          memcmp(pq->tag, PQCB_TAG, sizeof(pq->tag)) == 0 &&
          "invalid PQCB");
   if (pq->threaded) {
      pthread_mutex_lock(&pq->mutex);
   }
   prim_pq_put(pq, priority, payload);
   if (pq->threaded) {
      pthread_mutex_unlock(&pq->mutex);
   }
}

/*
 * remove and return the top item from the queue.
 */

static
void *
prim_pq_get(
   pqcb *pq
) {
   if (prim_pq_empty(pq)) {
      return NULL;
   }
   pqentry *qe = pq->last;
   void *payload = qe->payload;
   pq->last = qe->bwd;
   free(qe);
   if (pq->last == NULL) {
      pq->first = NULL;
   } else {
      pq->last->fwd = NULL;
   }
   return payload;
}

void *
pq_get(
   pqcb *pq
) {
   assert(pq &&
          memcmp(pq->tag, PQCB_TAG, sizeof(pq->tag)) == 0 &&
          "invalid PQCB");
   if (pq->threaded) {
      pthread_mutex_lock(&pq->mutex);
   }
   void *res = prim_pq_get(pq);
   if (pq->threaded) {
      pthread_mutex_unlock(&pq->mutex);
   }
   return res;
}

/*
 * return the top of the queue but do not remove it.
 */

void *
prim_pq_peek(
   pqcb *pq
) {
   if (prim_pq_empty(pq)) {
      return NULL;
   }
   return pq->last->payload;
}

void *
pq_peek(
   pqcb *pq
) {
   assert(pq &&
          memcmp(pq->tag, PQCB_TAG, sizeof(pq->tag)) == 0 &&
          "invalid PQCB");
   void *res = NULL;
   if (pq->threaded) {
      pthread_mutex_lock(&pq->mutex);
   }
   res = prim_pq_peek(pq);
   if (pq->threaded) {
      pthread_mutex_unlock(&pq->mutex);
   }
   return res;
}

/*
 * create a new priority queue.
 */

pqcb *
pq_create(
   bool threaded
) {
   pqcb *pq = malloc(sizeof(*pq));
   assert(pq && "could not allocate PQCB");
   memset(pq, 0, sizeof(*pq));
   memcpy(pq->tag, PQCB_TAG, sizeof(pq->tag));
   pq->first = NULL;
   pq->last = NULL;
   pq->threaded = threaded;
   if (threaded) {
      assert(pthread_mutex_init(&pq->mutex, NULL) == 0 &&
             "error initializing mutx for PQCB");
   }
   return pq;
}

/*
 * free the priority queue control block if the queue is empty.
 *
 * TODO i'm not thrilled with the threading around this part of
 * things. do i need a second resource or some sort of critical
 * section?
 */

bool
pq_destroy(
   pqcb *pq
) {
   assert(pq &&
          memcmp(pq->tag, PQCB_TAG, sizeof(pq->tag)) == 0 &&
          "invalid PQCB");
   if (pq_empty(pq)) {
      if (pq->threaded) {
         while (EBUSY == pthread_mutex_destroy(&pq->mutex))
            ;
      }
      memset(pq, 255, sizeof(*pq));
      free(pq);
      return true;
   }
   return false;
}
