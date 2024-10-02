/*
 * single file header generated via:
 * buildhdr --macro TXBST --intro LICENSE --pub inc/st.h --priv src/st.c 
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

#ifndef TXBST_SINGLE_HEADER
#define TXBST_SINGLE_HEADER
/* *** begin pub *** */
/* st.h -- blametroi's fixed size stack -- */

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

typedef struct stcb stcb;

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
);

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
);
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
);

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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* st.h ends here */
/* *** end pub *** */

#endif /* TXBST_SINGLE_HEADER */

#ifdef TXBST_IMPLEMENTATION
#undef TXBST_IMPLEMENTATION
/* *** begin priv *** */
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
	dlnode *dn = dl_get_last(st->dl);
	assert(dn && "stack empty");
	void *ret = dn->payload;
	dl_delete(st->dl, dn);
	return ret;
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
	dlnode *dn = dl_get_last(st->dl);
	assert(dn && "stack empty");
	void *ret = dn->payload;
	return ret;
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
/* *** end priv *** */

#endif /* TXBST_IMPLEMENTATION */
