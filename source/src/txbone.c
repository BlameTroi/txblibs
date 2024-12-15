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
 * keep these in synch with the one_type enum in the client
 * header.
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
	"accumulator list",      /* alist */
	"dynamic array",         /* dynarray */
	"key:value store",       /* keyval */
	"priority queue",        /* pqueue */
	"unknowable",         /* -- end of list -- */
	NULL
};

/**
 * a singly linked list (singly) behaves as one would expect, and
 * the parameters of it functions should all be obvious. this
 * library does not expose the 'listness' or any other structural
 * information to the client. it's all pointers or pointer sized
 * objects (payloads).
 *
 * as no realloocation of the main control block are made, the
 * one_singly is passed directly to these functions.
 *
 * generally the other arguments are all what you would expect them to
 * be.
 */

static
one_singly *
singly_add_first(
	one_singly *self,
	void *payload
) {
	sgl_item *next = tsmalloc(sizeof(*next));
	memset(next, 0, sizeof(*next));
	next->payload = payload;
	next->next = self->first;
	self->first = next;
	return self;
}

static
void *
singly_peek_first(
	one_singly *self
) {
	sgl_item *first = self->first;
	return first ? first->payload : NULL;
}

static
void *
singly_get_first(
	one_singly *self
) {
	sgl_item *first = self->first;
	if (!first)
		return NULL;
	self->first = first->next;
	void *res = first->payload;
	memset(first, 253, sizeof(*first));
	tsfree(first);
	return res;
}

static
one_singly *
singly_add_last(
	one_singly *self,
	void *payload
) {
	sgl_item *next = tsmalloc(sizeof(*next));
	memset(next, 0, sizeof(*next));
	next->payload = payload;

	/* empty list is dead simple */
	if (!self->first) {
		self->first = next;
		return self;
	}

	/* find current end and link */
	sgl_item *curr = self->first;
	while (curr->next)
		curr = curr->next;
	curr->next = next;
	return self;
}

static
void *
singly_peek_last(
	one_singly *self
) {
	sgl_item *curr = self->first;
	if (!curr)
		return NULL;

	/* chase to end. */
	while (curr->next)
		curr = curr->next;

	/* and payload back */
	return curr->payload;
}

static
void *
singly_get_last(
	one_singly *self
) {
	sgl_item *curr = self->first;
	if (!curr)
		return NULL;

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
		self->first = NULL;

	/* extract payload, clear and free old item */
	void *res = curr->payload;
	memset(curr, 253, sizeof(*curr));
	tsfree(curr);
	return res;
}

static
int
singly_count(
	one_singly *self
) {
	int count = 0;
	sgl_item *curr = self->first;
	while (curr) {
		count += 1;
		curr = curr->next;
	}
	return count;
}

static
int
singly_purge(
	one_singly *self
) {
	int count = 0;
	sgl_item *curr = self->first;
	self->first = NULL;
	while (curr) {
		count += 1;
		sgl_item *del = curr;
		curr = curr->next;
		memset(del, 253, sizeof(*del));
		tsfree(del);
	}
	return count;
}

/**
 * a doubly linked list (doubly) is the typical doubly linked list.
 * the comments for the singly linked list list implementations apply.
 */

static
one_doubly *
doubly_add_first(
	one_doubly *self,
	void *payload
) {
	dbl_item *first = tsmalloc(sizeof(*first));
	memset(first, 0, sizeof(*first));
	first->payload = payload;
	first->next = self->first;
	first->previous = NULL;

	/* empty list, easy peasy */
	if (!self->first) {
		self->first = first;
		self->last = first;
		return self;
	}

	self->first->previous = first;
	self->first = first;
	return self;
}

static
void *
doubly_peek_first(
	one_doubly *self
) {
	return self->first ? self->first->payload : NULL;
}

static
void *
doubly_get_first(
	one_doubly *self
) {
	if (!self->first)
		return NULL;

	dbl_item *first = self->first;
	self->first = first->next;

	if (first->next)
		first->next->previous = NULL;
	else self->last = NULL;

	void *res = first->payload;
	memset(first, 253, sizeof(*first));
	tsfree(first);
	return res;
}

static
one_doubly *
doubly_add_last(
	one_doubly *self,
	void *payload
) {
	dbl_item *last = tsmalloc(sizeof(*last));
	memset(last, 0, sizeof(*last));
	last->payload = payload;
	last->previous = self->last;
	last->next = NULL;

	/* empty list is easy peasy */
	if (!self->first) {
		self->first = last;
		self->last = last;
		return self;
	}

	self->last->next = last;
	self->last = last;
	return self;
}

static
void *
doubly_peek_last(
	one_doubly *self
) {
	return self->last ? self->last->payload : NULL;
}

static
void *
doubly_get_last(
	one_doubly *self
) {
	if (!self->last)
		return NULL;

	dbl_item *last = self->last;
	self->last = last->previous;

	/* watch for the only item on the list */
	if (last->previous)
		last->previous->next = NULL;
	else {
		self->first = NULL;
		self->last = NULL;
	}

	void *res = last->payload;
	memset(last, 253, sizeof(*last));
	tsfree(last);
	return res;
}

static
int
doubly_count(
	one_doubly *self
) {
	int count = 0;
	dbl_item *curr = self->first;
	while (curr) {
		count += 1;
		curr = curr->next;
	}
	return count;
}

static
int
doubly_purge(
	one_doubly *self
) {
	int count = 0;
	dbl_item *curr = self->first;
	self->first = NULL;
	self->last = NULL;
	while (curr) {
		count += 1;
		dbl_item *del = curr;
		curr = curr->next;
		memset(del, 253, sizeof(*del));
		tsfree(del);
	}
	return count;
}

/**
 * the accumulator list (alist) is a cross between a java array list and a
 * lisp or sml list. while it has some similarities to the dynamic array
 * i kept the implementations separate to avoid special subtype specific
 * conditionals from cruding up the code.
 *
 * the motivation for the alist as that it provide the functionality of
 * lists in recursions.
 *
 * as the list is stored separately from it's controlling self block,
 * the external api passes the whole one_block.
 *
 * some operations (slice) create copies of a portion of the alist. in
 * a java array list, this would just be a view over the array list.
 *
 * the api convention is the returned list is meant to replace the
 * primary list in arguments. if the list has been significantly
 * mutated, the original primary list is freed and an updated
 * copy is returned.
 *
 * always use the array list you get back from an api call, not the
 * one you sent in.
 *
 * repeat: ALWAYS USE THE ARRAY LIST YOU GET BACK FROM AN API CALL,
 * NOT THE ONE YOU SENT IN.
 *
 * for illustration, here is an example where consing repeatedly
 * will force the alist to grow, creating a new list and destroying
 * the old list. the default allocation is ALIST_DEFAULT_CAPACITY.
 *
 * one_block *xs = make_one(alist);
 * one_block *remember = xs;
 * for (int i = 0; i > ALIST_DEFAULT_CAP * 2; i++)
 *         xs = cons_to_alist(xs, (uintptr_t)i);
 * assert(remember != xs);
 * xs = free_one(xs);
 * assert(xs == NULL);
 *
 * 'remember' holds the original address of 'xs', but that storage was
 * released when 'xs' was expanded.
 *
 * some of the api is inspired by the java arraylist, and the rest
 * from lisp. it's pseudo lisp lists with fake garbage collection.
 */

static
int
alist_purge(
	one_block *xs
) {
	int ret = xs->u.acc.used;
	xs->u.acc.used = 0;
	memset(xs->u.acc.list, 0, ret * sizeof(uintptr_t));
	return ret;
}

/*
 * add something that fits in a unintprt_t (currently 8 bytes) to the
 * end of the array list.
 */

static
one_block *
alist_cons(
	one_block *xs,
	uintptr_t p
) {
	if (xs->u.acc.used == xs->u.acc.capacity) {
		int lena = xs->u.acc.capacity * sizeof(uintptr_t);
		one_block *new = tsmalloc(sizeof(*xs));
		memcpy(new, xs, sizeof(*xs));
		uintptr_t *acc = tsmalloc(lena * 2);
		memset(acc, 0, lena * 2);
		memcpy(acc, xs->u.acc.list, lena);
		new->u.acc.capacity = xs->u.acc.capacity * 2;
		new->u.acc.list = acc;
		free_one(xs);
		xs = new;
	}
	xs->u.acc.list[xs->u.acc.used] = p;
	xs->u.acc.used += 1;
	return xs;
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

static
one_block *
alist_slice(
	one_block *xs,
	int from_inclusive,
	int to_exclusive
) {
	if (to_exclusive > xs->u.acc.used || from_inclusive < 0) {
		fprintf(stderr,
			"\nERROR txbone-slice: range out of bounds holds [0..%d) requested [%d..%d)\n",
			xs->u.acc.used, from_inclusive, to_exclusive);
		return NULL;
	}
	one_block *res = make_one(alist);
	/* to be explicit about this. */
	if (from_inclusive >= to_exclusive)
		return res;

	for (int i = from_inclusive; i < to_exclusive; i++)
		res = alist_cons(res, xs->u.acc.list[i]);
	return res;
}

/*
 * an iterator of sorts over the alist. call it repeatedly and *index
 * *index is updated. reaching the end of the array list is signalled
 * by *index == -1.
 */

static
uintptr_t
alist_iterate(one_block *xs, int *curr) {
	if (*curr < 0)
		return 0;
	if (*curr >= xs->u.acc.used) {
		*curr = -1;  /* as a null might be valid, use a negative count to also signal end */
		return 0;
	}
	uintptr_t res = xs->u.acc.list[*curr];
	*curr += 1;
	return res;
}

static
one_block *
alist_cdr(one_block *xs) {
	return alist_slice(xs, 1, xs->u.acc.used);;
}

/*
 * creates a shallow copy of an array list.
 */

static
one_block *
alist_clone(
	one_block *xs
) {
	int lenu = sizeof(*xs);
	one_block *resu = tsmalloc(lenu);
	memset(resu, 0, lenu);
	memcpy(resu, xs, lenu);

	int lena = xs->u.acc.capacity * sizeof(uintptr_t);
	memcpy(resu->u.acc.list, xs->u.acc.list, lena);
	resu->u.acc.list = tsmalloc(lena);
	memset(resu->u.acc.list, 0, lena);
	memcpy(resu->u.acc.list, xs->u.acc.list, lena);
	return resu;
}

/*
 * append one array list to the end of another.
 */

static
one_block *
alist_append(one_block *xs, one_block *ys) {

	/* be rational if list to append is empty */
	if (!ys || ys->u.acc.used < 1)
		return xs;

	/* if the 'append to' list is empty, return a copy of the
	 * append list. this is consistent with the idea that only the
	 * primary list will be mutated. */
	if (xs->u.acc.used < 1) {
		free_one(xs);
		return alist_clone(ys);
	}

	/* be lazy and just use the iterator for now. we should always
	 * pass through the append loop at least once. */
	one_block *new = alist_clone(xs);
	free_one(xs);
	int index = 0;
	while (true) {
		if (index < 0) break;
		uintptr_t got = alist_iterate(ys, &index);
		new = alist_cons(new, got);
	}

	return new;
}

/*
 * return the first element in the alist.
 */

static
uintptr_t
alist_car(
	one_block *xs
) {
	if (xs->u.acc.used > 0)
		return xs->u.acc.list[0];
	else return 0;
}

/*
 * return the nth element in the alist.
 */

static
uintptr_t
alist_nth(
	one_block *xs,
	int n
) {
	if (n >= xs->u.acc.used || n < 0)
		return 0;
	return xs->u.acc.list[n];
}

/*
 * set the nth element in the alist.
 */

static
bool
alist_setnth(
	one_block *xs,
	int n,
	uintptr_t atom
) {
	if (n >= xs->u.acc.used || n < 0)
		return false;
	xs->u.acc.list[n] = atom;
	return true;
}


/**
 * the unified or generic api.
 *
 * create, destroy, and functions global to all data structures. all
 * entry points other than make_one and free_one tend to delegate to
 * other functions that don't have external scope.
 *
 * conventions:
 *
 * functions that don't really need to return a payload or count
 * return their first argument, which allows for chaining calls and
 * (in the case of an alist) the replacement of one copy of the
 * structure with a new updated copy.
 *
 * functions that return an integer (count) return -1 for any error.
 * functions that return the one_block will return a NULL for any
 * error.
 *
 * of course 'read' functions will return NULL if there is nothing to
 * return.
 *
 * the `one_block` holds all the various subtype control blocks in a
 * union. this is the instance of the datatype in question
 *
 * generally, the whole `one_block` is not passed to detailed
 * implementations. all of these datatypes started out as separate
 * libraries. as most of those libraries do not create a new control
 * block in flight, the union member is passed to lower level
 * functions instead of the whole one block.
 *
 * the noteable exception to this is the alist, which can create and
 * dispose of the alist control block in flight. there the whole
 * `one_block` is passed.
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

	case alist:
		ob->u.acc.used = 0;
		ob->u.acc.capacity = ALIST_DEFAULT_CAPACITY;
		ob->u.acc.list = tsmalloc(ALIST_DEFAULT_CAPACITY * sizeof(uintptr_t));
		memset(ob->u.acc.list, 0, ob->u.acc.capacity * sizeof(uintptr_t));
		return ob;

	case dynarray:
		ob->u.dyn.length = -1;
		ob->u.dyn.capacity = DYNARRAY_DEFAULT_CAPACITY;
		ob->u.dyn.array = tsmalloc(DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
		memset(ob->u.dyn.array, 0, DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-make_one: unknown or not yet implemented type %d %s\n",
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
	one_block *ob
) {

	if (ob)
		switch (ob->isa) {

		case singly:
		case stack:
		case doubly:
		case queue:
		case deque:
			purge(ob);
			memset(ob, 253, sizeof(*ob));
			tsfree(ob);
			return NULL;

		case alist:
			memset(ob->u.acc.list, 253, ob->u.acc.capacity * sizeof(uintptr_t));
			tsfree(ob->u.acc.list);
			memset(ob, 253, sizeof(*ob));
			tsfree(ob);
			return NULL;

		case dynarray:
			memset(ob->u.dyn.array, 253, ob->u.dyn.capacity * sizeof(void *));
			tsfree(ob->u.dyn.array);
			memset(ob, 253, sizeof(*ob));
			tsfree(ob);
			return NULL;

		default:
			fprintf(stderr, "\nERROR txbone-free_one: unknown or unsupported type %d %s\n",
				ob->isa, ob->tag);
			memset(ob, 253, sizeof(*ob));
			tsfree(ob);
			return NULL;
		}

	fprintf(stderr, "\nERROR txbone-free_one: called with NULL one block\n");
	return NULL;
}



/**
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
	one_block *ob,
	void *payload
) {
	if (payload == NULL)
		return ob;
	switch (ob->isa) {

	case singly:
		singly_add_first(&ob->u.sgl, payload);
		return ob;

	case doubly:
		doubly_add_first(&ob->u.dbl, payload);
		return ob;

	default:
		fprintf(stderr, "\nERROR txbone-add_first: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
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
	one_block *ob,
	void *payload
) {
	if (payload == NULL)
		return ob;
	switch (ob->isa) {

	case singly:
		singly_add_last(&ob->u.sgl, payload);
		return ob;

	case doubly:
		doubly_add_last(&ob->u.dbl, payload);
		return ob;

	default:
		fprintf(stderr, "\nERROR txbone-add_last: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
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
	one_block *ob
) {
	switch (ob->isa) {

	case singly:
		return singly_peek_first(&ob->u.sgl);

	case doubly:
		return doubly_peek_first(&ob->u.dbl);

	default:
		fprintf(stderr,
			"\nERROR txbone-peek_first: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
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
	one_block *ob
) {
	switch (ob->isa) {

	case singly:
		return singly_peek_last(&ob->u.sgl);

	case doubly:
		return doubly_peek_last(&ob->u.dbl);

	default:
		fprintf(stderr, "\nERROR txbone-peek_last: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
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
	one_block *ob
) {
	switch (ob->isa) {

	case singly:
		return singly_get_first(&ob->u.sgl);

	case doubly:
		return doubly_get_first(&ob->u.dbl);

	default:
		fprintf(stderr, "\nERROR txbone-get_first: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
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
	one_block *ob
) {
	switch (ob->isa) {

	case singly:
		return singly_get_last(&ob->u.sgl);

	case doubly:
		return doubly_get_last(&ob->u.dbl);

	default:
		fprintf(stderr, "\nERROR txbone-get_last: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
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
	one_block *ob
) {
	switch (ob->isa) {

	case singly:
		return singly_count(&ob->u.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_count(&ob->u.dbl);

	case alist:
		return ob->u.acc.used;

	default:
		fprintf(stderr, "\nERROR txbone-count: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return -1;
	}
}

/**
 * is_empty
 *
 * predicate is this data structure empty (count/depth == 0)?
 *
 *     in: the instance
 *
 * return: boolean, any errors come back as false
 */

bool
is_empty(
	one_block *ob
) {
	switch (ob->isa) {

	case singly:
	case stack:
		return ob->u.sgl.first == NULL;

	case doubly:
	case queue:
	case deque:
		return ob->u.dbl.first == NULL;

	case alist:
		return ob->u.acc.used = 0;

	default:
		fprintf(stderr, "\nERROR txbone-empty: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return false;
	}
}

/**
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
	one_block *ob
) {
	switch (ob->isa) {

	case singly:
	case stack:
		return singly_purge(&ob->u.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_purge(&ob->u.dbl);

	case alist:
		return alist_purge(ob);

	default:
		fprintf(stderr, "\nERROR txbone-purge: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return -1;
	}
}

/**
 * a stack is implemented on a singly linked list, but use the
 * following entry points in addition to make_one, free_one, is_empty,
 * peek, and purge.
 */

/**
 * depth
 *
 * stacks don't have counts, they have depth.
 */

int
depth(
	one_block *ob
) {
	switch (ob->isa) {

	case stack:
		return singly_count(&ob->u.sgl);

	default:
		fprintf(stderr,
			"\nERROR txbone-depth: unknown or unsupported type %d %s, expected stack\n",
			ob->isa, ob->tag);
		return -1;
	}
}

one_block *
push(
	one_block *ob,
	void *payload
) {
	switch (ob->isa) {

	case stack:
		singly_add_first(&ob->u.sgl, payload);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-push: unknown or unsupported type %d %s, expected stack\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

void *
pop(
	one_block *ob
) {
	switch (ob->isa) {

	case stack:
		return singly_get_first(&ob->u.sgl);

	default:
		fprintf(stderr,
			"\nERROR txbone-pop: unknown or unsupported type %d %s, expected stack\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * a queue (fifo) is implemented on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, is_empty,
 * count, peek, and purge.
 */

one_block *
enqueue(
	one_block *ob,
	void *payload
) {
	switch (ob->isa) {

	case queue:
		doubly_add_last(&ob->u.dbl, payload);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-enqueue: unknown or unsupported type %d %s, expected queue\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

void *
dequeue(
	one_block *ob
) {
	switch (ob->isa) {

	case queue:
		return doubly_get_first(&ob->u.dbl);

	default:
		fprintf(stderr,
			"\nERROR txbone-dequeue: unknown or unsupported type %d %s, expected queue\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * peek is common to stack and queue.
 */

void *
peek(
	one_block *ob
) {
	switch (ob->isa) {

	case stack:
		return singly_peek_first(&ob->u.sgl);

	case queue:
		return doubly_peek_first(&ob->u.dbl);

	default:
		fprintf(stderr,
			"\nERROR txbone-peek: unknown or unsupported type %d %s, expected stack or queue\n",
			ob->isa, ob->tag);
		return NULL;
	}
}


/**
 * a deque (f/l-ifo)is built on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * count, and purge.
 */

one_block *
push_front(
	one_block *ob,
	void *payload
) {
	switch (ob->isa) {

	case deque:
		doubly_add_first(&ob->u.dbl, payload);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-push_front: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

one_block *
push_back(
	one_block *ob,
	void *payload
) {
	switch (ob->isa) {

	case deque:
		doubly_add_last(&ob->u.dbl, payload);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-push_back: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

void *
pop_front(
	one_block *ob
) {
	switch (ob->isa) {

	case deque:
		return doubly_get_first(&ob->u.dbl);

	default:
		fprintf(stderr,
			"\nERROR txbone-pop_front: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

void *
pop_back(
	one_block *ob
) {
	switch (ob->isa) {

	case deque:
		return doubly_get_last(&ob->u.dbl);

	default:
		fprintf(stderr,
			"\nERROR txbone-pop_back: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

void *
peek_front(
	one_block *ob
) {
	switch (ob->isa) {

	case deque:
		return doubly_peek_first(&ob->u.dbl);

	default:
		fprintf(stderr,
			"\nERROR txbone-peek_front: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

void *
peek_back(
	one_block *ob
) {
	switch (ob->isa) {

	case deque:
		return doubly_peek_last(&ob->u.dbl);

	default:
		fprintf(stderr,
			"\nERROR txbone-peek_back: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * dynamic arrays are self expanding arrays. in addition to make and
 * free, they support hbound via high_index, get, and put.
 */

/**
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
	one_block *self
) {
	if (self->isa != dynarray) {
		fprintf(stderr,
			"\nERROR txbone-high_index: unknown or unsupported type %d %s, expected dynarray\n",
			self->isa, self->tag);
		return -1;
	}
	return self->u.dyn.length;
}

/**
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
	one_block *self,
	void *payload,
	int n
) {
	if (self->isa != dynarray) {
		fprintf(stderr,
			"\nERROR txbone-put_at: unknown or unsupported type %d %s, expected dynarray\n",
			self->isa, self->tag);
		return NULL;
	}
	if (n < 0) {
		fprintf(stderr,
			"\nERROR txbone-put_at: index may not be negative %d\n", n);
		return NULL;
	}
	while (n >= self->u.dyn.capacity) {
		void *old = self->u.dyn.array;
		self->u.dyn.array = tsmalloc(2 * self->u.dyn.capacity * sizeof(void *));
		memset(self->u.dyn.array, 0, 2 * self->u.dyn.capacity * sizeof(void *));
		memcpy(self->u.dyn.array, old, self->u.dyn.capacity * sizeof(void *));
		memset(old, 253, self->u.dyn.capacity * sizeof(void *));
		tsfree(old);
		self->u.dyn.capacity *= 2;
	}
	self->u.dyn.array[n] = payload;
	if (n > self->u.dyn.length)
		self->u.dyn.length = n;
	return self;
}

/**
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
	one_block *self,
	int n
) {
	if (self->isa != dynarray) {
		fprintf(stderr,
			"\nERROR txbone-get_at: unknown or unsupported type %d %s, expected dynarray\n",
			self->isa, self->tag);
		return NULL;
	}
	if (n > self->u.dyn.length || n < 0) {
		fprintf(stderr,
			"\nERROR txbone-get_at: index out of bounds %d not in range [0..%d]\n", n,
			self->u.dyn.length);
		return NULL;
	}
	return (self->u.dyn.array)[n];
}

/**
 * functions specific to accumulator lists (alists). the naming is somewhat
 * lisp inspired.
 */

one_block *
cons(
	one_block *ob,
	uintptr_t atom
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-cons: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_cons(ob, atom);
}

uintptr_t
car(
	one_block *ob
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-car: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_car(ob);
}

one_block *
cdr(
	one_block *ob
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-cdr: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_cdr(ob);
}

one_block *
append(
	one_block *left,
	one_block *right
) {
	if (left->isa != alist) {
		fprintf(stderr, "\nERROR txbone-append: unknown or unsupported type %d %s\n",
			left->isa, left->tag);
	}
	if (right->isa != alist) {
		fprintf(stderr, "\nERROR txbone-append: unknown or unsupported type %d %s\n",
			right->isa, right->tag);
	}
	return alist_append(left, right);
}

one_block *
slice(
	one_block *ob,
	int from_inclusive,
	int to_exclusive
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-slice: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_slice(ob, from_inclusive, to_exclusive);
}

bool
setnth(
	one_block *ob,
	int n,
	uintptr_t atom
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-setnth: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_setnth(ob, n, atom);
}

uintptr_t
nth(
	one_block *ob,
	int n
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-nth: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_nth(ob, n);
}

one_block *
clone(
	one_block *ob
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-clone: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_clone(ob);
}

uintptr_t
iterate(
	one_block *ob,
	int *curr
) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-iterate: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_iterate(ob, curr);
}

/* txbone.c ends here */
