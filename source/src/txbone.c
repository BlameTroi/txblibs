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
	"accumulator list",      /* alist */
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
	if (first == NULL) return NULL;

	return first->payload;
}

static
void *
singly_get_first(
	one_singly *self
) {
	sgl_item *first = self->first;
	if (first == NULL) return NULL;

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
	if (self->first == NULL) {
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
	one_singly *self
) {
	sgl_item *curr = self->first;
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

/*
 * doubly linked list implementations. the comments on singly linked
 * list implementations apply.
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

	if (self->first == NULL) {
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
	if (self->first == NULL) return NULL;

	return self->first->payload;
}

static
void *
doubly_get_first(
	one_doubly *self
) {
	if (self->first == NULL) return NULL;

	dbl_item *first = self->first;
	self->first = first->next;

	if (first->next) first->next->previous = NULL;
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

	if (self->first == NULL) {
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
	if (self->last == NULL) return NULL;

	return self->last->payload;
}

static
void *
doubly_get_last(
	one_doubly *self
) {
	if (self->last == NULL) return NULL;

	dbl_item *last = self->last;
	self->last = last->previous;
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
	one_block *self
) {

	if (self)
		switch (self->isa) {

		case singly:
		case stack:
		case doubly:
		case queue:
		case deque:
			purge(self);
			memset(self, 253, sizeof(*self));
			tsfree(self);
			return NULL;

		case alist:
			memset(self->u.acc.list, 253, self->u.acc.capacity * sizeof(uintptr_t));
			tsfree(self->u.acc.list);
			memset(self, 253, sizeof(*self));
			tsfree(self);
			return NULL;

		case dynarray:
			memset(self->u.dyn.array, 253, self->u.dyn.capacity * sizeof(void *));
			tsfree(self->u.dyn.array);
			memset(self, 253, sizeof(*self));
			tsfree(self);
			return NULL;

		default:
			fprintf(stderr, "\nTXBONE error free_one: unknown or unsupported type %d %s\n",
				self->isa, self->tag);
			memset(self, 253, sizeof(*self));
			tsfree(self);
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
	one_block *self,
	void *payload
) {
	if (payload == NULL)
		return self;
	switch (self->isa) {

	case singly:
		singly_add_first(&self->u.sgl, payload);
		return self;

	case doubly:
		doubly_add_first(&self->u.dbl, payload);
		return self;

	default:
		fprintf(stderr, "\nTXBONE error add_first: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self,
	void *payload
) {
	if (payload == NULL)
		return self;
	switch (self->isa) {

	case singly:
		singly_add_last(&self->u.sgl, payload);
		return self;

	case doubly:
		doubly_add_last(&self->u.dbl, payload);
		return self;

	default:
		fprintf(stderr, "\nTXBONE error add_last: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case singly:
		return singly_peek_first(&self->u.sgl);

	case doubly:
		return doubly_peek_first(&self->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_first: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case singly:
		return singly_peek_last(&self->u.sgl);

	case doubly:
		return doubly_peek_last(&self->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error peek_last: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case singly:
		return singly_get_first(&self->u.sgl);

	case doubly:
		return doubly_get_first(&self->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error get_first: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case singly:
		return singly_get_last(&self->u.sgl);

	case doubly:
		return doubly_get_last(&self->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error get_last: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case singly:
		return singly_count(&self->u.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_count(&self->u.dbl);

	case alist:
		return self->u.acc.used;

	default:
		fprintf(stderr, "\nTXBONE error count: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case singly:
	case stack:
		return self->u.sgl.first == NULL;

	case doubly:
	case queue:
	case deque:
		return self->u.dbl.first == NULL;

	case alist:
		return self->u.acc.used = 0;

	default:
		fprintf(stderr, "\nTXBONE error empty: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case singly:
	case stack:
		return singly_purge(&self->u.sgl);

	case doubly:
	case queue:
	case deque:
		return doubly_purge(&self->u.dbl);

	default:
		fprintf(stderr, "\nTXBONE error purge: unknown or unsupported type %d %s\n",
			self->isa, self->tag);
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
	one_block *self,
	void *payload
) {
	switch (self->isa) {

	case stack:
		singly_add_first(&self->u.sgl, payload);
		return self;

	default:
		fprintf(stderr,
			"\nTXBONE error push: unknown or unsupported type %d %s, expected stack\n",
			self->isa, self->tag);
		return NULL;
	}
}

void *
pop(
	one_block *self
) {
	switch (self->isa) {

	case stack:
		return singly_get_first(&self->u.sgl);

	default:
		fprintf(stderr,
			"\nTXBONE error pop: unknown or unsupported type %d %s, expected stack\n",
			self->isa, self->tag);
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
	one_block *self,
	void *payload
) {
	switch (self->isa) {

	case queue:
		doubly_add_last(&self->u.dbl, payload);
		return self;

	default:
		fprintf(stderr,
			"\nTXBONE error enqueue: unknown or unsupported type %d %s, expected queue\n",
			self->isa, self->tag);
		return NULL;
	}
}

void *
dequeue(
	one_block *self
) {
	switch (self->isa) {

	case queue:
		return doubly_get_first(&self->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error dequeue: unknown or unsupported type %d %s, expected queue\n",
			self->isa, self->tag);
		return NULL;
	}
}

/*
 * peek is common to stack and queue.
 */

void *
peek(
	one_block *self
) {
	switch (self->isa) {

	case stack:
		return singly_peek_first(&self->u.sgl);

	case queue:
		return doubly_peek_first(&self->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek: unknown or unsupported type %d %s, expected stack or queue\n",
			self->isa, self->tag);
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
	one_block *self
) {
	switch (self->isa) {

	case stack:
		return singly_count(&self->u.sgl);

	default:
		fprintf(stderr,
			"\nTXBONE error depth: unknown or unsupported type %d %s, expected stack\n",
			self->isa, self->tag);
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
	one_block *self,
	void *payload
) {
	switch (self->isa) {

	case deque:
		doubly_add_first(&self->u.dbl, payload);
		return self;

	default:
		fprintf(stderr,
			"\nTXBONE error push_front: unknown or unsupported type %d %s, expected deque\n",
			self->isa, self->tag);
		return NULL;
	}
}

one_block *
push_back(
	one_block *self,
	void *payload
) {
	switch (self->isa) {

	case deque:
		doubly_add_last(&self->u.dbl, payload);
		return self;

	default:
		fprintf(stderr,
			"\nTXBONE error push_back: unknown or unsupported type %d %s, expected deque\n",
			self->isa, self->tag);
		return NULL;
	}
}

void *
pop_front(
	one_block *self
) {
	switch (self->isa) {

	case deque:
		return doubly_get_first(&self->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error pop_front: unknown or unsupported type %d %s, expected deque\n",
			self->isa, self->tag);
		return NULL;
	}
}

void *
pop_back(
	one_block *seslf
) {
	switch (seslf->isa) {

	case deque:
		return doubly_get_last(&seslf->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error pop_back: unknown or unsupported type %d %s, expected deque\n",
			seslf->isa, seslf->tag);
		return NULL;
	}
}

void *
peek_front(
	one_block *self
) {
	switch (self->isa) {

	case deque:
		return doubly_peek_first(&self->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_front: unknown or unsupported type %d %s, expected deque\n",
			self->isa, self->tag);
		return NULL;
	}
}

void *
peek_back(
	one_block *self
) {
	switch (self->isa) {

	case deque:
		return doubly_peek_last(&self->u.dbl);

	default:
		fprintf(stderr,
			"\nTXBONE error peek_back: unknown or unsupported type %d %s, expected deque\n",
			self->isa, self->tag);
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
	one_block *self
) {
	if (self->isa != dynarray) {
		fprintf(stderr,
			"\nTXBONE error high_index: unknown or unsupported type %d %s, expected dynarray\n",
			self->isa, self->tag);
		return -1;
	}
	return self->u.dyn.length;
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
	one_block *self,
	void *payload,
	int n
) {
	if (self->isa != dynarray) {
		fprintf(stderr,
			"\nTXBONE error put_at: unknown or unsupported type %d %s, expected dynarray\n",
			self->isa, self->tag);
		return NULL;
	}
	if (n < 0) {
		fprintf(stderr,
			"\nTXBONE error put_at: index may not be negative %d\n", n);
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
	one_block *self,
	int n
) {
	if (self->isa != dynarray) {
		fprintf(stderr,
			"\nTXBONE error get_at: unknown or unsupported type %d %s, expected dynarray\n",
			self->isa, self->tag);
		return NULL;
	}
	if (n > self->u.dyn.length || n < 0) {
		fprintf(stderr,
			"\nTXBONE error get_at: index out of bounds %d not in range [0..%d]\n", n,
			self->u.dyn.length);
		return NULL;
	}
	return (self->u.dyn.array)[n];
}


/*
 * accumulator list specific
 */

/*
 * add something that fits in a unintprt_t (currently 8 bytes) to the
 * end of the array list.
 */

one_block *
cons_to_alist(
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
 * an iterator of sorts over the array list. call with repeatedly,
 * *index is updated. reaching the end of the array list is signalled
 * by *index == -1.
 *
 * it's just as easy to use array access, but that won't survive a
 * change in the underlying storage approach.
 */

uintptr_t
iterate_alist(one_block *xs, int *curr) {
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

/*
 * creates a shallow copy of an array list.
 */

one_block *
clone_alist(
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

one_block *
append_to_alist(one_block *xs, one_block *ys) {

	/* be rational if list to append is empty */
	if (!ys || ys->u.acc.used < 1)
		return xs;

	/* if the 'append to' list is empty, return a copy of the
	 * append list. this is consistent with the idea that only the
	 * primary list will be mutated. */
	if (xs->u.acc.used < 1) {
		free_one(xs);
		return clone_alist(ys);
	}

	/* be lazy and just use the iterator for now. we should always
	 * pass through the append loop at least once. */
	one_block *new = clone_alist(xs);
	free_one(xs);
	int index = 0;
	while (true) {
		if (index < 0) break;
		uintptr_t got = iterate_alist(ys, &index);
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

one_block *
slice_alist(
	one_block *xs,
	int from_inclusive,
	int to_exclusive
) {
	one_block *res = make_one(alist);

	/* to be explicit about this. */
	if (from_inclusive >= to_exclusive)
		return res;

	for (int i = from_inclusive; i < to_exclusive; i++)
		res = cons_to_alist(res, xs->u.acc.list[i]);
	return res;
}

/* txbone.c ends here */
