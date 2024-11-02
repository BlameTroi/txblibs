/* st.h -- blametroi's simple stack -- */

/*
 * a header only implementation of a stack.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TXBST_H
#define TXBST_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * a handle to a stack instance.
 */

typedef struct stcb hst;

/*
 * ppayload, pkey, pvalue
 *
 * these libraries manage client 'payloads'. these are void * sized
 * and are generally assumed to be a pointer to client managed data,
 * but anything that will fit in a void * pointer (typically eight
 * bytes) is allowed.
 *
 * it is the client's responsibility to free any of its dynamically
 * allocated memory. library code provides 'destroy' methods to clear
 * and release library data structures.
 *
 * these type helpers are all synonyms for void *.
 */

typedef void * pkey;
typedef void * pvalue;
typedef void * ppayload;

/*
 * st_empty
 *
 * are there any items on the stack?
 *
 *     in: the st instance
 *
 * return: boolean, true if empty
 */

bool
st_empty(
	hst *
);

/*
 * st_push
 *
 * add an item to the top of the stack.
 *
 *     in: the st instance
 *
 *     in: ppayload
 *
 * return: nothing
 */

void
st_push(
	hst *,
	ppayload
);

/*
 * st_pop
 *
 * remove and return the top item on the stack.
 *
 *     in: the st instance
 *
 * return: ppayload or NULL if the stack is empty
 */

ppayload
st_pop(
	hst *
);

/*
 * st_peek
 *
 * return but do not remove the top item on the stack.
 *
 *     in: the st instance
 *
 * return: ppayload or NULL If the stack is empty
 */

ppayload
st_peek(
	hst *
);

/*
 * st_create
 *
 * create a new stack.
 *
 *     in: nothing
 *
 * return: the new st instance
 */

hst *
st_create(
	void
);

/*
 * st_destroy
 *
 * free the stack control block if the stack is empty.
 *
 *     in: the st instance
 *
 * return: boolean, true if successful
 */

bool
st_destroy(
	hst *
);

/*
 * st_reset
 *
 * remove all items from the stack.
 *
 *     in: the st instance
 *
 * return: integer number of items removed
 */

int
st_reset(
	hst *
);

/*
 * st_depth
 *
 * how many items are in the stack?
 *
 *     in: the st instance
 *
 * return: inegert, number of items on the stack
 */

int
st_depth(
	hst *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBST_H */

#ifdef TXBST_IMPLEMENTATION
#undef TXBST_IMPLEMENTATION

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "txbabort.h"
#include "txbdl.h"

/*
 * transparent control block definitions.
 */

#define HST_TAG "__HST__"
#define HST_TAG_LEN 8

#define ASSERT_HST(p, m) \
	abort_if(!(p) || memcmp((p), HST_TAG, HST_TAG_LEN) != 0, (m));

#define ASSERT_HST_OR_NULL(p, m) \
	abort_if(p && memcmp((p), HST_TAG, HST_TAG_LEN) != 0, (m));

struct stcb {
	char tag[HST_TAG_LEN];
	hdl *dl;
};

/*
 * this is a simple stack implementation (lifo).
 *
 * error checking:
 *
 * minimal via assertions. fatal errors such as passing an invalid
 * stcb will fail the assertion. st_destroy will return a false
 * if it can not complete, but that shouldn't happen. i just haven't
 * worked out the threading around there.
 *
 * brief api overview:
 *
 * all functions except st_create take a stcb*, the stack control
 * block. the implementation is just an abstraction layer over a
 * doubly linked list. a dynamic array would be faster but this
 * is more flexible.
 *
 * the data to manage on the stack is a ppayload.
 *
 * the order of retrieval of items is first in first out.
 *
 * st_create  -- create a new stack, returns a stcb*
 *
 * st_empty   -- is the stack empty? returns boolean
 *
 * st_count   -- how many items are in the stack? returns int
 *
 * st_push    -- add an item to the stack.
 *
 * st_pop     -- remove and return the top item from the stack.
 *
 * st_peek    -- return but do not remove the top item from the stack.
 *
 * st_reset   -- remove all items from the stack.
 *
 * st_destroy -- if the stack is empty and not in use, release
 *               the stcb. returns true if successful.
 */

/*
 * st_empty
 *
 * are there items in the stack?
 *
 *     in: the st instance
 *
 * return: boolean, true if empty
 */

bool
st_empty(
	hst *st
) {
	ASSERT_HST(st, "invalid HST");
	return dl_empty(st->dl);
}

/*
 * st_count
 *
 * how many items are in the stack?
 *
 *     in: the st instance.
 *
 * return: int, number of items.
 */

int
st_depth(
	hst *st
) {
	ASSERT_HST(st, "invalid HST");
	return dl_count(st->dl);
}

/*
 * st_push
 *
 * add an item to the stack.
 *
 *     in: the st instance
 *
 *     in: void * client payload
 *
 * return: nothing.
 */

void
st_push(
	hst *st,
	void *payload
) {
	ASSERT_HST(st, "invalid HST");
	dl_insert_first(st->dl, payload);
}

/*
 * st_pop
 *
 * remove and return the first (oldest) item on the stack.
 *
 *     in: the st instance
 *
 * return: ppayload or NULL if the stack is empty
 */

ppayload
st_pop(
	hst *st
) {
	dlid id;
	ppayload payload;
	ASSERT_HST(st, "invalid HST");
	id = dl_get_first(st->dl, &payload);
	if (payload != NULL) {
		dl_delete(st->dl, id);
	}
	return payload;
}

/*
 * st_peek
 *
 * return the first (oldest) item on the stack but leave
 * it on the stack.
 *
 *     in: the st instance
 *
 * return: void * client payload or NULL if stack is empty.
 */

ppayload
st_peek(
	hst *st
) {
	ppayload payload;
	ASSERT_HST(st, "invalid HST");
	dl_get_first(st->dl, &payload);
	return payload;
}

/*
 * st_create
 *
 * create a new stack.
 *
 *     in: nothing
 *
 * return: the new st instance.
 */

hst *
st_create(
	void
) {
	hst *st = malloc(sizeof(*st));
	abort_if(!st,
		"st_create could not allocate HST");
	memset(st, 0, sizeof(*st));
	memcpy(st->tag, HST_TAG, sizeof(st->tag));
	st->dl = dl_create();
	return st;
}

/*
 * st_reset
 *
 * remove all items from the stack.
 *
 *     in: the st instance
 *
 * return: int number of items removed
 */

int
st_reset(
	hst *st
) {
	ASSERT_HST(st, "invalid HST");
	return dl_reset(st->dl);
}


/*
 * st_destroy
 *
 * free the stack control block if the stack is empty.
 *
 *     in: the st instance
 *
 * return: boolean, true if successful, false if stack is not empty
 */

bool
st_destroy(
	hst *st
) {
	ASSERT_HST(st, "invalid HST");
	return dl_destroy(st->dl);
}
#endif /* TXBST_IMPLEMENTATION */
/* txbst.h ends here */
