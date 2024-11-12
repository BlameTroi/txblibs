/* txbone.c -- one data structure library to rule them all */

/*
 * a header only implementation several data structures (linked lists,
 * queues, dynamic arrays, etc.) that i have written while working on
 * advent of code problems. this is a 'grand unified' library as i
 * rewrite several of the prior implementations for consistency and
 * some actual reuse. along the way i've added simplistic memory leak
 * tracking.
 *
 * there are better dst libraries out there, but anyone is welcome to
 * use these if they wish.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <string.h>

#include "txballoc.h"
#include "txbone.h"

/*
 * keep these in synch with the isa types in txbone.h:
 */

static
const
char *
one_tags[] = {
	"unknown",            /* -- enum names -- */
	"deque",                 /* deque */
	"queue",                 /* queue */
	"stack",                 /* stack */
	"singly linked list",    /* singly */
	"doubly linked list",    /* doubly */
	"dynamic array",         /* dynarray */
	"binary search tree",    /* bst */
	"key:value store",       /* keyval */
	"hash table",            /* hash */
	"dictionary",           /* dictionary */
	"bag",                  /* bag */
	"ordered collection",   /* ordered */
	NULL
};

/*
 * singly linked list implementations.
 */

static
one_singly *
singly_add_first(
	one_singly *me,
	void *payload
) {
	sgl_item *next = tmalloc(sizeof(*next));
	memset(next, 0, sizeof(*next));
	next->payload = payload;
	next->next = me->first;
	me->first = next;
	return me;
}

static
void *
singly_peek_first(
	one_singly *me
) {
	sgl_item *first = me->first;
	if (first == NULL) return NULL;

	return first->payload;
}

static
void *
singly_get_first(
	one_singly *me
) {
	sgl_item *first = me->first;
	if (first == NULL) return NULL;

	me->first = first->next;
	void *res = first->payload;
	memset(first, 253, sizeof(*first));
	tfree(first);
	return res;
}

static
one_singly *
singly_add_last(
	one_singly *me,
	void *payload
) {
	sgl_item *next = tmalloc(sizeof(*next));
	memset(next, 0, sizeof(*next));
	next->payload = payload;

	/* empty list is dead simple */
	if (me->first == NULL) {
		me->first = next;
		return me;
	}

	/* find current end and link */
	sgl_item *curr = me->first;
	while (curr->next)
		curr = curr->next;
	curr->next = next;
	return me;
}

static
void *
singly_peek_last(
	one_singly *me
) {
	sgl_item *curr = me->first;
	if (curr == NULL) return NULL;

	/* chase to end. */
	while (curr->next)
		curr = curr->next;

	/* and payload back */
	return curr->payload;
}

static
void *
singly_get_last(
	one_singly *me
) {
	sgl_item *curr = me->first;
	if (curr == NULL) return NULL;

	/* chase to end, remembering preceeding */
	sgl_item *previous = NULL;
	while (curr->next) {
		previous = curr;
		curr = curr->next;
	}

	/* if no previous item, there was only one item. if there was
	 * a previous item, clear it's next pointer. */
	if (previous)
		previous->next = NULL;
	else
		me->first = NULL;

	/* extract payload, clear and free old item */
	void *res = curr->payload;
	memset(curr, 253, sizeof(*curr));
	tfree(curr);
	return res;
}

static
int
singly_count(
	one_singly *me
) {
	int count = 0;
	sgl_item *curr = me->first;
	while (curr) {
		count += 1;
		curr = curr->next;
	}
	return count;
}

static
int
singly_purge(
	one_singly *me
) {
	int count = 0;
	sgl_item *curr = me->first;
	me->first = NULL;
	while (curr) {
		count += 1;
		sgl_item *del = curr;
		curr = curr->next;
		memset(del, 253, sizeof(*del));
		tfree(del);
	}
	return count;
}

/*
 * doubly linked list implementations.
 */

static
one_doubly *
doubly_add_first(
	one_doubly *me,
	void *payload
) {
	dbl_item *first = tmalloc(sizeof(*first));
	memset(first, 0, sizeof(*first));
	first->payload = payload;
	first->next = me->first;
	first->previous = NULL;

	if (me->first == NULL) {
		me->first = first;
		me->last = first;
		return me;
	}

	me->first->previous = first;
	me->first = first;
	return me;
}

static
void *
doubly_peek_first(
	one_doubly *me
) {
	if (me->first == NULL) return NULL;

	return me->first->payload;
}

static
void *
doubly_get_first(
	one_doubly *me
) {
	if (me->first == NULL) return NULL;

	dbl_item *first = me->first;
	me->first = first->next;

	if (first->next) first->next->previous = NULL;
	else me->last = NULL;

	void *res = first->payload;
	memset(first, 253, sizeof(*first));
	tfree(first);
	return res;
}

static
one_doubly *
doubly_add_last(
	one_doubly *me,
	void *payload
) {
	dbl_item *last = tmalloc(sizeof(*last));
	memset(last, 0, sizeof(*last));
	last->payload = payload;
	last->previous = me->last;
	last->next = NULL;

	if (me->first == NULL) {
		me->first = last;
		me->last = last;
		return me;
	}

	me->last->next = last;
	me->last = last;
	return me;
}

static
void *
doubly_peek_last(
	one_doubly *me
) {
	if (me->last == NULL) return NULL;

	return me->last->payload;
}

static
void *
doubly_get_last(
	one_doubly *me
) {
	if (me->last == NULL) return NULL;

	dbl_item *last = me->last;
	me->last = last->previous;
	if (last->previous)
		last->previous->next = NULL;
	else {
		me->first = NULL;
		me->last = NULL;
	}
	void *res = last->payload;
	memset(last, 253, sizeof(*last));
	tfree(last);
	return res;
}

static
int
doubly_count(
	one_doubly *me
) {
	int count = 0;
	dbl_item *curr = me->first;
	while (curr) {
		count += 1;
		curr = curr->next;
	}
	return count;
}

static
int
doubly_purge(
	one_doubly *me
) {
	int count = 0;
	dbl_item *curr = me->first;
	me->first = NULL;
	me->last = NULL;
	while (curr) {
		count += 1;
		dbl_item *del = curr;
		curr = curr->next;
		memset(del, 253, sizeof(*del));
		tfree(del);
	}
	return count;
}

/*
 * generic entries, most of these route control to detailed
 * implementations for each advanced data type.
 */

one_block *
make_one(
	enum one_type isa
) {
	one_block *ob = tmalloc(sizeof(*ob));
	memset(ob, 0, sizeof(*ob));
	ob->isa = isa;
	if (isa <= ONE_TYPE_MAX && isa > 0)
		strncpy(ob->tag, one_tags[isa], ONE_TAG_LEN-1);
	else
		strncpy(ob->tag, "*invalid one type*", ONE_TAG_LEN-1);

	switch (ob->isa) {
	case singly:
	case stack:
		ob->dtl.sgl.first = NULL;
		return ob;

	case doubly:
	case queue:
	case deque:
		ob->dtl.dbl.first = NULL;
		ob->dtl.dbl.last = NULL;
		return ob;

	case dynarray:
		ob->dtl.dyn.length = -1;
		ob->dtl.dyn.capacity = DYNARRAY_DEFAULT_CAPACITY;
		ob->dtl.dyn.array = tmalloc(DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
		memset(ob->dtl.dyn.array, 0, DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
		return ob;

	default:
		fprintf(stderr, "\nTXBONE error make_one: unknown or unsupported type %d %s\n",
			isa,
			ob->tag);
		memset(ob, 253, sizeof(*ob));
		tfree(ob);
		return NULL;
	}
}

one_block *
free_one(
	one_block *me
) {
	switch (me->isa) {

	case singly:
	case stack:
	case doubly:
	case queue:
	case deque:
		purge(me);
		memset(me, 253, sizeof(*me));
		tfree(me);
		return me;

	case dynarray:
		memset(me->dtl.dyn.array, 253, me->dtl.dyn.capacity * sizeof(void *));
		tfree(me->dtl.dyn.array);
		memset(me, 253, sizeof(*me));
		tfree(me);
		return me;

	default:
		fprintf(stderr, "\nTXBONE error free_one: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		memset(me, 253, sizeof(*me));
		tfree(me);
		return NULL;
	}
}

one_block *
add_first(
	one_block *me,
	void *payload
) {
	if (payload == NULL)
		return me;
	switch (me->isa) {

	case singly:
		singly_add_first(&me->dtl.sgl, payload);
		return me;

	case doubly:
		doubly_add_first(&me->dtl.dbl, payload);
		return me;

	default:
		fprintf(stderr, "\nTXBONE error add_first: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

one_block *
add_last(
	one_block *me,
	void *payload
) {
	if (payload == NULL)
		return me;
	switch (me->isa) {

	case singly:
		singly_add_last(&me->dtl.sgl, payload);
		return me;

	case doubly:
		doubly_add_last(&me->dtl.dbl, payload);
		return me;

	default:
		fprintf(stderr, "\nTXBONE error add_last: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
peek_first(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_peek_first(&me->dtl.sgl);

	case doubly:
		return doubly_peek_first(&me->dtl.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_first: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
peek_last(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_peek_last(&me->dtl.sgl);

	case doubly:
		return doubly_peek_last(&me->dtl.dbl);

	default:
		fprintf(stderr, "\nTXBONE error peek_last: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
get_first(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_get_first(&me->dtl.sgl);

	case doubly:
		return doubly_get_first(&me->dtl.dbl);

	default:
		fprintf(stderr, "\nTXBONE error get_first: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
get_last(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_get_last(&me->dtl.sgl);

	case doubly:
		return doubly_get_last(&me->dtl.dbl);

	default:
		fprintf(stderr, "\nTXBONE error get_last: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

int
count(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_count(&me->dtl.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_count(&me->dtl.dbl);

	default:
		fprintf(stderr, "\nTXBONE error count: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return -1;
	}
}

bool
empty(
	one_block *me
) {
	switch (me->isa) {

	case singly:
	case stack:
		return me->dtl.sgl.first == NULL;

	case doubly:
	case queue:
	case deque:
		return me->dtl.dbl.first == NULL;

	default:
		fprintf(stderr, "\nTXBONE error empty: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return false;
	}
}

int
purge(
	one_block *me
) {
	switch (me->isa) {

	case singly:
	case stack:
		return singly_purge(&me->dtl.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_purge(&me->dtl.dbl);

	default:
		fprintf(stderr, "\nTXBONE error purge: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return -1;
	}
}

/*
 * stack (lifo) is implemented on a singly linked list, but it should
 * use the following entry points.
 */

one_block *
push(
	one_block *me,
	void *payload
) {
	switch (me->isa) {

	case stack:
		singly_add_first(&me->dtl.sgl, payload);
		return me;

	default:
		fprintf(stderr,
			"\nTXBONE error push: unknown or unsupported type %d %s, expected stack\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
pop(
	one_block *me
) {
	switch (me->isa) {

	case stack:
		return singly_get_first(&me->dtl.sgl);

	default:
		fprintf(stderr,
			"\nTXBONE error pop: unknown or unsupported type %d %s, expected stack\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * a queue (fifo) has enqueue and dequeue instead of push and pop, and is
 * built on a doubly linked list.
 */

one_block *
enqueue(
	one_block *me,
	void *payload
) {
	switch (me->isa) {

	case queue:
		doubly_add_last(&me->dtl.dbl, payload);
		return me;

	default:
		fprintf(stderr,
			"\nTXBONE error enqueue: unknown or unsupported type %d %s, expected queue\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
dequeue(
	one_block *me
) {
	switch (me->isa) {

	case queue:
		return doubly_get_first(&me->dtl.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error dequeue: unknown or unsupported type %d %s, expected queue\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * peek is common to stack and queue.
 */

void *
peek(
	one_block *me
) {
	switch (me->isa) {

	case stack:
		return singly_peek_first(&me->dtl.sgl);

	case queue:
		return doubly_peek_first(&me->dtl.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek: unknown or unsupported type %d %s, expected stack or queue\n",
			me->isa, me->tag);
		return NULL;
	}
}

int
depth(
	one_block *me
) {
	switch (me->isa) {

	case stack:
		return singly_count(&me->dtl.sgl);

	default:
		fprintf(stderr,
			"\nTXBONE error depth: unknown or unsupported type %d %s, expected stack\n",
			me->isa, me->tag);
		return -1;
	}
}

/*
 * deque (double ended queue) is built on a doubly linked list but uses
 * these entry points along with count, purge, and empty.
 */

one_block *
push_front(
	one_block *me,
	void *payload
) {
	switch (me->isa) {

	case deque:
		doubly_add_first(&me->dtl.dbl, payload);
		return me;

	default:
		fprintf(stderr,
			"\nTXBONE error push_front: unknown or unsupported type %d %s, expected deque\n",
			me->isa, me->tag);
		return NULL;
	}
}

one_block *
push_back(
	one_block *me,
	void *payload
) {
	switch (me->isa) {

	case deque:
		doubly_add_last(&me->dtl.dbl, payload);
		return me;

	default:
		fprintf(stderr,
			"\nTXBONE error push_back: unknown or unsupported type %d %s, expected deque\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
pop_front(
	one_block *me
) {
	switch (me->isa) {

	case deque:
		return doubly_get_first(&me->dtl.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error pop_front: unknown or unsupported type %d %s, expected deque\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
pop_back(
	one_block *me
) {
	switch (me->isa) {

	case deque:
		return doubly_get_last(&me->dtl.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error pop_back: unknown or unsupported type %d %s, expected deque\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
peek_front(
	one_block *me
) {
	switch (me->isa) {

	case deque:
		return doubly_peek_first(&me->dtl.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_front: unknown or unsupported type %d %s, expected deque\n",
			me->isa, me->tag);
		return NULL;
	}
}

void *
peek_back(
	one_block *me
) {
	switch (me->isa) {

	case deque:
		return doubly_peek_last(&me->dtl.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_back: unknown or unsupported type %d %s, expected deque\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * dynamic arrays are resizing arrays. in addition to make and free, they
 * support hbound, get, put. TODO: sort and func for sort.
 */

int
high_index(
	one_block *me
) {
	if (me->isa != dynarray) {
		fprintf(stderr,
			"\nTXBONE error high_index: unknown or unsupported type %d %s, expected dynarray\n",
			me->isa, me->tag);
		return -1;
	}
	return me->dtl.dyn.length;
}

one_block *
put_at(
	one_block *me,
	int n,
	void *payload
) {
	if (me->isa != dynarray) {
		fprintf(stderr,
			"\nTXBONE error put_at: unknown or unsupported type %d %s, expected dynarray\n",
			me->isa, me->tag);
		return NULL;
	}
	if (n < 0) {
		fprintf(stderr,
			"\nTXBONE error put_at: index may not be negative %d\n", n);
		return NULL;
	}
	while (n >= me->dtl.dyn.capacity) {
		void *old = me->dtl.dyn.array;
		me->dtl.dyn.array = tmalloc(2 * me->dtl.dyn.capacity * sizeof(void *));
		memset(me->dtl.dyn.array, 0, 2 * me->dtl.dyn.capacity * sizeof(void *));
		memcpy(me->dtl.dyn.array, old, me->dtl.dyn.capacity * sizeof(void *));
		memset(old, 253, me->dtl.dyn.capacity * sizeof(void *));
		tfree(old);
		me->dtl.dyn.capacity *= 2;
	}
	me->dtl.dyn.array[n] = payload;
	if (n > me->dtl.dyn.length)
		me->dtl.dyn.length = n;
	return me;
}

void *
get_at(
	one_block *me,
	int n
) {
	if (me->isa != dynarray) {
		fprintf(stderr,
			"\nTXBONE error get_at: unknown or unsupported type %d %s, expected dynarray\n",
			me->isa, me->tag);
		return NULL;
	}
	if (n > me->dtl.dyn.length || n < 0) {
		fprintf(stderr,
			"\nTXBONE error get_at: index out of bounds %d not in range [0..%d]\n", n,
			me->dtl.dyn.length);
		return NULL;
	}
	return (me->dtl.dyn.array)[n];
}

/* txbone.c ends here */
