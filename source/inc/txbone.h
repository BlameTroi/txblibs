/* txbone.h -- one data structure library to rule them all */

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

#ifndef _TXBONE_H_
#define _TXBONE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>

/*
 * the supported data structures. there is a table of tag strings in
 * the implementation side that must be kept in synch with these
 * values. also make sure ONE_TYPE_MAX is correct.
 */

enum one_type {
	unknown = 0,
	deque,
	queue,
	stack,
	singly, /* linked list */
	doubly, /* linked list */
	dynarray,
	bst,
	keyval,
	hash,
	dictionary,
	bag,
	ordered /* collection */
};

#define ONE_TYPE_MAX ordered

#define ONE_TAG_LEN 24

/*
 * this is completely arbitrary. some implementations actually
 * start at 1 or 2.
 */

#define DYNARRAY_DEFAULT_CAPACITY 512

/*
 * i prefer working with typedefs of structs. the one_block carries
 * all the information needed for an instance of any of the data
 * structures in these libraries.
 */

typedef struct one_block one_block;

typedef union one_details one_details;

/*
 * the declarations and definitions of the supported data
 * structures.
 */

/*
 * a singly linked list and its nodes.
 */

typedef struct sgl_item sgl_item;

struct sgl_item {
	sgl_item *next;
	void *payload;
};

typedef struct one_singly one_singly;

struct one_singly {
	sgl_item *first;
};

/*
 * a stack (lifo) is merely a different api for a singly linked list.
 */

typedef one_singly one_stack;

/*
 * a doubly linked list and it's nodes.
 */

typedef struct dbl_item dbl_item;

struct dbl_item {
	dbl_item *next;
	dbl_item *previous;
	void *payload;
};

typedef struct one_doubly one_doubly;

struct one_doubly {
	dbl_item *first;
	dbl_item *last;
};

/*
 * both a deque (double ended queue) and queue (fifo) are just
 * different apis over a doubly linked list.
 */

typedef one_doubly one_deque;

typedef one_doubly one_queue;

/*
 * a dynamically resizing array.
 */

typedef struct one_dynarray one_dynarray;

struct one_dynarray {
	int length;                 /* last used via put */
	int capacity;               /* current maximum capacity */
	void **array;               /* and where can it be now */
};

/*
 * a binary search tree.
 */

typedef struct one_bst one_bst;

struct one_bst {
	void *not_implemented_yet;
};

/*
 * a key value store.
 */

typedef struct one_keyval one_keyval;

struct one_keyval {
	void *not_implemented_yet;
};

/*
 * rather than have separate high level control blocks, this union
 * approach allows for a cleaner interface and less redundancy.
 */

union one_details {
	one_deque deq;
	one_queue que;
	one_stack stk;
	one_singly sgl;
	one_doubly dbl;
	one_dynarray dyn;
	one_bst bst;
	one_keyval kvl;
};

/*
 * the 'one_block' is a control block used as a handle for the client
 * code. it is kept small and the details of each specific data
 * structure are kept here in common via a union.
 */

struct one_block {
	enum one_type isa;
	int odometer;
	char tag[ONE_TAG_LEN];
	one_details dtl;
};

/*
 * create, destroy, and functions global to all data structures. all
 * entry points other than make_one and free_one tend to delegate to
 * other functions that don't have external scope.
 *
 * functions that don't really need to return a payload or count
 * return their first argument, which allows for chaining calls.
 */

one_block *
make_one(
	enum one_type isa
);

one_block *
free_one(
	one_block *me
);

int
count(
	one_block *me
);

bool
empty(
	one_block *me
);

int
purge(
	one_block *me
);

/*
 * these are possibly likely entry points for all the data structure.
 * they are definitely the entry points for singly and doubly linked
 * lists.
 */

one_block *
add_first(
	one_block *me,
	void *payload
);

one_block *
add_last(
	one_block *me,
	void *payload
);

void *
peek_first(
	one_block *me
);

void *
peek_last(
	one_block *me
);

void *
get_first(
	one_block *me
);

void *
get_last(
	one_block *me
);

/*
 * a stack is implemented on a singly linked list, but it should use
 * the following entry points.
 */

one_block *
push(
	one_block *me,
	void *payload
);

void *
pop(
	one_block *me
);

void *
peek(
	one_block *me
);

int
depth(
	one_block *me
);

/*
 * a queue (fifo) is implemented on a doubly linked list, but it
 * should use the following entry points.
 */

one_block *
enqueue(
	one_block *me,
	void *payload
);

void *
dequeue(
	one_block *me
);

/* queue also has: peek as in stack, count, empty, and purge */

/*
 * a deque is built on a doubly linked list, but it should use the
 * following entry points.
 */

one_block *
push_back(
	one_block *me,
	void *payload
);

one_block *
push_front(
	one_block *me,
	void *payload
);

void *
pop_back(
	one_block *me
);

void *
pop_front(
	one_block *me
);

void *
peek_back(
	one_block *me
);

void *
peek_front(
	one_block *me
);

/* deque also has: count, empty, and purge */

/*
 * dynamic arrays are resizing arrays. in addition to make and free,
 * they support hbound via high_index, get, put. TODO: sort and func
 * for sort.
 */

int
high_index(
	one_block *me
);

one_block *
put_at(
	one_block *me,
	int n,
	void *payload
);

void *
get_at(
	one_block *me,
	int n
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TXBONE_H_ */
/* txbone.h ends here */
