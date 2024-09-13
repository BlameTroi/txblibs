/*
 * single file header generated via:
 * buildhdr --macro TXBFS --intro LICENSE --pub inc/fs.h --priv src/fs.c 
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Troy Brumley 
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   *** end intro ***
 */

#ifndef TXBFS_SINGLE_HEADER
#define TXBFS_SINGLE_HEADER
/* *** begin pub *** */
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
/* *** end pub *** */

#endif /* TXBFS_SINGLE_HEADER */

#ifdef TXBFS_IMPLEMENTATION
#undef TXBFS_IMPLEMENTATION
/* *** begin priv *** */
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
/* *** end priv *** */

#endif /* TXBFS_IMPLEMENTATION */
