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

/* a helpful macro for infrequently needed traces */
// #define FPRINTF_INFO
#ifndef FPRINTF_INFO
#define FPRINTF_INFO if (false)
#endif

/**
 * this is a copy paste from my txblog2 header, which implements a
 * fast integer log base 2 function for 32 bit integers. i changed the
 * function name slightly and made it static so there won't be a
 * problem if txblog2.h is included.
 *
 * this originally comes from:
 *
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
 *
 * but i've tweaked some naming and formatting. i don't claim the
 * algorithm, just this particular implementation, which as always
 * i have --
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 *
 * implementation notes:
 *
 * i had to make a few changes, after which i felt free to do some
 * reformatting of the original code to match my sense of esthetics.
 */

#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n

static const uint8_t
log_table_256[256] = {
	-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	LT(4),
	LT(5),
	LT(5),
	LT(6),
	LT(6),
	LT(6),
	LT(6),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7)
};

static
uint32_t
u32_log2(uint32_t v) {
	uint32_t r;
	register uint32_t t, tt;
	if ((tt = v >> 16))
		r = (t = tt >> 8) ? 24 + log_table_256[t] : 16 + log_table_256[tt];
	else
		r = (t = v >> 8) ? 8 + log_table_256[t] : log_table_256[v];
	return r;
}

/**
 * supported types so far are singly and doubly linked lists, queues,
 * deques, stacks, dynamic arrays, key:value stores (on binary search
 * trees), and priority queues. as need arises, hashes, dictionaries,
 * bags, and sets could be added, but i think the key:value store will
 * fill most of those needs.
 *
 * all types store client data as pointer sized items, either `void *`
 * or uintptr_t. management of client payloads are the responsibility
 * of the client.
 *
 * errors return invalid values (negatives or NULLs, see each
 * function) and can print a diagnostic message on stderr.
 */

/*
 * keep these in synch with the one_type enum in the client
 * header.
 */

static const char *
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
 * difficult to avoid forward references
 *
 * these should be all of the internal functions and they are
 * meant to be static.
 */

static
void
btree_node_free(one_tree *, one_node *);

static
int
btree_node_children_free(one_tree *, one_node *);

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
singly_add_first(one_singly *self, void *item) {
	sgl_item *next = tsmalloc(sizeof(*next));
	memset(next, 0, sizeof(*next));
	next->item = item;
	next->next = self->first;
	self->first = next;
	return self;
}

static
void *
singly_peek_first(one_singly *self) {
	sgl_item *first = self->first;
	return first ? first->item : NULL;
}

static
void *
singly_get_first(one_singly *self) {
	sgl_item *first = self->first;
	if (!first)
		return NULL;
	self->first = first->next;
	void *res = first->item;
	memset(first, 253, sizeof(*first));
	tsfree(first);
	return res;
}

static
one_singly *
singly_add_last(one_singly *self, void *item) {
	sgl_item *next = tsmalloc(sizeof(*next));
	memset(next, 0, sizeof(*next));
	next->item = item;

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
singly_peek_last(one_singly *self) {
	sgl_item *curr = self->first;
	if (!curr)
		return NULL;

	/* chase to end. */
	while (curr->next)
		curr = curr->next;

	/* and item back */
	return curr->item;
}

static
void *
singly_get_last(one_singly *self) {
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

	/* extract item, clear and free old item */
	void *res = curr->item;
	memset(curr, 253, sizeof(*curr));
	tsfree(curr);
	return res;
}

static
int
singly_count(one_singly *self) {
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
singly_purge(one_singly *self) {
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
doubly_add_first(one_doubly *self, void *item) {
	dbl_item *first = tsmalloc(sizeof(*first));
	memset(first, 0, sizeof(*first));
	first->item = item;
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
doubly_peek_first(one_doubly *self) {
	return self->first ? self->first->item : NULL;
}

static
void *
doubly_get_first(one_doubly *self) {
	if (!self->first)
		return NULL;

	dbl_item *first = self->first;
	self->first = first->next;

	if (first->next)
		first->next->previous = NULL;
	else self->last = NULL;

	void *res = first->item;
	memset(first, 253, sizeof(*first));
	tsfree(first);
	return res;
}

static
one_doubly *
doubly_add_last(one_doubly *self, void *item) {
	dbl_item *last = tsmalloc(sizeof(*last));
	memset(last, 0, sizeof(*last));
	last->item = item;
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
doubly_peek_last(one_doubly *self) {
	return self->last ? self->last->item : NULL;
}

static
void *
doubly_get_last(one_doubly *self) {
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

	void *res = last->item;
	memset(last, 253, sizeof(*last));
	tsfree(last);
	return res;
}

static
int
doubly_count(one_doubly *self) {
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
doubly_purge(one_doubly *self) {
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
alist_purge(one_block *xs) {
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
alist_cons(one_block *xs, uintptr_t p) {
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
alist_slice(one_block *xs, int from_inclusive, int to_exclusive) {
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
	if (!xs || xs->u.acc.used < 1)
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
alist_car(one_block *xs) {
	if (xs->u.acc.used > 0)
		return xs->u.acc.list[0];
	else return 0;
}

/*
 * return the nth element in the alist.
 */

static
uintptr_t
alist_nth(one_block *xs, int n) {
	uintptr_t atom = 0;
	if (n >= xs->u.acc.used || n < 0)
		fprintf(stderr,
			"\nERROR txbone-nth: index out of range %d lies outside [0..%d)\n",
			n, xs->u.acc.used);
	else
		atom =  xs->u.acc.list[n];
	return atom;
}

/*
 * set the nth element in the alist.
 */

static
one_block *
alist_setnth(one_block *xs, int n, uintptr_t atom) {
	if (n >= xs->u.acc.used || n < 0)
		fprintf(stderr,
			"\nERROR txbone-setnth: index out of range %d lies outside [0..%d)\n",
			n, xs->u.acc.used);
	else
		xs->u.acc.list[n] = atom;
	return xs;
}

/**
 * key comparison
 */

/*
 * some structures use keys to uniquely identify an item. so far
 * the externally visible structure is the key:value store, which
 * is internally a binary search tree.
 *
 * as keys and values are most usually passed as `void *` pointers, the
 * client must let us know how to compare the keys. the comparator
 * function has the same basic interface as the standard `strcmp`.
 *
 * the key's type and comparator function are set when a structure is
 * created via `make_one_keyed()`.
 *
 * integral and string compares are set automatically to either
 * `integral_comp` or `strcmp`.
 *
 * argument 'left' tends to be searched for key, and argument 'right'
 * tends to be the key of the node being checked against.
 */

static
int
integral_comp(void *left, void *right) {
	return (long)left - (long)right;
}

/*
 * the key comparison happens often enough that it was worth factoring
 * out. going with three discreet values instead of 'less than/greather
 * than zero' is a peersonal preference.
 */

enum keycmp_result {
	kcr_unknown = 0,                /* always have an invalid value  */
	LESS,
	EQUAL,
	GREATER
};
typedef enum keycmp_result keycmp_result;

static
keycmp_result
keycmp(one_tree *self, void *left, void *right) {
	int cmp = self->fn_cmp(left, right);
	if (cmp < 0) return LESS;
	else if (cmp > 0) return GREATER;
	else return EQUAL;
}


/*
 * free memory for the tree and its Nodes. returns the now invalid
 * pointer to the old tree.
 */

one_tree *
btree_free(one_tree *self) {
	int freed = 0;
	if (self->root) {
		freed += btree_node_children_free(self, self->root);
		btree_node_free(self, self->root);
		freed += 1;
	}
	memset(self, 253, sizeof(*self));
	// tsfree(self);
	FPRINTF_INFO fprintf(stderr, "INFO free_Tree %d nodes freed\n", freed);
	return self;
}

/*
 * get a node or its parent (for insertion).
 */

one_node *
btree_get_Node_or_parent(one_tree *self, void *key) {
	/* we can only return a NULL if the tree is empty */
	if (!self->root)
		return NULL;

	/* walk to find node or parent */
	one_node *prior = NULL;
	one_node *curr = self->root;
	while (curr) {
		keycmp_result cmp = keycmp(self, key, curr->key);
		prior = curr;
		switch (cmp) {
		case LESS:
			curr = curr->left;
			break;
		case EQUAL:
			return curr;
		case GREATER:
			curr = curr->right;
			break;
		default:
			fprintf(stderr, "ERROR keycmp invalid key comparison result\n");
		}
	}

	/* if NULL here then Node with key not found, pass parent back */
	return curr
		? curr
		: prior;
}

/*
 * get a node or return NULL if the key is not found. this
 * is a wrapper over btree_get_Node_or_parent for the times
 * we aren't wanting to insert.
 */

one_node *
btree_get_Node_or_NULL(one_tree *self, void *key) {
	one_node *n = btree_get_Node_or_parent(self, key);
	if (!n) return NULL;
	if (keycmp(self, key, n->key) != EQUAL) return NULL;
	return n;
}

/*
 * height (or depth) of a node in the tree. this is distance
 * from root.
 */

int
btree_height(one_tree *self, one_node *n) {
	int height = 0;
	while (n->parent) {
		height += 1;
		n = n->parent;
	}
	return height;
}

/*
 * for analysis functions, what is the height of the node
 * holding a key? returns -1 if the key is not found.
 */

int
btree_height_for_key(one_tree *self, void *key) {
	if (!key) return -1;
	one_node *n = btree_get_Node_or_NULL(self, key);
	if (!n) return -1;
	return btree_height(self, n);
}

/*
 * the number of items in the tree (or subtree).
 */

int
btree_size(one_tree *self, one_node *n) {
	if (!n) return 0;
	return 1 + btree_size(self, n->left) + btree_size(self, n->right);
}

/*
 * is the tree out of balance? determined by checking the height of
 * the current node against the optimal size (log2(N)).
 */

bool
btree_is_unbalanced(one_tree *self, one_node *n) {
	int h = btree_height(self, n);
	int s = btree_size(self, self->root);
	return (h > ONE_REBALANCE_ALPHA * u32_log2(s));
}

/*
 * if the node high enough to trigger a rebalance (see
 * btree_is_unbalanced), walk back toward root to find a node that
 * holds more than 3/2 of its parent's children.
 */

bool
btree_is_scapegoat(one_tree *self, one_node *n) {
	return n && (3*btree_size(self, n) > 2*btree_size(self, n->parent));
}

/**
 * tree trversal. the standard tree traversal functions. these require
 * a client provided callback function. that function's api is
 * documented in the library header.
 *
 * these are currently marked static, and and three other functions
 * provide the external api.
 *
 * each traversal has a 'cap' function and the actual traversal
 * recursion function (name suffixed _r).
 */

static
int
btree_pre_order_traversal_r(one_tree *self, one_node *n,
	void *context, fn_traversal_cb fn) {
	if (!n) return 0;
	if (!n->deleted) fn(n->key, n->value, context, self);
	btree_pre_order_traversal_r(self, n->left, context, fn);
	btree_pre_order_traversal_r(self, n->right, context, fn);
	return 1;
}

static
int
btree_in_order_traversal_r(one_tree *self, one_node *n,
	void *context, fn_traversal_cb fn) {
	if (!n) return 0;
	btree_in_order_traversal_r(self, n->left, context, fn);
	if (!n->deleted) fn(n->key, n->value, context, self);
	btree_in_order_traversal_r(self, n->right, context, fn);
	return 1;
}

static
int
btree_post_order_traversal_r(one_tree *self, one_node *n,
	void *context, fn_traversal_cb fn) {
	if (!n) return 0;
	btree_post_order_traversal_r(self, n->left, context, fn);
	btree_post_order_traversal_r(self, n->right, context, fn);
	if (!n->deleted) fn(n->key, n->value, context, self);
	return 1;
}

static
int
pre_order_traversal(one_tree *self, void *context, fn_traversal_cb fn) {
	return btree_pre_order_traversal_r(self, self->root, context, fn);
}

static
int
in_order_traversal(one_tree *self, void *context, fn_traversal_cb fn) {
	return btree_in_order_traversal_r(self, self->root, context, fn);
}

static
int
post_order_traversal(one_tree *self, void *context, fn_traversal_cb fn) {
	return btree_post_order_traversal_r(self, self->root, context, fn);
}

/**
 * keys and values are packed into nodes, and the tree is composed of
 * nodes.
 */

/*
 * create a new unlinked Node
 */

one_node *
btree_make_Node(one_tree *self, void *key, void *value) {
	one_node *n = tsmalloc(sizeof(*n));
	memset(n, 0, sizeof(*n));
	n->key = key;
	n->value = value;
	return n;
}

/*
 * free an individual Node, warning if it has children and clearing
 * the parent's link to it if there is one.
 */

void
btree_node_free(one_tree *self, one_node *n) {
	/* warn on linked children but allow it through for now */
	if (n->left || n->right) {
		fprintf(stderr, "WARNING free_Node: freed a Node with linked children\n");
	}
	/* unlink from parent */
	if (n->parent) {
		if (n->parent->left == n) n->parent->left = NULL;
		if (n->parent->right == n) n->parent->right = NULL;
	}
	/* scrub and free */
	memset(n, 253, sizeof(*n));
	tsfree(n);
}

/*
 * free child Nodes.
 */

int
btree_node_children_free(one_tree *self, one_node *n) {
	if (!n) return 0;
	int freed = 0;
	if (n->left) {
		freed += btree_node_children_free(self, n->left);
		n->left->left = NULL;
		n->left->right =NULL;
		freed += 1;
		btree_node_free(self, n->left);
	}
	if (n->right) {
		freed += btree_node_children_free(self, n->right);
		n->right->left = NULL;
		n->right->right =NULL;
		freed += 1;
		btree_node_free(self, n->right);
	}
	return freed + 1;
}


/*
 * recursively snip off all the branches of a (sub)tree for the
 * rebuild.
 */

void
btree_reset_subtree_r(one_tree *self, one_node *subtree) {
	if (!subtree)
		return;

	if (subtree->left) {
		btree_reset_subtree_r(self, subtree->left);
		subtree->left = NULL;
	}
	if (subtree->right) {
		btree_reset_subtree_r(self, subtree->right);
		subtree->right = NULL;
	}
	if (subtree->parent) {
		if (subtree->parent->left && subtree->parent->left == subtree)
			subtree->parent->left = NULL;
		if (subtree->parent->right && subtree->parent->right == subtree)
			subtree->parent->right = NULL;
		subtree->parent = NULL;
	}

	btree_node_free(self, subtree);
}

/*
 * recursively go through key ordered list of nodes to rebuild the
 * tree (or subtree) in balance.
 *
 * given an ordered list of nodes, create a subtree from the center
 * node's key & value, building out the left and right child branches
 * by halving the list repeatedly. this halving via sub_alist creates
 * a new alist with duplicate node pointers. we delete these new lists
 * as soon as we are finished with them.
 *
 * it would be possible to delete the old node after the new node is
 * built here instead of doing a sweep over the whole list again
 * later in balancing, but for now we'll leave them alone.
 */

one_node *
btree_make_subtree_r(one_tree *self, one_block *nodes) {
	int k = count(nodes);
	if (k == 0)
		return NULL;

	/* next subtree root */
	int j = k/2;
	one_node *old = (one_node *)nth(nodes, j);
	one_node *new = btree_make_Node(self, old->key, old->value);

	/* and to either side of the root, we have further subtrees */
	one_block *left_side = slice(nodes, 0, j);
	one_block *right_side = slice(nodes, j+1, k);

	/* so do the left children */
	new->left = btree_make_subtree_r(self, left_side);
	free_one(left_side);
	if (new->left)
		new->left->parent = new;

	/* and the right children */
	new->right = btree_make_subtree_r(self, right_side);
	free_one(right_side);
	if (new->right)
		new->right->parent = new;

	/* and return our subtree root */
	return new;
}

/*
 * an in order traversal recursively collect all the non-deleted
 * nodes for rebalancing.
 */

one_block *
btree_node_collector(one_tree *self, one_node *n, one_block *xs) {
	if (!n)
		return xs;
	xs = btree_node_collector(self, n->left, xs);
	if (!n->deleted)
		xs = cons(xs, (uintptr_t)n);
	xs = btree_node_collector(self, n->right, xs);
	return xs;
}

/*
 * an in order traversal recursively collect the keys of all the
 * non-deleted nodes for rebalancing.
 */

one_block *
btree_key_collector(one_tree *self, one_node *n, one_block *xs) {
	if (!n)
		return xs;
	xs = btree_key_collector(self, n->left, xs);
	if (!n->deleted)
		xs = cons(xs, (uintptr_t)n->key);
	xs = btree_key_collector(self, n->right, xs);
	return xs;
}

/*
 * an in order traversal recursively collect the values of all the
 * non-deleted nodes for rebalancing.
 */

one_block *
btree_value_collector(one_tree *self, one_node *n, one_block *xs) {
	if (!n)
		return xs;
	xs = btree_value_collector(self, n->left, xs);
	if (!n->deleted)
		xs = cons(xs, (uintptr_t)n->value);
	xs = btree_value_collector(self, n->right, xs);
	return xs;
}

/*
 * once we know where we need to rebalance from, use the old (sub)tree
 * to build a balanced (sub)tree and and replace the old (sub)tree.
 *
 * to rebalance the whole Tree, pass self->root as the subtree.
 */

one_node *
btree_rebalance_r(one_tree *self, one_node *subtree) {

	FPRINTF_INFO fprintf(stderr, "INFO rebalance begin rebalancing\n");

	/* remember where to hang the subtree. if parent is NULL,
	 * this is root. if it isn't remember if the subtree was
	 * on the left or right. */

	one_node *parent = subtree->parent;
	bool left_side = (parent && parent->left == subtree);

	/* do an in_order traversal to get an alist of all non-deleted
	 * nodes in the (sub)tree rooted at *subtree.
	 *
	 * this can be very slow when running with sanitizers and
	 * debugging help. 10k items over 10 seconds vs 10k items
	 * over 2 seconds.
	 *
	 * and then there's the additinal memory consumed. we need
	 * to start deleting once we're done with a node.
	 */

	one_block *xs = make_one(alist);
	xs = btree_node_collector(self, subtree, xs);

	/* build a new subtree by going over the node list in the
	 * optimal order for insertion: that is, repeatedly halving
	 * the list to create the ordered list and then linking the
	 * nodes into a subtree. */

	FPRINTF_INFO fprintf(stderr, "INFO rebalance nodes in subtree %d\n", count(xs));
	one_node *new_subtree = btree_make_subtree_r(self, xs);
	free_one(xs);

	/* destroy the old nodes and replace them with the new
	 * nodes. */

	btree_reset_subtree_r(self, subtree);

	if (!parent) {
		self->root = new_subtree;
	} else {
		if (left_side) parent->left = new_subtree;
		else           parent->right = new_subtree;
		self->partial_rebalances += 1;
	}

	FPRINTF_INFO fprintf(stderr, "INFO rebalance end rebalancing\n");
	return new_subtree;
}

/**
 * the key:value store is built on a self balancing binary search
 * tree.
 *
 * i originally tried to use sedgewick's left-leaning red-black tree
 * but even after converting his java to c i can't get a working
 * solution. the best i got is that the my c of his java loses 23 keys
 * out of my 10k dataset. i have no idea why. after a couple of weeks
 * of tracing and banging my head, i gave up.
 *
 * next i looked at a plain red-black tree but the implementations are
 * fugly. it works but i don't really get procedure. at least with
 * sedgewick i understood what was going on.
 *
 * i had heard about scapegoat trees a while back and after reviewing
 * some documentation on them i know i can put an implementation
 * together that will work and be comprehensible.
 *
 * the basic operations are all externally similar to any other search
 * tree, but additional work is done both on insert and delete to
 * determine if the tree is out of balance and if so guide the
 * rebalancing.
 *
 * scapegoat trees are 'loosely' height and width balanced, and while
 * the rebalance operation can be lenghy, it will be done infrequently,
 * amortizing the expense.
 *
 * the maximum depth for a balanced binary tree is log(2)N, but strict
 * adherence to that is probably not wise.
 *
 * for relatively small trees (~100 nodes? ~1000? testing is needed)
 * we don't need to rebalance the tree because even if the tree has
 * degenerated into a list it just doesn't take long to find a node on
 * modern hardware.
 *
 * a bit of tennessee windage is applied (currently +2) to the optimal
 * depth before rebalancing is triggered. the whole tree or just a
 * branch may be rebalanced. my current plan is to avoid full tree
 * rebalancing by backtracking up to the root and if a node is found
 * where one side or the other has 'too many children', just rebalance
 * there and continue.
 *
 * while checking node depth is valid for insertion, it is not for
 * deletion.
 *
 * the recommendation is to rebalance the entire tree once 'the number
 * of deletions performed is significant compared with the tree size.'
 * (lecture notes by Dave Mount for CMSC 420: Lecture 12 Balancing
 * by Rebuilding – Scapegoat Trees, University of Maryland, fall 2020)
 *
 * to do this, count insertions and compare that to the actual number
 * of nodes in the tree. once there has been enough churn (inserts > 2
 * * nodes), reblance the whole tree.
 *
 * i expect to experiment to find a ratio that works in my use cases.
 * i am also trying to envision ways to rebalance only a portion of
 * the tree on deletion.
 *
 * for either case, rebalancing is just a matter of recursive bisection
 * of the nodes and relinking them.
 */

/*
 * full Tree rebalance.
 */

one_tree *
btree_rebalance(one_tree *self) {
	if (!self || !self->root)
		return self;
	btree_rebalance_r(self, self->root);
	FPRINTF_INFO fprintf(stderr, "Rebalance:\n");
	FPRINTF_INFO fprintf(stderr, "  nodes: %d\n", self->nodes);
	FPRINTF_INFO fprintf(stderr, "inserts: %d\n", self->inserts);
	FPRINTF_INFO fprintf(stderr, "deletes: %d\n", self->deletes);
	FPRINTF_INFO fprintf(stderr, "updates: %d\n", self->updates);
	FPRINTF_INFO fprintf(stderr, " marked: %d\n", self->marked_deleted);
	FPRINTF_INFO fprintf(stderr, "partial: %d\n", self->partial_rebalances);

	self->inserts = 0;
	self->deletes = 0;
	self->updates = 0;
	self->marked_deleted = 0;
	self->full_rebalances += 1;

	FPRINTF_INFO fprintf(stderr, "   full: %d\n", self->full_rebalances);
	return self;
}

/*
 * a full tree rebalance is triggered by deletes exceeding some percenta
 * of total nodes or inserts being currently double nodes. post
 * insert depth checks are done on insert.
 */

bool
btree_should_full_rebalance(one_tree *self) {
	/* empty tree or less than (arbitrarily) 64 nodes, don't bother */
	if (!self->root || self->nodes < 64)
		return false;

	/* have we done enough deletes? */
	if (100*((float)self->marked_deleted/self->nodes) >
		ONE_REBALANCE_DELETE_PERCENT)
		return true;

	/* too much churn */
	if (self->inserts > 2 * self->nodes)
		return true;

	return false;
}

/*
 * keys and values are passed as if they are pointers, but they do not
 * have to be.
 *
 * if key is a pointer, make sure the value referenced doesn't change
 * or the Tree will be broken.
 *
 * see the comments on get() below for more on pointers.
 */

/**
 * insert a new node into the tree
 */

bool
btree_insert_r(one_tree *self, one_node *parent, one_node *new) {

	/* tree is empty */
	if (!parent) {
		self->root = new;
		return true;
	}

	/* were does new key fit? */
	int side = self->fn_cmp(new->key, parent->key);

	/* key match, but is it a deleted node? */
	if (side == 0) {
		if (parent->deleted) {
			parent->deleted = false;
			parent->value = new->value;
			btree_node_free(self, new);
			return true;
		}
		btree_node_free(self, new);
		/* fprintf(stderr, "ERROR insert: attempt to insert an existing key\n"); */
		return false;
	}

	new->parent = parent;

	/* counting on deletion of leaf Nodes to really delete, not just
	 * mark the Node as deleted. */
	if (side < 0 && !parent->left)
		parent->left = new;
	else if (side > 0 && !parent->right)
		parent->right = new;
	else {
		fprintf(stderr, "ERROR insert: attempting to overlay existing node %p %p\n",
			(void *)new, (void *)parent);
		btree_node_free(self, new);
		return false;
	}

	return true;
}

bool
btree_insert(one_tree *self, void *key, void *value) {
	one_node *parent = btree_get_Node_or_parent(self, key);
	one_node *n = btree_make_Node(self, key, value);
	bool did = btree_insert_r(self, parent, n);
	if (did) {
		self->nodes += 1;
		self->inserts += 1;
		if (self->rebalance_allowed && btree_is_unbalanced(self, n)) {
			FPRINTF_INFO fprintf(stderr, "INFO insert: unbalanced@ %d %d %d %d %ld\n",
				self->nodes, self->inserts,
				btree_size(self, self->root), btree_height(self, n), (uintptr_t)key);
			one_node *s = n->parent;
			while (s) {
				if (!btree_is_scapegoat(self, s)) {
					s = s->parent;
					continue;
				}
				FPRINTF_INFO fprintf(stderr, "INFO insert:  scapegoat@ %d %d %d %d %ld\n",
					self->nodes, self->inserts,
					btree_size(self, self->root), btree_height(self, s), (uintptr_t)s->key);
				s = btree_rebalance_r(self, s);
				break;
			}
			if (!s) FPRINTF_INFO fprintf(stderr, "INFO insert: no scapegoat found!\n");
		}
	}
	return did;
}

/**
 * delete a node from the tree.
 */

/*
 * most deletes are deferred. rather than juggle pointers we'll drop the
 * deleted Nodes during a rebalance.
 */

bool
btree_delete(one_tree *self, void *key) {
	one_node *n = btree_get_Node_or_NULL(self, key);
	if (n && n->deleted) {
		fprintf(stderr, "WARNING delete: key not found in tree.\n");
		return false;
	}

	/*
	 * if it's a terminal Node that isn't root, really delete it.
	 */

	if (!n->left && !n->right) {
		FPRINTF_INFO fprintf(stderr, "INFO deleting leaf: %p\n", (void *)n->key);
		if (n->parent && n->parent->left == n) n->parent->left = NULL;
		if (n->parent && n->parent->right == n) n->parent->right = NULL;
		if (n->parent == NULL && self->root == n)
			self->root = NULL;
		n->parent = NULL;
		btree_node_free(self, n);
		self->deletes += 1;
		self->nodes -= 1;
		return true;
	};

	/*
	 * flag the Node as deleted, it will be removed during rebalancing.
	 */
	FPRINTF_INFO fprintf(stderr, "INFO marking deleted non-leaf: %p\n",
		(void *)n->key);

	n->deleted = true;
	n->value = NULL;
	self->marked_deleted += 1;
	self->deletes += 1;
	self->nodes -= 1;

	/* /\* */
	/*  * experiment, just rebalance on every delete, but only under node's */
	/*  * parent. */
	/*  *\/ */
	/*  */
	/* if (n->parent) { */
	/*      btree_rebalance_r(self, n); */
	/* } else { */
	/*      btree_rebalance_r(self, self->root); */
	/* } */

	if (btree_should_full_rebalance(self))
		btree_rebalance(self);

	return true;
}

/**
 * get and return the value associated with a key
 */

/*
 * while the code reads as if key and value are pointers, that is
 * not strictly enforced. any value that will fit into a pointer
 * sized space is legal.
 *
 * if the key is not found or deleted, a NULL is returned, but a
 * NULL could be a valid value in the client's schema. if this
 * is true, exists() should be used before get() if the client
 * cares that a given key did not exist.
 */

void *
btree_get(one_tree *self, void *key) {
	one_node *n = btree_get_Node_or_NULL(self, key);
	if (!n || n->deleted) return NULL;
	return n->value;
}

/**
 * update the value associated with a key
 */

/*
 * if value is a pointer, this isn't strictly needed.
 */

bool
btree_update(one_tree *self, void *key, void *value) {
	one_node *n = btree_get_Node_or_NULL(self, key);
	if (n == NULL) return false;
	n->value = value;
	self->updates += 1;
	return true;
}

/**
 * predicates and queries on the Tree or its contents
 */

bool
btree_is_empty(one_tree *self) {
	return self->nodes == 0;
}

int
btree_count(one_tree* self) {
	return self->nodes;
}

bool
btree_exists(one_tree *self, void *key) {
	one_node *n = btree_get_Node_or_NULL(self, key);
	if (!n || n->deleted) return NULL;
	return n;
}

/**
 * the priority queue (pqueue) is a non-uniquely keyed doubly
 * linked list. many of the functions look redundant with their
 * doubly counterparts, but i prefer duplication to a bunch of
 * special case if/else blocks.
 */

static
int
pq_count(
	one_block *pq
) {
	int i = 0;
	pq_item *qi = pq->u.pqu.first;
	while (qi) {
		i += 1;
		qi = qi->next;
	}
	return i;
}

static
pq_item *
pq_create_item(
	long priority,
	void * payload
) {
	pq_item *qi = tmalloc(sizeof(*qi));
	memset(qi, 0, sizeof(*qi));
	qi->priority = priority;
	qi->item = payload;
	qi->next = NULL;
	qi->previous = NULL;
	return qi;
}

static
int
pq_purge(
	one_block *pq
) {
	int i = 0;
	pq_item *qi = NULL;
	while (qi = pq->u.pqu.first, qi) {
		i += 1;
		pq->u.pqu.first = qi->next;
		memset(qi, 253, sizeof(*qi));
		tfree(qi);
	}
	return i;
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
 * functions that don't really need to return a item or count
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

/**
 * structure creation, duplication, and deletion
 */

/**
 * make_one
 *
 * create an instance of one of the data structure types. allocates and
 * initializes the 'one block' and returns it to the client. the client
 * passes this back on subsequent calls as a handle.
 * a constructor, if you will.
 *
 * returns the instance handle or NULL on error.
 */

one_block *
make_one(
	one_type isa
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

	case pqueue:
		ob->u.pqu.first = NULL;
		ob->u.pqu.last = NULL;
		return ob;

	case alist:
		ob->u.acc.used = 0;
		ob->u.acc.capacity = ONE_ALIST_DEFAULT_CAPACITY;
		ob->u.acc.list = tsmalloc(ONE_ALIST_DEFAULT_CAPACITY * sizeof(uintptr_t));
		memset(ob->u.acc.list, 0, ob->u.acc.capacity * sizeof(uintptr_t));
		return ob;

	case dynarray:
		ob->u.dyn.length = -1;
		ob->u.dyn.capacity = ONE_DYNARRAY_DEFAULT_CAPACITY;
		ob->u.dyn.array = tsmalloc(ONE_DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
		memset(ob->u.dyn.array, 0, ONE_DYNARRAY_DEFAULT_CAPACITY * sizeof(void *));
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

/**
 * create a new empty keyed instance. this will usually be a backed by
 * a binary search tree, but it may present an different api.
 *
 * at this time, dictionary like or symbol table like access via the
 * key to some value is available.
 *
 * as with make_one, the returns the instance handle or NULL.
 */

one_block *
make_one_keyed(
	one_type isa,
	one_key_type kt,
	one_key_comparator func_or_NULL
) {
	one_block *ob = tsmalloc(sizeof(*ob));
	memset(ob, 0, sizeof(*ob));
	ob->isa = isa;
	if (isa <= ONE_TYPE_MAX && isa > 0)
		strncpy(ob->tag, one_tags[isa], ONE_TAG_LEN-1);
	else
		strncpy(ob->tag, "*invalid one type*", ONE_TAG_LEN-1);

	switch (ob->isa) {
	case keyval:

		ob->u.kvl.fn_cmp = func_or_NULL;
		ob->u.kvl.root = NULL;
		ob->u.kvl.rebalance_allowed = true;
		ob->u.kvl.kt = kt;
		switch (kt) {

		case integral:     /* treat the key as a void * sized integer, a long */
			ob->u.kvl.fn_cmp = (one_key_comparator)integral_comp;
			if (func_or_NULL)
				fprintf(stderr,
					"WARNING make_Tree: client provided comparator function for integral keys ignored.\n");
			break;

		case string:      /* strings are standard char * bytestrings */
			ob->u.kvl.fn_cmp = (one_key_comparator)strcmp;
			if (func_or_NULL)
				fprintf(stderr,
					"WARNING make_Tree: client provided comparator function for string keys ignored.\n");
			break;

		case custom:    /* client provides comparator */
			ob->u.kvl.fn_cmp = func_or_NULL;
			if (ob->u.kvl.fn_cmp)
				break;
			fprintf(stderr, "ERROR make_Tree: missing comparator function.\n");

		default:
			fprintf(stderr, "ERROR make_Tree: error in key type or function\n");
			tsfree(ob);
			ob = NULL;
		}

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

/**
 * clone -- alist
 *
 * returns a new copy of an alist.
 */

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

/**
 * purge -- singly, stack, doubly, queue, deque, alist
 *
 * empty the data structure. frees internal resources for all items
 * managed by the structure. client data is left alone. this has no
 * meaning for a dynamic array.
 *
 * returns the number of items purged or -1 on any detected error.
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

	case pqueue:
		return pq_purge(ob);

	default:
		fprintf(stderr, "\nERROR txbone-purge: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return -1;
	}
}

/**
 * free_one
 *
 * destroy an instance of a data structure, releasing library managed
 * memory, a destructor.
 *
 * returns NULL.
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

		case keyval:
			// TODO: fix to use purge as for others ...
			btree_free(&ob->u.kvl);
			memset(ob, 253, sizeof(*ob));
			tsfree(ob);
			return NULL;

		case pqueue:
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
 * singly and doubly linked list functions
 */

/**
 * add_first -- singly, doubly.
 *
 * add an item to the front of all items held.
 *
 * returns the instance handle or NULL on error.
 */

one_block *
add_first(
	one_block *ob,
	void *item
) {
	if (item == NULL)
		return ob;
	switch (ob->isa) {

	case singly:
		singly_add_first(&ob->u.sgl, item);
		return ob;

	case doubly:
		doubly_add_first(&ob->u.dbl, item);
		return ob;

	default:
		fprintf(stderr, "\nERROR txbone-add_first: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * add_last -- singly, doubly
 *
 * add an item to the end of all items held.
 *
 * returns the instance handle or NULL.
 */

one_block *
add_last(
	one_block *ob,
	void *item
) {
	if (item == NULL)
		return ob;
	switch (ob->isa) {

	case singly:
		singly_add_last(&ob->u.sgl, item);
		return ob;

	case doubly:
		doubly_add_last(&ob->u.dbl, item);
		return ob;

	default:
		fprintf(stderr, "\nERROR txbone-add_last: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * peek_first -- singly, doubly
 *
 * return but do not remove the item at the front of all items held.
 *
 * returns the item or NULL on either error or empty.
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
 * peek_last -- singly, doubly
 *
 * return but do not remove the item at the end of all items held.
 *
 * returns the item or NULL on either an error or empty.
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
 * get_first -- singly, doubly
 *
 * remove and return the item at the front of all items held.
 *
 * returns the item or NULL on either an error or empty.
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
 * get_last -- singly, doubly
 *
 * remove and return the item at the end of all items held.
 *
 * returns the item or NULL on either an error or empty.
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
 * functions common to all (most) structures, for information
 * about the structure or its state.
 */

/**
 * count -- singly, doubly, queue, deque, alist
 *
 * how many things are managed by the data structure. for a stack, use
 * depth. has no meaning for a dynamic array.
 *
 * returns the number of items or -1 on error.
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

	case keyval:
		return ob->u.kvl.nodes;
	// btree_size(&ob->u.kvl, ob->u.kvl.root);

	case pqueue:
		return pq_count(ob);

	default:
		fprintf(stderr, "\nERROR txbone-count: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return -1;
	}
}

/**
 * is_empty -- singly, stack, doubly, queue, deque, alist
 *
 * predicate is this data structure empty (count/depth == 0)?
 *
 * returns a boolean, any detected errors return false.
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
		return ob->u.acc.used == 0;

	case keyval:
		return ob->u.kvl.root == NULL;

	case pqueue:
		return ob->u.pqu.first == NULL;

	default:
		fprintf(stderr, "\nERROR txbone-empty: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return false;
	}
}

/**
 * a stack is implemented on a singly linked list, but use the
 * following entry points in addition to make_one, free_one, is_empty,
 * peek, and purge.
 */

/**
 * depth -- stack
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

/**
 * push -- stack
 *
 * add an item to the top of the stack.
 *
 * returns the stack instance.
 */

one_block *
push(
	one_block *ob,
	void *item
) {
	switch (ob->isa) {

	case stack:
		singly_add_first(&ob->u.sgl, item);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-push: unknown or unsupported type %d %s, expected stack\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * pop -- stack
 *
 * remove an item from the top of the stack.
 *
 * returns the item.
 */

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
 * peek -- stack, queue
 *
 * return but do not remove the top (stack) or oldest (queue) item.
 *
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
 * a queue (fifo) is implemented on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, is_empty,
 * count, peek, and purge.
 */

/**
 * enqueue -- queue
 *
 * add an item to the queue.
 *
 * returns the queue instance.
 */

one_block *
enqueue(
	one_block *ob,
	void *item
) {
	switch (ob->isa) {

	case queue:
		doubly_add_last(&ob->u.dbl, item);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-enqueue: unknown or unsupported type %d %s, expected queue\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * dequeue -- queue
 *
 * remove and return the oldest item from the queue.
 *
 * returns the item.
 */

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
 * a deque (f/l-ifo)is built on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * count, and purge.
 *
 * items can be removed from either end of the deque. one end is
 * arbitrarily considered to be the 'front', while the other end is
 * considered to be the 'back'.
 *
 * 'left' and 'right' would be just as valid, but 'top' or 'bottom'
 * would collide with stack terminology.
 */

/**
 * push_front -- deque
 *
 * add an item to the front of the deque.
 *
 * returns the deque instance.
 */

one_block *
push_front(
	one_block *ob,
	void *item
) {
	switch (ob->isa) {

	case deque:
		doubly_add_first(&ob->u.dbl, item);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-push_front: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * push_back -- deque
 *
 * add an item to the back of the deque.
 *
 * returns the deque instance.
 */

one_block *
push_back(
	one_block *ob,
	void *item
) {
	switch (ob->isa) {

	case deque:
		doubly_add_last(&ob->u.dbl, item);
		return ob;

	default:
		fprintf(stderr,
			"\nERROR txbone-push_back: unknown or unsupported type %d %s, expected deque\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * pop_front -- deque
 *
 * remove and return the item at the front of the deque.
 */

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

/**
 * pop_back -- deque
 *
 * remove and return the item at the back of the deque.
 */

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

/**
 * peek_front -- deque
 *
 * return but do not remove the item at the front of the deque.
 */

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

/**
 * peek_back -- deque
 *
 * return but do not remove the item at the back of the deque.
 */

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
 * free, they support hbound via high_index, get_from, and put_at.
 */

/**
 * high_index -- dynarray
 *
 * the highest used (via put_at) index in the array. while a item
 * may be put anywhere with a non-negative index, a get_from is only
 * valid for an index in the range (0, high index).
 *
 * returns the index or -1 on error.
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
 * put_at -- dynarray
 *
 * store a value at a particular index in the array. if the array's
 * capacity is less than the index, double the capacity until the
 * index is valid.
 *
 * returns the array instance or NULL on error.
 */

one_block *
put_at(
	one_block *self,
	void *item,
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
	self->u.dyn.array[n] = item;
	if (n > self->u.dyn.length)
		self->u.dyn.length = n;
	return self;
}

/**
 * get_from -- dynarray
 *
 * returns the item from a particular index in the array.
 *
 * if the index lies outside [0..high_index) but nothing has been
 * put_at that location, return NULL.
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
			"\nERROR txbone-get_at: index out of bounds %d not in range [0..%d)\n", n,
			self->u.dyn.length);
		return NULL;
	}
	return (self->u.dyn.array)[n];
}

/**
 * accumulator list functions. the alist has a subset of array list
 * and lisp/sml list semantics. it is meant to ease accumulating
 * discrete values in recursions. the function naming is lisp
 * inspired.
 *
 * see the comment block at the head of the alist_* functions for
 * a more detailed description of behaviors.
 */

/**
 * cons -- alist
 *
 * add an item to the end of an alist.
 *
 * returns the instance.
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

/**
 * car -- alist
 *
 * return but do not remove the item at the front of the alist.
 *
 * returns the item.
 */

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

/**
 * cdr -- alist
 *
 * return a copy of the alist with the first item (car) removed.
 */

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

/**
 * append -- alist
 *
 * concatenate two lists, adding a copy of the items from the second
 * or 'right' list to the first or 'left' list. behaves sensibly when
 * either list is empty.
 *
 * returns a new handle to the left list. the old handle should be
 * considered invalid.
 */

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

/**
 * slice -- alist
 *
 * return a new alist containing only the elements selected by the slice.
 *
 * the range is [from_inclusive..to_exclusive).
 *
 * the result is distinct from the original list, which is unchanged.
 */

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

/**
 * setnth -- alist
 *
 * store an item in the nth slot of the list (counting from 0). if the
 * index is out of bounds, a warning is issued to the log and the
 * operation is ignored.
 */

one_block *
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

/**
 * nth -- alist
 *
 * return the item from the nth slot of the list (counting from 0). if
 * the index is out of bounds, a warning is issued to the log and 0 is
 * returned.
 */

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

/**
 * priority queue functions
 *
 * priorities are signed longs. the queue is maintained in
 * order by priority, and then first in first out for items
 * of equal priority.
 */

/**
 * add_with_max -- pqueue
 *
 * add the item as the maximum item in the queue. this means priority
 * equal to the highest priority found, but positioned as the first
 * item instead of the next item.
 */

one_block *
add_with_max(
	one_block *ob,
	void *item
) {
	return add_with_priority(ob, max_priority(ob), item);
}

/**
 * add_with_min -- pqueue
 *
 * as in add_with_max but the priority is equal to the lowest priority
 * found and positioned as the last item.
 */

one_block *
add_with_min(
	one_block *ob,
	void *item
) {
	return add_with_priority(ob, min_priority(ob), item);
}

/**
 * add_with_priority -- pqueue
 *
 * add the item to its spot in the queue based on its priority.
 */

one_block *
add_with_priority(
	one_block *ob,
	long priority,
	void *item
) {
	pq_item *qi = pq_create_item(priority, item);

	/* empty is easy.  */
	if (ob->u.pqu.first == NULL) {
		ob->u.pqu.first = qi;
		ob->u.pqu.last = qi;
		return ob;
	}

	if (qi->priority <= ob->u.pqu.first->priority) {
		qi->next = ob->u.pqu.first;
		qi->next->previous = qi;
		ob->u.pqu.first = qi;
		return ob;
	} else if (qi->priority > ob->u.pqu.last->priority) {
		qi->previous = ob->u.pqu.last;
		qi->previous->next = qi;
		ob->u.pqu.last = qi;
		return ob;
	}

	/* find an insertion point. */
	pq_item *p = ob->u.pqu.first;
	while (p) {
		if (p->priority < qi->priority) {
			p = p->next;
			continue;
		}
		qi->previous = p->previous;
		p->previous = qi;
		qi->previous->next = qi;
		qi->next = p;
		return ob;
	}

	/* if we get here, the queue is broken. */
	return NULL;
}

/**
 * get_ and peek_max -- pqueue
 *
 * return the oldest item at the highest priority in the pqueue. peek
 * leaves the item in place.
 */

void *
get_max(
	one_block *ob
) {
	if (ob->u.pqu.first == NULL)
		return NULL;
	pq_item *qi = ob->u.pqu.last;
	void *ret = qi->item;
	ob->u.pqu.last = qi->previous;
	memset(qi, 253, sizeof(*qi));
	tfree(qi);
	if (ob->u.pqu.last == NULL)
		ob->u.pqu.first = NULL;
	else
		ob->u.pqu.last->next = NULL;
	return ret;
}

void *
peek_max(
	one_block *ob
) {
	return ob->u.pqu.last ? ob->u.pqu.last->item : NULL;
}

/**
 * get_ and peek_min -- pqueue
 *
 * return the oldest item at the lowest priority in the pqueue. peek
 * leaves the item in place.
 */

void *
get_min(
	one_block *ob
) {
	if (ob->u.pqu.first == NULL)
		return NULL;
	pq_item *qi = ob->u.pqu.first;
	void *ret = qi->item;
	ob->u.pqu.first = qi->next;
	memset(qi, 253, sizeof(*qi));
	tfree(qi);
	if (ob->u.pqu.last == NULL)
		ob->u.pqu.first = NULL;
	else
		ob->u.pqu.last->next = NULL;
	return ret;
}

void *
peek_min(
	one_block *ob
) {
	return ob->u.pqu.first ? ob->u.pqu.first->item : NULL;
}

/**
 * max_priority -- pqueue
 *
 * maximum found or 0 if the queue is empty.
 */

long
max_priority(
	one_block *ob
) {
	return ob->u.pqu.last ? ob->u.pqu.last->priority : 0;
}

/**
 * min_priority -- pqueue
 *
 * minimum found or 0 if the queue is empty.
 */

long
min_priority(
	one_block *ob
) {
	return ob->u.pqu.first ? ob->u.pqu.first->priority : 0;
}

/**
 * key:value store
 */

/**
 * insert, get, update, delete -- keyval
 *
 * these are the standard 'io' functions for any indexed access
 * method. all except get return a boolean indicating success or
 * failure.
 *
 * delete returns the item or NULL on error. remember that the item
 * could be NULL as we do no special handling of them.
 */

bool
insert(one_block *ob, void *key, void *value) {
	switch (ob->isa) {
	case keyval:
		return btree_insert(&ob->u.kvl, key, value);

	default:
		fprintf(stderr, "\nERROR txbone-insert: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

void *
get(one_block *ob, void *key) {
	switch (ob->isa) {
	case keyval:
		return btree_get(&ob->u.kvl, key);

	default:
		fprintf(stderr, "\nERROR txbone-get: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

bool
delete (one_block *ob, void *key) {
	switch (ob->isa) {
	case keyval:
		return btree_delete(&ob->u.kvl, key);

	default:
		fprintf(stderr, "\nERROR txbone-delete: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return false;
	}
}

bool
update(one_block *ob, void *key, void *value) {
	switch (ob->isa) {
	case keyval:
		return btree_update(&ob->u.kvl, key, value);

	default:
		fprintf(stderr, "\nERROR txbone-update: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * exists -- keyval
 *
 * does an item with a particular key exist in the key:value store?
 */

bool
exists(one_block *ob, void *key) {
	switch (ob->isa) {
	case keyval:
		return btree_exists(&ob->u.kvl, key);

	default:
		fprintf(stderr, "\nERROR txbone-exists: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return false;
	}
}

/**
 * functions that collect keys, values, or those that can iterate over the
 * items in a structure.
 */

/**
 * iterate -- alist
 *
 * return the item at index curr and increments the index. when the
 * index is out of bounds, it is set to -1 and 0 is returned.
 *
 * no warning is issued if the initial index is out of bounds.
 */

uintptr_t
iterate(one_block *ob, int *curr) {
	if (ob->isa != alist) {
		fprintf(stderr, "\nERROR txbone-iterate: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
	}
	return alist_iterate(ob, curr);
}

/**
 * keys -- keyval
 *
 * return an alist of all the current keys in order.
 */

one_block *
keys(one_block *ob) {
	switch (ob->isa) {
	case keyval: {
		one_block *xs = make_one(alist);
		if (ob->u.kvl.root)
			xs = btree_key_collector(&ob->u.kvl, ob->u.kvl.root, xs);
		return xs;
	}
	default:
		fprintf(stderr, "\nERROR txbone-keys: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * values -- keyval
 *
 * return an alist of all the current values in key order.
 */

one_block *
values(one_block *ob) {
	switch (ob->isa) {
	case keyval: {
		one_block *xs = make_one(alist);
		if (ob->u.kvl.root)
			xs = btree_value_collector(&ob->u.kvl, ob->u.kvl.root, xs);
		return xs;
	}
	default:
		fprintf(stderr, "\nERROR txbone-values: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return NULL;
	}
}

/**
 * in_, pre_, and post_order_keyed -- keyval
 *
 * these provide the standard tree traversals. the client's callback
 * function is called once for each item. a context field (pointer
 * sized) can be provided by the client and is passed unaltered to the
 * callback.
 *
 * the client callback function returns a boolean `true` to continue
 * the traversal, or `false` to stop it.
 *
 * the return value is currently unused.
 */

int
in_order_keyed(one_block *ob, void *context, fn_traversal_cb fn) {
	switch (ob->isa) {
	case keyval:
		return in_order_traversal(&ob->u.kvl, context, fn);

	default:
		fprintf(stderr,
			"\nERROR txbone-in-order-keyed: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return -1;
	}
}

int
pre_order_keyed(one_block *ob, void *context, fn_traversal_cb fn) {
	switch (ob->isa) {
	case keyval:
		return pre_order_traversal(&ob->u.kvl, context, fn);

	default:
		fprintf(stderr,
			"\nERROR txbone-pre-order-keyed: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return -1;
	}
}

int
post_order_keyed(one_block *ob, void *context, fn_traversal_cb fn) {
	switch (ob->isa) {
	case keyval:
		return post_order_traversal(&ob->u.kvl, context, fn);

	default:
		fprintf(stderr,
			"\nERROR txbone-in-order-keyed: unknown or unsupported type %d %s\n",
			ob->isa, ob->tag);
		return -1;
	}
}

/* txbone.c ends here */
