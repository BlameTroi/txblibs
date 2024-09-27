/* st.c -- blametroi's simple stack -- */

/*
 * a header only implementation of a simple stack.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#undef NDEBUG
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../inc/dl.h"

#include "../inc/st.h"

/*
 * the st is a stack api laid over a doubly linked list.
 */

#define STCB_TAG "__STCB__"
#define STCB_TAG_LEN 8
#define ASSERT_STCB(p, m) assert((p) && memcmp((p), STCB_TAG, STCB_TAG_LEN) == 0 && (m))
#define ASSERT_STCB_OR_NULL(p) assert((p) == NULL || memcmp((p), STCB_TAG, STCB_TAG_LEN) == 0)

struct stcb {
	char tag[STCB_TAG_LEN];
	dlcb *dl;
};

/*
 * create a new empty stack instance large enough to hold 'limit' items.
 */

stcb *
st_create(
	void
) {
	stcb *st = malloc(sizeof(*st));
	memset(st, 0, sizeof(*st));
	memcpy(st->tag, STCB_TAG, sizeof(st->tag));
	st->dl = dl_create();
	return st;
}

/*
 * push a new item on the stack.
 */

void
st_push(
	stcb *st,
	void *item
) {
	ASSERT_STCB(st, "invalid STCB");
	dl_insert_last(st->dl, item);
}

/*
 * pop an item from the stack.
 */

void *
st_pop(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	dlnode *dn = dl_get_last(st->dl);
	assert(dn && "stack empty");
	void *ret = dn->payload;
	dl_delete(st->dl, dn);
	return ret;
}

/*
 * get the top item from the stack without removing it.
 */

void *
st_peek(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	dlnode *dn = dl_get_last(st->dl);
	assert(dn && "stack empty");
	void *ret = dn->payload;
	return ret;
}

/*
 * predicates and status queries.
 */

bool
st_empty(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	return dl_empty(st->dl);
}

int
st_depth(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	return dl_count(st->dl);
}

/*
 * if the stack is empty, release its resources.
 */

bool
st_destroy(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	if (!st_empty(st))
		return false;
	dl_destroy(st->dl);
	memset(st, 253, sizeof(*st));
	free(st);
	return true;
}
/* st.c ends here */
