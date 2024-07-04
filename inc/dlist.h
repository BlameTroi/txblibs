/* listd.h -- blametroi's doubly linked list functions -- */

/*
 * a header only implementation of a doubly linked list.
 *
 * the list is kept in order by a key, which can be either an
 * identifying long integer, or by some unique value in the payload
 * that each list node carries.
 *
 * each list will have a control block containing the approriate
 * counters, links, configuration information, and when function
 * pointers for routines to compare payload key values and to
 * dynamically free payload storage when a node is freed.
 *
 * the api is reasonably complete, but i'm still leaving the setup for
 * the list control blocks in open user code.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <pthread.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * a node of the doubly linked list. keying for ordering can use
 * either the id or results from the compare_payload function. node
 * keys must be unique within a list.
 */

#define LDITEM_TAG "--LDIT--"
typedef struct lditem_t {
   char tag[8];
   long id;                        /* either a user supplied id or an odometer value */
   void *payload;                  /* if provided, usually a pointer to the payload */
   struct ldcb_t *list;   /* owning list */
   struct lditem_t *fwd;      /* and chain pointers */
   struct lditem_t *bwd;
} lditem_t;

/*
 * double list control block holding chain pointers and other
 * controling information. if a payload is carried, function pointers
 * to free the payload and also perform comparisons for ordering are
 * put here. an id field is provided for ordering and is set from an
 * odometer value.
 *
 * all functions take as their first argument a pointer to an instance
 * of this control block.
 */

#define LDCB_TAG "--LDCB--"
typedef struct ldcb_t {
   char tag[8];
   lditem_t *first;               /* head and tail item pointers */
   lditem_t *last;
   void (*free_payload)(void *);  /* if a payload is carried and if it needs to be freed, place address here */
   long (*compare_payload)(void *, void *);  /* if a payload is carried and used for keying, a compare function here */
   long odometer;                 /* just a counter of calls to the api */
   long count;                    /* how many items are on the list? */
   bool use_id;                   /* use the id field for ordering and finding */
   bool has_payload;              /* this list's items carry a payload */
   bool dynamic_payload;          /* and the payload should be freed when the item is freed */
   bool threaded;
   pthread_mutex_t mutex;
} ldcb_t;

/*
 * forward declarations for all functions.
 *
 * i hope that argument names are sufficiently wordy to aid in
 * understanding. more complete comment blocks are included with
 * the implementations.
 *
 * error checking and handling is limited. if things look bad,
 * execution is halted via an assert(). this is only done in cases
 * where continued execution is unwise, as in the list control block
 * is not initialized, or it appears that the link chain is broken.
 */

ldcb_t *
ld_create(
   bool threaded,
   bool use_id,
   bool has_payload,
   void (*free_payload)(void *),
   void (*compare_payload)(void *, void *)
);

bool
ld_reset(
   ldcb_t *list
);

void
ld_free_all(
   ldcb_t *list
);


lditem_t *
ld_new(
   ldcb_t *list,
   void *id_or_payload_pointer
);

bool
ld_free(
   ldcb_t *list,
   lditem_t **address_of_item_pointer
);

lditem_t *
ld_find(
   ldcb_t *list,
   void *id_or_payload_pointer
);

int
ld_count(
   ldcb_t *list
);

bool
ld_add(
   ldcb_t *list,
   lditem_t *unlinked_item
);

lditem_t *
ld_remove(
   ldcb_t *list,
   void *id_or_payload_pointer
);

lditem_t *
ld_next(
   ldcb_t *list,
   lditem_t *(*next_item)
);

lditem_t *
ld_prev(
   ldcb_t *list,
   lditem_t *(*prior_item)
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
