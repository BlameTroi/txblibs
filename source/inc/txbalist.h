/* txbalist.h -- a poor man's light weight array list */

/*
 * this is a header only implementation of a subset of the java 'array
 * list' memory allocation tracker for use in my library code. it
 * holds pointer sized objects, has a small footprint, and mimics some
 * of the dynamism of lists in lisp.
 *
 * i wrote this because i needed a light weight list accumulator for
 * recursion.
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

#ifndef TXBALIST_H
#define TXBALIST_H

/*
 * a quick array list for pointer sized objects (uintptr_t).
 * its intent is for easy collection of things (often pointers)
 * during recursive processing. using uintptr_t instead of void *
 * worked out better here.
 *
 * the api convention is the returned list is meant to replace the
 * primary list in arguments. if the list has been significantly
 * mutated, the original primary list is freed and an updated
 * copy is returned.
 *
 * always use the array list you get back from an api call, not the
 * one you sent in. for example, in the following, the assert will
 * succeed as the ALIST_DEFAULT_CAP'th pass through the for loop will
 * require an expansion of the array list.
 *
 * alist *xs = make_alist();
 * alist *remember = xs;
 * for (int i = 0; i > ALIST_DEFAULT_CAP * 2; i++)
 *         xs = cons_to_alist(xs, (uintptr_t)i);
 * assert(remember != xs);
 * xs = free_alist(xs);
 * assert(xs == NULL);
 *
 * 'remember' holds the original address of 'xs', but that storage was
 * released when 'xs' was expanded.
 *
 * some of the api is inspired by the java arraylist, and the rest
 * from lisp. it's pseudo lisp lists with fake garbage collection.
 */

#include <stdbool.h>
#include <stdint.h>

/*
 * the array list is a small subset of the functionality of the java
 * arraylist class, but i used more lispish terminology and there is
 * no api for updating elements beyond c array access.
 *
 * cons replaces add, append to join two lists.
 */

#ifndef ALIST_DEFAULT_CAP
#define ALIST_DEFAULT_CAP 50
#endif

typedef struct alist alist;
struct alist {
	int capacity;                /* starts at ALIST_DEFAULT_CAP */
	int used;                    /* how many things are there   */
	uintptr_t list[];            /* open eneded array to anchor */
};

/*
 * create an instance of the array list.
 */

alist *
make_alist(
	void
);

/*
 * release an instance of the array list. the return value is always
 * NULL.
 */

alist *
free_alist(
	alist *xs
);

/*
 * add something that fits in a unintprt_t (currently 8 bytes) to the
 * end of the array list.
 */

alist *
cons_to_alist(
	alist *xs,
	uintptr_t atom
);

/*
 * append one array list to the end of another.
 */

alist *
append_to_alist(
	alist *xs,
	alist *ys
);

/*
 * an iterator of sorts over the array list. call with repeatedly,
 * *index is updated. reaching the end of the array list is signalled
 * by *index == -1.
 *
 * it's just as easy to use array access, but that won't survive a
 * change in the underlying storage approach.
 */

uintptr_t
iterate_alist(
	alist *xs,
	int *index
);

/*
 * is the alist empty?
 */

bool
alist_empty(
	alist *xs
);

/*
 * returns the number of items in the array list.
 */

int
alist_length(
	alist *xs
);

/*
 * creates a copy of an array list.
 */

alist *
clone_alist(
	alist *xs
);

/*
 * return a sublist of the alist. this is a new alist holding only
 * entries [from_inclusive, to_exclusive). i don't like the sublist
 * terminology so i've opted for slice.
 */

alist *
slice_alist(
	alist *xs,
	int from_inclusive,
	int to_exclusive
);

/*
 * size of the array list in bytes. not intended for client use.
 */

int
alist_size(
	alist *xs
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBALIST_H */

#ifdef TXBALIST_IMPLEMENTATION
#undef TXBALIST_IMPLEMENTATION

/*
 * this is a header only implementation of a subset of the java 'array
 * list' memory allocation tracker for use in my library code. it
 * holds pointer sized objects, has a small footprint, and mimics some
 * of the dynamism of lists in lisp.
 *
 * i wrote this because i needed a light weight list accumulator for
 * recursion.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "txballoc.h"

#include "txbalist.h"

/*
 * a quick array list for pointer sized objects (uintptr_t).
 * its intent is for easy collection of things (often pointers)
 * during recursive processing. using uintptr_t instead of void *
 * worked out better here.
 *
 * the api convention is the returned list is meant to replace the
 * primary list in arguments. if the list has been significantly
 * mutated, the original primary list is freed and an updated
 * copy is returned.
 *
 * always use the array list you get back from an api call, not the
 * one you sent in. for example, in the following, the assert will
 * succeed as the ALIST_DEFAULT_CAP'th pass through the for loop will
 * require an expansion of the array list.
 *
 * alist *xs = make_alist();
 * alist *remember = xs;
 * for (int i = 0; i > ALIST_DEFAULT_CAP * 2; i++)
 *         xs = cons_to_alist(xs, (uintptr_t)i);
 * assert(remember != xs);
 * xs = free_alist(xs);
 * assert(xs == NULL);
 *
 * 'remember' holds the original address of 'xs', but that storage was
 * released when 'xs' was expanded.
 *
 * some of the api is inspired by the java arraylist, and the rest
 * from lisp. it's pseudo lisp lists with fake garbage collection.
 */

/*
 * create an instance of the array list.
 */

alist *
make_alist(void) {
	alist *xs = tsmalloc(
			sizeof(*xs) +
			ALIST_DEFAULT_CAP * sizeof(uintptr_t)
		);
	xs->capacity = ALIST_DEFAULT_CAP;
	xs->used = 0;
	memset(&xs->list, 0, xs->capacity * sizeof(uintptr_t));
	return xs;
}

/*
 * is the alist empty?
 */

bool
alist_empty(
	alist *xs
) {
	return xs->used < 1;
}

/*
 * provide a length api instead of looking in to the structure all the
 * time.
 */

int
alist_length(
	alist *xs
) {
	return xs->used;
}

/*
 * size of the array list in bytes. not intended for client use.
 */

int
alist_size(
	alist *xs
) {
	return sizeof(*xs) + xs->capacity * sizeof(uintptr_t);
}

/*
 * release an instance of the array list. the return value is always
 * NULL.
 */

alist *
free_alist(
	alist *xs
) {
	if (xs) {
		int len = alist_size(xs);
		memset(xs, 253, len);
		tsfree(xs);
	}
	return NULL;
}

/*
 * add something that fits in a unintprt_t (currently 8 bytes) to the
 * end of the array list.
 */

alist *
cons_to_alist(
	alist *xs,
	uintptr_t p
) {
	if (xs->used == xs->capacity) {
		int len = alist_size(xs);
		alist *new = tsmalloc(len * 2);
		memset(new, 0, len * 2);
		memcpy(new, xs, len);
		new->capacity = xs->capacity * 2;
		free_alist(xs);
		xs = new;
	}
	xs->list[xs->used] = p;
	xs->used += 1;
	return xs;
}

/*
 * an iterator of sorts over the array list. call with repeatedly,
 * *index is updated. reaching the end of the array list is signalled
 * by *index == -1.
 *
 * it's just as easy to use array access, but that won't survive a
 * change in the underlying storage approach.
 */

uintptr_t
iterate_alist(alist *xs, int *curr) {
	if (*curr < 0)
		return 0;
	if (*curr >= xs->used) {
		*curr = -1;  /* as a null might be valid, use a negative count to also signal end */
		return 0;
	}
	uintptr_t res = xs->list[*curr];
	*curr += 1;
	return res;
}

/*
 * creates a shallow copy of an array list.
 */

alist *
clone_alist(
	alist *xs
) {
	int len = alist_size(xs);
	alist *res = tsmalloc(len);
	memcpy(res, xs, len);
	return res;
}

/*
 * append one array list to the end of another.
 */

alist *
append_to_alist(alist *xs, alist *ys) {

	/* be rational if list to append is empty */
	if (!ys || alist_length(ys) < 1)
		return xs;

	/* if the 'append to' list is empty, return a copy of the
	 * append list. this is consistent with the idea that only the
	 * primary list will be mutated. */
	if (alist_length(xs) < 1) {
		free_alist(xs);
		return clone_alist(ys);
	}

	/* be lazy and just use the iterator for now. we should always
	 * pass through the append loop at least once. */
	alist *new = clone_alist(xs);
	free_alist(xs);
	int index = 0;
	while (true) {
		if (index < 0) break;
		uintptr_t got = iterate_alist(xs, &index);
		new = cons_to_alist(new, got);
	}

	return new;
}

/*
 * create a new alist of the contents from inclusive->to exclusive.
 *
 * ie, [from, to) in standard mathematic notation.
 *
 * this does not have the same semantics as the java arraylist
 * sublist. here we create an entirely new list holding only the
 * elements requested. i've gone with 'slice' as more intention
 * revealing.
 *
 * the original list is preserved.
 */

alist *
slice_alist(
	alist *xs,
	int from_inclusive,
	int to_exclusive
) {
	alist *res = make_alist();

	/* to be explicit about this. */
	if (from_inclusive >= to_exclusive)
		return res;

	for (int i = from_inclusive; i < to_exclusive; i++)
		res = cons_to_alist(res, xs->list[i]);
	return res;
}

#endif /* TXBALIST_IMPLEMENTATION */
/* alist.c ends here */
