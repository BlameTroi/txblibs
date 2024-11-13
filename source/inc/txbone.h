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
 * this is completely arbitrary. some implementations actually start
 * at 1 or 2.
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
 * the declarations and definitions of the supported data structures.
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

typedef one_singly
one_stack;      /* so this typedef indicates a stack is a singly */

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
	one_deque deq;               /* actually doubly linked list under the covers */
	one_queue que;               /* same */
	one_stack stk;               /* actually a singly linked list under the covers */
	one_singly sgl;              /* singly linked list */
	one_doubly dbl;              /* doubly linked list */
	one_dynarray dyn;            /* dynamically resizing array */
	one_bst bst;                 /* binary search tree */
	one_keyval kvl;              /* key:value store */
};

/*
 * the 'one_block' is a control block used as a handle for the client
 * code. it is kept small and the details of each specific data
 * structure are kept here in common via a union.
 */

struct one_block {
	enum one_type isa;           /* this is-a what? */
	int odometer;                /* call odometer */
	char tag[ONE_TAG_LEN];       /* eye catcher for those of us who remember core dumps */
	one_details u;               /* what data structure sits under this instance? */
};

/*
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
);

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
);

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
);

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
);

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
);

/*
 * these are possibly likely entry points for all the data structure.
 * they are definitely the entry points for singly and doubly linked
 * lists.
 *
 * these will be rescoped to static if i detrmine that the singly and
 * doubly linked lists provide no value on their own outside of of
 * backing the other structures.
 */

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
);

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
);

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
);

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
);

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
);

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
);

/*
 * a stack is implemented on a singly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * peek, and purge.
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
 * a queue (fifo) is implemented on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * count, peek, and purge.
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

/*
 * a deque (f/l-ifo)is built on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, empty,
 * count, and purge.
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
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TXBONE_H_ */
/* txbone.h ends here */
