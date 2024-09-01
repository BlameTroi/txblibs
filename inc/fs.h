/* fs.h -- blametroi's fixed size stack -- */

/*
 * a header only implementation of a stack.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>

/*
 * an instance of a stack.
 */

typedef struct fscb fscb;

/*
 * create a new stack instance. the memory allocation is large enough to hold
 * 'limit' item pointers.
 */

fscb *
fs_create(
	int limit
);

/*
 * push a new item onto the stack.
 */

void
fs_push(
	fscb *fs,
	void *item
);

/*
 * pop the top item off the stack.
 */

void *
fs_pop(
	fscb *fs
);

/*
 * return the top item from the stack but leave it
 * on the stack.
 */

void *
fs_peek(
	fscb *fs
);

/*
 * predicates for checking the stack.
 */

bool
fs_empty(
	fscb *fs
);

bool
fs_full(
	fscb *fs
);

int
fs_depth(
	fscb *fs
);

int
fs_free(
	fscb *fs
);

/*
 * free stack resources if the stack is empty.
 */

bool
fs_destroy(
	fscb *fs
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
