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

#include "dl.h"

#include "st.h"

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
 * sb_create
 *
 * create a new empty stack instance.
 *
 *     in: nothing
 *
 * return: the st instance
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
 * st_push
 *
 * push a new item on the stack.
 *
 *     in: the st instance
 *
 *     in: the item as a void *
 *
 * return: nothing
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
 * st_pop
 *
 * pop an item off the stack.
 *
 *     in: the st instance
 *
 * return: the item as a void *
 */

void *
st_pop(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	void *payload = NULL;
	dlid id = dl_get_last(st->dl, &payload);
	assert(!null_dlid(id) && "stack empty");
	dl_delete(st->dl, id);
	return payload;
}

/*
 * st_peek
 *
 * get the top item from the stack without removing it.
 *
 *     in: the st instance
 *
 * return: the item as a void *
 */

void *
st_peek(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	void *payload = NULL;
	dlid id = dl_get_last(st->dl, &payload);
	assert(!null_dlid(id) && "stack empty");
	return payload;
}

/*
 * st_empty
 *
 * is the stack empty?
 *
 *    in: the st instance
 *
 * return: bool
 */

bool
st_empty(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	return dl_empty(st->dl);
}

/*
 * st_depth
 *
 * how many items are on the stack?
 *
 *    in: the st instance
 *
 * return: int
 */

int
st_depth(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	return dl_count(st->dl);
}

/*
 * st_reset
 *
 * delete all items from the stack.
 *
 *     in: the st instance
 *
 * return: int number of items deleted
 */

int
st_reset(
	stcb *st
) {
	ASSERT_STCB(st, "invalid STCB");
	return dl_reset(st->dl);
}

/*
 * sb_destroy
 *
 * if the stack is empty, release its resources.
 *
 *     in: the st instance
 *
 * return: bool was the st destroyed and freed
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
