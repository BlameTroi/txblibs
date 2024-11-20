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
 * supported types so far are singly and doubly linked lists, queues,
 * deques, stacks, and dynamic arrays. binary search trees, key:value
 * stores, hashes, dictionaries, and bags, are planned.
 *
 * all types store client payloads, as void * pointers. memory
 * management of client payloads are the responsibility of the client.
 *
 * errors return invalid values (negatives or NULLs, see each
 * function) and can print a diagnostic message on stderr.
 */

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
 * singly linked list implementations. these are called to service
 * external requests. see those functions for more detailed
 * documentation.
 *
 * generally the arguments are all what you would expect them to be.
 */

static
one_singly *
singly_add_first(
	one_singly *me,
	void *payload
) {
	sgl_item *next = tsmalloc(sizeof(*next));
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
	tsfree(first);
	return res;
}

static
one_singly *
singly_add_last(
	one_singly *me,
	void *payload
) {
	sgl_item *next = tsmalloc(sizeof(*next));
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
	tsfree(curr);
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
		tsfree(del);
	}
	return count;
}

/*
 * doubly linked list implementations. the comments on singly linked
 * list implementations apply.
 */

static
one_doubly *
doubly_add_first(
	one_doubly *me,
	void *payload
) {
	dbl_item *first = tsmalloc(sizeof(*first));
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
	tsfree(first);
	return res;
}

static
one_doubly *
doubly_add_last(
	one_doubly *me,
	void *payload
) {
	dbl_item *last = tsmalloc(sizeof(*last));
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
	tsfree(last);
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
		tsfree(del);
	}
	return count;
}

/*
 * generic entries, most of these route control to detailed
 * implementations for each advanced data type.
 *
 * create, destroy, and functions global to all data structures. all
 * entry points other than make_one and free_one tend to delegate to
 * other functions that don't have external scope.
 *
 * conventions:
 *
 * functions that don't really need to return a payload or count
 * return their first argument, which allows for chaining calls.
 *
 * functions that return an integer (count) return -1 for any error.
 * functions that return the one_block will return a NULL for any
 * error.
 *
 * of course 'read' functions will return NULL if there is nothing to
 * return.
 */

/*
 * make_one
 *
 * create an instance of one of the data structure types. allocates and
 * initializes the 'one block' and returns it to the client. the client
 * passes this back on subsequent calls as a handle.
 *
 * a constructor, if you will.
 *
 *     in: what to instantiate, see enum one_type for values
 *
 * return: the new instance or NULL on error
 */

one_block *
make_one(
	enum one_type isa
) {
	one_block *ob = tsmalloc(sizeof(*ob));
	memset(ob, 0, sizeof(*ob));
	ob->isa = isa;
	if (isa <= ONE_TYPE_MAX && isa > 0)
		strncpy(ob->tag, one_tags[isa], ONE_TAG_LEN-1);
	else
		strncpy(ob->tag, "*invalid one type*", ONE_TAG_LEN-1);

	switch (ob->isa) {
	case singly:
	case stack:
		ob->u.sgl.first = NULL;
		return ob;

	case doubly:
	case queue:
	case deque:
		ob->u.dbl.first = NULL;
		ob->u.dbl.last = NULL;
		return ob;

	case dynarray:
		ob->u.dyn.length = -1;
		ob->u.dyn.capacity = DYNARRAY_DEFAULT_CAPACITY;
		ob->u.dyn.array = tsmalloc(DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
		memset(ob->u.dyn.array, 0, DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
		return ob;

	default:
		fprintf(stderr,
			"\nTXBONE error make_one: unknown or not yet implemented type %d %s\n",
			isa, ob->tag);
		memset(ob, 253, sizeof(*ob));
		tsfree(ob);
		return NULL;
	}
}

/*
 * free_one
 *
 * destroy an instance of a data structure, releasing library managed
 * memory.
 *
 * a destructor.
 *
 *     in: the instance to destroy
 *
 * return: the now invalid handle (pointer) to the instance or NULL on
 *         error
 */

one_block *
free_one(
	one_block *me
) {

	if (me)
		switch (me->isa) {

		case singly:
		case stack:
		case doubly:
		case queue:
		case deque:
			purge(me);
			memset(me, 253, sizeof(*me));
			tsfree(me);
			return me;

		case dynarray:
			memset(me->u.dyn.array, 253, me->u.dyn.capacity * sizeof(void *));
			tsfree(me->u.dyn.array);
			memset(me, 253, sizeof(*me));
			tsfree(me);
			return me;

		default:
			fprintf(stderr, "\nTXBONE error free_one: unknown or unsupported type %d %s\n",
				me->isa, me->tag);
			memset(me, 253, sizeof(*me));
			tsfree(me);
			return NULL;
		}

	fprintf(stderr, "\nTXBONE error free_on: called with NULL one block\n");
	return NULL;
}



/*
 * add_first
 *
 * add an item to the front/top of all items held.
 *
 *     in: the instance
 *
 *     in: void * payload
 *
 * return: the one block or NULL if error
 */

one_block *
add_first(
	one_block *me,
	void *payload
) {
	if (payload == NULL)
		return me;
	switch (me->isa) {

	case singly:
		singly_add_first(&me->u.sgl, payload);
		return me;

	case doubly:
		doubly_add_first(&me->u.dbl, payload);
		return me;

	default:
		fprintf(stderr, "\nTXBONE error add_first: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * add_last
 *
 * add an item to the back/bottom of all items held.
 *
 *     in: the instance
 *
 *     in: void * payload
 *
 * return: the one block or NULL if error
 */

one_block *
add_last(
	one_block *me,
	void *payload
) {
	if (payload == NULL)
		return me;
	switch (me->isa) {

	case singly:
		singly_add_last(&me->u.sgl, payload);
		return me;

	case doubly:
		doubly_add_last(&me->u.dbl, payload);
		return me;

	default:
		fprintf(stderr, "\nTXBONE error add_last: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * peek_first
 *
 * return but do not remove the item at the front/top of all
 * items held.
 *
 *     in: the instance
 *
 * return: the item payload or NULL on either an error or empty
 */

void *
peek_first(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_peek_first(&me->u.sgl);

	case doubly:
		return doubly_peek_first(&me->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_first: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * peek_last
 *
 * return but do not remove the item at the back/bottom of all
 * items held.
 *
 *     in: the instance
 *
 * return: the item payload or NULL on either an error or empty
 */

void *
peek_last(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_peek_last(&me->u.sgl);

	case doubly:
		return doubly_peek_last(&me->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error peek_last: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * get_first
 *
 * remove and return the item at the front/top of all items held.
 *
 *     in: the instance
 *
 * return: the item payload or NULL on either an error or empty
 */

void *
get_first(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_get_first(&me->u.sgl);

	case doubly:
		return doubly_get_first(&me->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error get_first: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * get_last
 *
 * remove and return the item at the back/bottom of all items held.
 *
 *     in: the instance
 *
 * return: the item payload or NULL on either an error or empty
 */

void *
get_last(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_get_last(&me->u.sgl);

	case doubly:
		return doubly_get_last(&me->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error get_last: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * count
 *
 * how many things are managed by the data structure. for a stack, use
 * depth. has no meaning for a dynamic array.
 *
 *      in: the instance
 *
 * return: integer number of items or -1 on error
 */

int
count(
	one_block *me
) {
	switch (me->isa) {

	case singly:
		return singly_count(&me->u.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_count(&me->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error count: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return -1;
	}
}

/*
 * empty
 *
 * predicate is this data structure empty (count/depth == 0)?
 *
 *     in: the instance
 *
 * return: boolean, any errors come back as false
 */

bool
empty(
	one_block *me
) {
	switch (me->isa) {

	case singly:
	case stack:
		return me->u.sgl.first == NULL;

	case doubly:
	case queue:
	case deque:
		return me->u.dbl.first == NULL;

	default:
		fprintf(stderr, "\nTXBONE error empty: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return false;
	}
}

/*
 * purge
 *
 * empty the data structure. deletes all storage for items/nodes
 * managed by the structure. client data is left alone. this has no
 * meaning for a dynamic array.
 *
 *     in: the instance
 *
 * return: integer how many things were purged or -1 on error
 */

int
purge(
	one_block *me
) {
	switch (me->isa) {

	case singly:
	case stack:
		return singly_purge(&me->u.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_purge(&me->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error purge: unknown or unsupported type %d %s\n",
			me->isa, me->tag);
		return -1;
	}
}

/*
 * a stack is implemented on a singly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * peek, and purge.
 */

one_block *
push(
	one_block *me,
	void *payload
) {
	switch (me->isa) {

	case stack:
		singly_add_first(&me->u.sgl, payload);
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
		return singly_get_first(&me->u.sgl);

	default:
		fprintf(stderr,
			"\nTXBONE error pop: unknown or unsupported type %d %s, expected stack\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * a queue (fifo) is implemented on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * count, peek, and purge.
 */

one_block *
enqueue(
	one_block *me,
	void *payload
) {
	switch (me->isa) {

	case queue:
		doubly_add_last(&me->u.dbl, payload);
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
		return doubly_get_first(&me->u.dbl);

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
		return singly_peek_first(&me->u.sgl);

	case queue:
		return doubly_peek_first(&me->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek: unknown or unsupported type %d %s, expected stack or queue\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * depth
 *
 * stacks don't have counts, they have depth.
 */

int
depth(
	one_block *me
) {
	switch (me->isa) {

	case stack:
		return singly_count(&me->u.sgl);

	default:
		fprintf(stderr,
			"\nTXBONE error depth: unknown or unsupported type %d %s, expected stack\n",
			me->isa, me->tag);
		return -1;
	}
}

/*
 * a deque (f/l-ifo)is built on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * count, and purge.
 */

one_block *
push_front(
	one_block *me,
	void *payload
) {
	switch (me->isa) {

	case deque:
		doubly_add_first(&me->u.dbl, payload);
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
		doubly_add_last(&me->u.dbl, payload);
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
		return doubly_get_first(&me->u.dbl);

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
		return doubly_get_last(&me->u.dbl);

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
		return doubly_peek_first(&me->u.dbl);

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
		return doubly_peek_last(&me->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_back: unknown or unsupported type %d %s, expected deque\n",
			me->isa, me->tag);
		return NULL;
	}
}

/*
 * dynamic arrays are self expanding arrays. in addition to make and
 * free, they support hbound via high_index, get, and put. TODO: sort
 * and func for sort.
 */

/*
 * high_index
 *
 * the highest used (via put_at) index in the array. while a payload
 * may be put anywhere with a non-negative index, a get is only
 * valid for an index in the range 0->high index.
 *
 *     in: the instance
 *
 * return: integer index or -1 on error.
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
	return me->u.dyn.length;
}

/*
 * put_at
 *
 * place a payload at a particular index in the array. if the array's
 * capacity is less than the index, double the capacity until the
 * index is valid.
 *
 *     in: the instance
 *
 *     in: the payload to store
 *
 *     in: integer index to store the payload at
 *
 * return: the one block or NULL on error
 */

one_block *
put_at(
	one_block *me,
	void *payload,
	int n
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
	while (n >= me->u.dyn.capacity) {
		void *old = me->u.dyn.array;
		me->u.dyn.array = tsmalloc(2 * me->u.dyn.capacity * sizeof(void *));
		memset(me->u.dyn.array, 0, 2 * me->u.dyn.capacity * sizeof(void *));
		memcpy(me->u.dyn.array, old, me->u.dyn.capacity * sizeof(void *));
		memset(old, 253, me->u.dyn.capacity * sizeof(void *));
		tsfree(old);
		me->u.dyn.capacity *= 2;
	}
	me->u.dyn.array[n] = payload;
	if (n > me->u.dyn.length)
		me->u.dyn.length = n;
	return me;
}

/*
 * get_from
 *
 * return the payload from a particular index in the array. if the index
 * is either negative or greater than high_index, it is an error. if
 * the index is between 0 .. high_index (inclusive) but nothing has been
 * put_at that index yet, return NULL.
 *
 *     in: the instance
 *
 *     in: integer index to retrieve payload from
 *
 * return: the payload or NULL on error, but note that NULL could also
 *         be the payload.
 */

void *
get_from(
	one_block *me,
	int n
) {
	if (me->isa != dynarray) {
		fprintf(stderr,
			"\nTXBONE error get_at: unknown or unsupported type %d %s, expected dynarray\n",
			me->isa, me->tag);
		return NULL;
	}
	if (n > me->u.dyn.length || n < 0) {
		fprintf(stderr,
			"\nTXBONE error get_at: index out of bounds %d not in range [0..%d]\n", n,
			me->u.dyn.length);
		return NULL;
	}
	return (me->u.dyn.array)[n];
}

/* txbone.c ends here */
