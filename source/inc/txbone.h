/* txbone.h -- one data structure library to rule them all */
#ifndef _LISTING_H_
#define _LISTING_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>

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

#define DYNARRAY_DEFAULT_CAPACITY 512

typedef struct one_block one_block;

typedef union one_details one_details;

typedef struct one_deque one_deque;

struct one_deque {
	void *front;
	void *back;
};

typedef struct one_queue one_queue;

struct one_queue {
	void *front;
	void *back;
};

typedef struct sgl_item sgl_item;

struct sgl_item {
	sgl_item *next;
	void *payload;
};

/*
 * singly linked list
 * stack
 */

typedef struct one_singly one_singly;

typedef struct one_singly one_stack;

struct one_singly {
	sgl_item *first;
};

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

typedef struct one_dynarray one_dynarray;

struct one_dynarray {
	int length;                 /* last used via put */
	int capacity;               /* current maximum capacity */
	void **array;
};

typedef struct one_bst one_bst;

struct one_bst {
	void *something;
};

typedef struct one_keyval one_keyval;

struct one_keyval {
	void *something;
};

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
 * stack is implemented on a singly linked list, but it should use the
 * following entry points.
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
 * a queue (fifo) is implemented on a doubly linked list, but it should
 * use the following entry points.
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
 * a deque is built on a doubly linked list, but it should use the following
 * entry points.
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
 * dynamic arrays are resizing arrays. in addition to make and free, they
 * support hbound, get, put. TODO: sort and func for sort.
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

#endif /* _LISTING_H_ */
/* txbone.h ends here */
