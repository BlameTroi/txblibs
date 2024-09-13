/* fs.c -- blametroi's fixed size stack -- */

/*
 * a header only implementation of a simple stack.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../inc/fs.h"

/*
 * a fixed length stack, as in the stack size is set at creation.
 */
#define FSCB_TAG "__FSCB__"
#define FSCB_TAG_LEN 8
#define ASSERT_FSCB(p, m) assert((p) && memcmp((p), FSCB_TAG, FSCB_TAG_LEN) == 0 && (m))
#define ASSERT_FSCB_OR_NULL(p) assert((p) == NULL || memcmp((p), FSCB_TAG, FSCB_TAG_LEN) == 0)

struct fscb {
	char tag[FSCB_TAG_LEN];
	int limit;
	int top;
	void *stack[];
};

/*
 * create a new empty stack instance large enough to hold 'limit' entries.
 */

fscb *
fs_create(
	int limit
) {
	fscb *fs = malloc(sizeof(*fs) + sizeof(void *) * (limit + 1));
	memset(fs, 0, sizeof(*fs) + sizeof(void *) * (limit + 1));
	memcpy(fs->tag, FSCB_TAG, sizeof(fs->tag));
	fs->limit = limit;
	fs->top = -1;
	return fs;
}

/*
 * push a new entry on the stack.
 */

void
fs_push(
	fscb *fs,
	void *item
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	assert(fs->top + 1 < fs->limit);
	fs->top += 1;
	fs->stack[fs->top] = item;
}

/*
 * pop an entry from the stack.
 */

void *
fs_pop(
	fscb *fs
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	assert(fs->top > -1);
	void *ret = fs->stack[fs->top];
	fs->top -= 1;
	return ret;
}

/*
 * get the top entry from the stack without removing it.
 */

void *
fs_peek(
	fscb *fs
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	assert(fs->top > -1);
	void *ret = fs->stack[fs->top];
	return ret;
}

/*
 * predicates and status queries.
 */

bool
fs_empty(
	fscb *fs
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	return fs->top == -1;
}

bool
fs_full(
	fscb *fs
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	return fs->top < fs->limit;
}

int
fs_depth(
	fscb *fs
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	return fs->top;
}

int
fs_free(
	fscb *fs
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	return fs->top - fs->limit;
}

/*
 * if the stack is empty, release its resources.
 */

bool
fs_destroy(
	fscb *fs
) {
	ASSERT_FSCB(fs, "invalid FSCB");
	if (fs->top > -1)
		return false;
	memset(fs, 253, sizeof(*fs) + sizeof(void *) * (fs->limit + 1));
	free(fs);
	return true;
}
