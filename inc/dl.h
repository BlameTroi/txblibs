/* dl.h -- blametroi's doubly linked list functions -- */

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
 * double list control block holding chain pointers and other control
 * information. if a payload is carried, function pointers to free the
 * payload and also perform comparisons for ordering are put here. an
 * id field is provided for ordering and is set from an odometer
 * value.
 *
 * all functions other than list creation take as their first argument
 * a pointer to an instance of this control block.
 */

typedef struct dlcb dlcb;

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

/*
 * create a new doubly linked list instance. nodes in the list are
 * uniquely identified by either an id field (a positive long integer)
 * or by some part of the payload that can be compared with other
 * payloads using a helper function.
 *
 * if a payload key is used, the id argument can be any value and is
 * ignored. internally it is set to an odometer value.
 *
 * if an id is used, it must be a long greater than zero. when
 * inserting a node, an id of zero indicates that an id should be
 * assigned automatically.
 *
 * if successful a doubly linked list control block (dlcb) pointer
 * is returned.
 */

dlcb *
dl_create_by_id(
   bool threaded,
   void (*free_payload)(void *)
);

dlcb *
dl_create_by_key(
   bool threaded,
   int (*compare_payload_key)(void *, void *),
   void (*free_payload)(void *));

/*
 * destroy a doubly linked list if it is empty, releasing all
 * allocated memory blocks
 */

bool
dl_destroy(
   dlcb *dl
);

/*
 * return the number of nodes in the list.
 */

int
dl_count(
   dlcb *dl
);

/*
 * is the list empty?
 */

bool
dl_empty(
   dlcb *dl
);

/*
 * delete all the nodes on the list, freeing allocated memory.
 */

int
dl_delete_all(
   dlcb *dl
);

/*
 * updating the list or its items. these functions return true on success or
 * false on failure.
 *
 * dl_insert fails if there is already a node on the list with the id
 * or payload key.
 *
 * dl_delete fails if there is no node on the list with the id or payload key.
 *
 * dl_update fails if there is no node on the list with the id or payload key.
 */

bool
dl_insert(
   dlcb *dl,
   long id,
   void *payload
);

bool
dl_delete(
   dlcb *dl,
   long id,
   void *payload
);

bool
dl_update(
   dlcb *dl,
   long id,
   void *payload
);

/*
 * get a node (by key, first, or last), or a preceeding or trailing
 * node, from the list.
 *
 * dl_get fails if there is no node on the list with the id or
 * payload key. dl_get_first and dl_get_last fail if the list is
 * empty.
 *
 * if dl_get succeeds, position state is stored in the dlcb and
 * dl_get_next and dl_get_previous can be used to move forward or
 * backward through the list.
 *
 * dl_get_next and dl_get_previous fail if there are no more next
 * or previous nodes.
 *
 * for dl_get, the id and payload arguments are in-out. you must
 * provide the correct keying information, and if a node was found
 * the id and payload pointers are updated.
 *
 * for dl_get_next and dl_get_previous ignore these values when
 * called but update them on successful return.
 *
 * there is no locking mechanism, but calling dl_insert, dl_delete,
 * or dl_delete_all will clear position state and a subsequent
 * dl_get_next or dl_get_previous return false.
 */

bool
dl_get(
   dlcb *dl,
   long *id,
   void *(*payload)
);

bool
dl_get_first(
   dlcb *dl,
   long *id,
   void *(*payload)
);

bool
dl_get_last(
   dlcb *dl,
   long *id,
   void *(*payload)
);

bool
dl_get_next(
   dlcb *dl,
   long *id,
   void *(*payload)
);

bool
dl_get_previous(
   dlcb *dl,
   long *id,
   void *(*payload)
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
