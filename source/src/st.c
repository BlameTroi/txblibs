/* qu.c -- blametroi's simple queue -- */

/*
 * a header only implementation of a stack.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/abort_if.h"

#include "../inc/st.h"
#include "../inc/dl.h"

/*
 * transparent control block definitions.
 */

#define STCB_TAG "__STCB__"
#define STCB_TAG_LEN 8

#define ASSERT_STCB(p, m) \
	abort_if(!(p) || memcmp((p), STCB_TAG, STCB_TAG_LEN) != 0, (m));

#define ASSERT_STCB_OR_NULL(p, m) \
	abort_if(p && memcmp((p), STCB_TAG, STCB_TAG_LEN) != 0, (m));

struct stcb {
	char tag[STCB_TAG_LEN];
	dlcb *dl;
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
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
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
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
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
	stcb *st,
	void *payload
) {
	ASSERT_STCB(st, "invalid STCB");
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
	stcb *st
) {
	dlid id;
	ppayload payload;
	ASSERT_STCB(st, "invalid STCB");
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
	stcb *st
) {
	ppayload payload;
	ASSERT_STCB(st, "invalid STCB");
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

stcb *
st_create(
	void
) {
	stcb *st = malloc(sizeof(*st));
	abort_if(!st,
		"st_create could not allocate STCB");
	memset(st, 0, sizeof(*st));
	memcpy(st->tag, STCB_TAG, sizeof(st->tag));
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
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
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
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	return dl_destroy(st->dl);
}

/* st.c ends here */
