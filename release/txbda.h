/*
 * single file header generated via:
 * buildhdr --macro TXBDA --intro LICENSE --pub inc/da.h --priv src/da.c 
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

#ifndef TXBDA_SINGLE_HEADER
#define TXBDA_SINGLE_HEADER
/* *** begin pub *** */
/* da.h -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic and somewhat leaky
 * dynamic array.
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

#include <stdlib.h>

/*
 * the da is a dynmically sized array. to deal with various datatypes
 * ranging from standard types to structures the da holds void *
 * pointers. storage management of elements stored in the da is the
 * responsibility of the client. freeing the da by da_destroy only
 * removes the dacb and supporting structures.
 *
 * the da grows by doubling its current allocation.
 *
 * initially all elements of the da are NULL. gaps are allowed. so
 * after:
 *
 * dacb *da = da_create(10);
 * char *data = "1234";
 * da_put(da, 5, data);
 *
 * elements 0 through 5 are defined but only element 5 is non NULL.
 *
 * char *readdata = da_get(da, 1); <-- returns a NULL
 *       readdata = da_get(da, 5); <-- returns pointer to "1234"
 *       readdata = da_get(da, 8); <-- fails
 */

/*
 * an instance of a dynamic array.
 */

typedef struct dacb dacb;

/*
 * da_create
 *
 * create a new instance of a dynamic array. the lone argument is the
 * number of entries in the initial allocation. if more are needed,
 * the allocation doubles.
 *
 * returns the da instance.
 */

dacb *
da_create(
	int size_or_zero_for_default
);

/*
 * da_destroy
 *
 * overwrite and release all dynamically allocated memory for a da.
 */

void
da_destroy(
	dacb *da
);

/*
 * da_get
 *
 * return the contents of array index n which will be NULL if nothing
 * has been put at that index.
 *
 * fails via an assert if n greater than the highest index of a da_put.
 *
 * takes the da instance and an integer index.
 *
 * returns the item as a void *.
 */

void *
da_get(
	dacb *da,
	int n
);

/*
 * da_put
 *
 * insert or overwrite the contents of array index n.
 *
 * takes the da instance, integer index, and the item to insert as a
 * void *.
 */

void
da_put(
	dacb *da,
	int n,
	void *put);

/*
 * da_count
 *
 * how many entries (null or otherwise) does the array hold. this will
 * be one more than the highest 'n' passed to da_put.
 *
 * takes the da instance and returns an int count.
 */

int
da_count(
	dacb *da
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* da.h ends here */
/* *** end pub *** */

#endif /* TXBDA_SINGLE_HEADER */

#ifdef TXBDA_IMPLEMENTATION
#undef TXBDA_IMPLEMENTATION
/* *** begin priv *** */
/* da.c -- blametroi's dynamic array library */

/*
 * a header only implementation of a very basic and somewhat leaky
 * dynamic array.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*
 * the trasnparent definition of the dacb. the default size for number
 * of elements is arbitrary and could be changed. the array storage
 * grows by doubling the current size.
 */

#define DACB_TAG "__DACB__"
#define DACB_TAG_LEN 8
#define ASSERT_DACB(p, m) assert((p) && memcmp((p), DACB_TAG, DACB_TAG_LEN) == 0 && (m))
#define ASSERT_DACB_OR_NULL(p) assert((p) == NULL || memcmp((p), DACB_TAG, DACB_TAG_LEN) == 0)

#define DACB_DEFAULT_SIZE 512

struct dacb {
	char tag[DACB_TAG_LEN];     /* eye catcher & verification */
	int length;                 /* last used (via put) entry */
	int size;                   /* size of data in number of entries */
	void **data;                /* pointer to the entry pointers */
};

/*
 * da_create
 *
 * create a new instance of a dynamic array. the lone argument is the
 * number of entries in the initial allocation. if more are needed,
 * the allocation doubles.
 *
 * returns the da instance.
 */

dacb *
da_create(
	int size_or_zero_for_default
) {
	dacb *da = malloc(sizeof(dacb));
	memset(da, 0, sizeof(dacb));
	memcpy(da->tag, DACB_TAG, sizeof(da->tag));
	da->size = size_or_zero_for_default ? size_or_zero_for_default :
		DACB_DEFAULT_SIZE;
	da->data = malloc(da->size *sizeof(void *));
	da->length = -1;
	memset(da->data, 0, da->size *sizeof(void *));
	return da;
}

/*
 * da_destroy
 *
 * overwrite and release all dynamically allocated memory for a da.
 */

void
da_destroy(
	dacb *da
) {
	ASSERT_DACB(da, "invalid DACB");
	memset(da->data, 0, da->size *sizeof(void *));
	free(da->data);
	memset(da, 0, sizeof(dacb));
	free(da);
}

/*
 * da_get
 *
 * return the contents of array index n which will be NULL if nothing
 * has been put at that index.
 *
 * fails via an assert if n greater than the highest index of a da_put.
 *
 * takes the da instance and an integer index.
 *
 * returns the item as a void *.
 */

void *
da_get(
	dacb *da,
	int n
) {
	ASSERT_DACB(da, "invalid DACB");
	assert(n < da->size);
	void *res = da->data[n];
	return res;
}

/*
 * da_put
 *
 * store an entry in the array. if the location is outside the current
 * buffer, repeatedly double the buffer size until it can hold the
 * location.
 */

void
da_put(
	dacb *da,
	int n,
	void *put
) {
	ASSERT_DACB(da, "invalid DACB");
	assert(put);
	while (n >= da->size) {
		void **old = da->data;
		da->data = malloc(2 * da->size *sizeof(void *));
		memcpy(da->data, old, da->size *sizeof(void *));
		memset(old, 0, da->size *sizeof(void *));
		free(old);
		da->size = 2 * da->size;
	}
	da->data[n] = put;
	if (n > da->length)
		da->length = n;
}


/*
 * da_count
 *
 * report the number of elements in the array if all 0 .. n were
 * added. i hate zero based indices, but they are the norm so +1
 * here.
 */

int
da_count(
	dacb *da
) {
	ASSERT_DACB(da, "invalid DACB");
	return da->length + 1;
}

/* da.c ends here */
/* *** end priv *** */

#endif /* TXBDA_IMPLEMENTATION */
