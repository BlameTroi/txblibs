/*
 * single file header generated via:
 * buildhdr --macro TXBST --intro LICENSE --pub ./source/inc/st.h --priv ./source/src/st.c 
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

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * an instance of a stack.
 */

typedef struct stcb stcb;

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
	stcb *
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
	stcb *,
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
	stcb *
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
	stcb *
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

stcb *
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
	stcb *
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
	stcb *
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
	stcb *
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

#include "txbabort_if.h"

#include "txbdl.h"

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
/* *** end priv *** */

#endif /* TXBST_IMPLEMENTATION */
