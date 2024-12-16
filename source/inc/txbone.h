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

#ifndef TXBONE_H
#define TXBONE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

/*
 * supported types so far are singly and doubly linked lists, queues,
 * deques, stacks, dynamic arrays, something i call an accumulator
 * list, and a key:value store.
 *
 * the backing structures for these are singly linked lists, doubly
 * linked lists, dynamic arrays, and a self balancing binary search
 * tree (scapegoat stratgey).
 *
 * if needed, hashes, bags and sets could be added.
 *
 * all types store client payloads, as void * pointer sized values.
 * memory management of client payloads are the responsibility of the
 * client.
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
	unknown = 0,   /* notes on backing: */
	deque,         /* linked list */
	queue,         /* linked list */
	stack,         /* linked list */
	singly,        /* linked list */
	doubly,        /* linked list */
	alist,         /* accumulator list */
	dynarray,      /* array */
	keyval,        /* scapegoat tree */
	pqueue,        /* priority queue */
	unknowable
};
typedef enum one_type one_type;

#define ONE_TYPE_MAX unknowable

#define ONE_TAG_LEN 24

/*
 * these are completely arbitrary. some implementations actually start
 * at 1 or 2.
 */

#ifndef DYNARRAY_DEFAULT_CAPACITY
#define DYNARRAY_DEFAULT_CAPACITY 512
#endif
#ifndef ALIST_DEFAULT_CAPACITY
#define ALIST_DEFAULT_CAPACITY 100
#endif

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
 * an accumulator list is a small subset of an array
 * list. it's lighter than a dynamic array even though
 * they are similar. it's intended use is as an accumulator
 * during recursion, as you might see in lisp or sml.
 *
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
 * one_block *xs = make_one(alist);
 * one_block *remember = xs;
 * for (int i = 0; i > ALIST_DEFAULT_CAPACITY * 2; i++)
 *         xs = cons_alist(xs, (uintptr_t)i);
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

typedef struct one_alist one_alist;
struct one_alist {
	int capacity;                /* starts at ALIST_DEFAULT_CAP */
	int used;                    /* how many things are there   */
	uintptr_t *list;             /* open eneded array to anchor */
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

enum key_type {
	kt_unknown,
	integral,
	string,
	custom
};
typedef enum key_type key_type;

typedef struct one_bst one_bst;

struct one_bst {
	void *not_implemented_yet;
};

typedef int (*key_comparator)(
	const void *left,           /* usually the key argument to api  */
	const void *right           /* usually the key of a `Tree` item */
);

/********************************************************************
 * tree traversal (iteration)
 ********************************************************************/

/*
 * for iteration the usual pre_, in_, and post_ order traversal
 * functions are available. the client provides a callback function
 * and optionally a pointer to some context. in the api it's a `void *`,
 * so if the data will fit in `sizeof(void *)`, it can be used directly
 * by the client.
 *
 * the callback function receives the key, value, and client context
 * field once for each key in the Tree.
 *
 * the key, value, and context fields may be updated. the context value
 * persists across node callbacks. key and value are not explicitly
 * posted back to the tree but as it's all in memory changes are immediate.
 *
 * all that said, you really *really* shouldn't update the key.
 *
 * the callback function should return a boolean. `true` to continue
 * the traversal or `false` to terminate it.
 */

typedef bool (*fn_traversal_cb)(
	void *key,
	void *value,
	void *context,
	void *reserved1,
	void *reserved2
);

typedef struct Node Node;
typedef struct Tree Tree;

/********************************************************************
 * detailed node and tree declarations
 ********************************************************************/

/*
 * all you really need for a scapegoat tree are keys, values, and
 * child pointers. the red-black tree would add a bit for color.
 * for tree walking i find having a parent pointer helpful.
 */

struct Node {
	Node *left;                 /* these four fields are all you */
	Node *right;                /* really need for a sgt.        */
	void *key;                  /* ..                            */
	void *value;                /* ..                            */
	Node *parent;               /* not required but helpful      */
	bool deleted;               /* defer deletes to rebalance    */
};

/*
 * controling information and instrumentation for an instance are
 * stored here.
 */

#define ALPHA 1.5
struct Tree {
	Node *root;                 /* a tree grows here             */
	key_comparator fn_cmp;      /* comparator function and type  */
	key_type kt;                /* are provided at creation      */
	bool rebalance_allowed;     /* mosty for testing             */
	int odometer;               /* actual api call count         */
	int nodes;                  /* logical node count            */
	int inserts;                /* count of specific api calls,  */
	int deletes;                /* used to decide when to        */
	int updates;                /* rebalance                     */
	int marked_deleted;         /* actual node removal deferred  */
	int rebalances;             /* how many?                     */
	void *transient1;           /* pointers for transient tasks  */
	void *transient2;           /* ..                            */
	void *transient3;           /* ..                            */
};

/*
 * a key value store.
 */

typedef Tree one_keyval;

typedef struct one_dynarray one_pqueue;

/* not yet implemented place holders */
/* typedef struct one_hash one_hash; */
/* struct one_hash { void *not_implemented_yet;}; */
/* typedef struct one_heap one_heap; */
/* struct one_heap { void *not_implemented_yet;}; */
/* typedef struct one_dictionary one_dictionary; */
/* struct one_dictionary { void *not_implemented_yet;}; */
/* typedef struct one_bag one_bag; */
/* struct one_bag { void *not_implemented_yet;}; */
/* typedef struct one_set one_set; */
/* struct one_set { void *not_implemented_yet;}; */
/* typedef struct one_ordered one_ordered; */
/* struct one_ordered { void *not_implemented_yet;}; */

/***
 * rather than have separate high level control blocks, this union
 * approach allows for a cleaner interface and less redundancy.
 */

union one_details {
	one_deque deq;               /* actually doubly linked list under the covers */
	one_queue que;               /* same */
	one_stack stk;               /* actually a singly linked list under the covers */
	one_singly sgl;              /* singly linked list */
	one_doubly dbl;              /* doubly linked list */
	one_alist acc;               /* accumulator list */
	one_dynarray dyn;            /* dynamically resizing array */
	one_keyval kvl;              /* key:value store */
	one_pqueue pqu;              /* priority queue */
};

/*
 * the 'one_block' is a control block used as a handle for the client
 * code. think of it as 'an instance of a <whatever>'. it is kept
 * small and the details of each specific data structure are hidden
 * behind a union.
 */

struct one_block {
	one_type isa;                /* this is-a what? */
	//      int odometer;                /* call odometer */
	char tag[ONE_TAG_LEN];       /* eye catcher for those of us who remember core dumps */
	one_details u;               /* what data structure sits under this instance? */
};

/**
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

/**
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


one_block *
make_one_keyed(one_type isa, key_type kt, key_comparator fncb);

/**
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
);

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
);

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
);

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
);

/**
 * clone
 *
 * return a copy of the current structure. the original is
 * unchanged.
 */

one_block *
clone(
	one_block *ob
);




/**
 * these are possibly likely entry points for many of the data
 * structure. they are definitely the entry points for singly and
 * doubly linked lists.
 *
 * these will be rescoped to static if i detrmine that the singly and
 * doubly linked lists provide no value on their own outside of of
 * backing the other structures.
 */

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
);

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
);

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
);

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
);

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
);

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
);

/**
 * a stack is implemented on a singly linked list, but use the
 * following entry points in addition to make_one, free_one, is_empty,
 * and purge.
 */

/**
 * push
 *
 * an item onto the stack.
 */

one_block *
push(
	one_block *ob,
	void *payload
);

/**
 * pop
 *
 * an item off of the stack.
 */

void *
pop(
	one_block *ob
);

/**
 * peek
 *
 * return but do hot remove the top item on the stack.
 */

void *
peek(
	one_block *ob
);

/**
 * depth
 *
 * an idiomatic count.
 */

int
depth(
	one_block *ob
);

/**
 * a queue (fifo) is implemented on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, is_empty,
 * count, and purge.
 *
 * peek is available and pulls from the top of the queue.
 */

/**
 * enqueue
 *
 * add an item to the back of the queue.
 */

one_block *
enqueue(
	one_block *ob,
	void *payload
);

/**
 * dequeue
 *
 * remove an item from the front of the queue.
 */

void *
dequeue(
	one_block *ob
);

/**
 * a deque (f/l-ifo)is built on a doubly linked list, but use the
 * following entry points in addition to make_one, free_one, is_empty,
 * count, and purge.
 *
 * a deque allows you to work from either end of the queue.
 */

/**
 * push_back, _front
 *
 * add an item to either end of the deque.
 */

one_block *
push_back(
	one_block *ob,
	void *payload
);

one_block *
push_front(
	one_block *ob,
	void *payload
);

/**
 * pop_back, _front
 *
 * pull an item from either end of the deque.
 */

void *
pop_back(
	one_block *ob
);

void *
pop_front(
	one_block *ob
);

/**
 * peek_back, _front
 *
 * also from either end.
 */

void *
peek_back(
	one_block *ob
);

void *
peek_front(
	one_block *ob
);

/**
 * accumulator lists are self expanding lists of uintptr_t sized
 * things. its api and intended use are as accumulators for
 * recursions, as you might see in lisp or sml.
 *
 * uintptr_t made more sense that void *, but it's all the same
 * under the covers. cast as you need to.
 */

one_block *
cons(
	one_block *ob,
	uintptr_t atom
);

/**
 * car
 *
 * return the first (0th) item from the list. does not alter the
 * list.
 *
 * equivalent to nth(list, 0);
 */

uintptr_t
car(
	one_block *ob
);

/**
 * cdr
 *
 * 'coulder', return a new alist with all the items except the first
 * one. the original list is unchanged.
 *
 * equivalent to slice(list, 1, count(list));
 */

one_block *
cdr(
	one_block *ob
);

/**
 * append
 *
 * append the contents of one structure to another. the structures
 * must be of the same type. this returns a new copy of the holding all
 * the items of both structures.
 */

one_block *
append(
	one_block *ob,
	one_block *other
);

/**
 * slice
 *
 * return a copy of the list holding the items [from, to) (0 based).
 *
 * the original list is unchanged, the slice is a proper alist.
 */

one_block *
slice(
	one_block *ob,
	int from_inclusive,
	int to_exclusive
);

/**
 * setnth
 *
 * replace the 'n'th item (0 based) in the list with this
 * new item. the list is otherwise unchanged.
 */

one_block *
setnth(
	one_block *ob,
	int n,
	uintptr_t atom
);

/**
 * nth
 *
 * return the 'n'th item (0 based) from the list. does not
 * alter the list.
 */

uintptr_t
nth(
	one_block *ob,
	int n
);

/**
 * iterate
 *
 * a simple iterator. set the starting index and call repeatedly.
 * sets the index to -1 when all items have been passed.
 */

uintptr_t
iterate(
	one_block *ob,
	int *idx
);

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
	one_block *ob
);

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
	one_block *ob,
	void *payload,
	int n
);

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
	one_block *ob,
	int n
);

/**
 * key:value -- this is built on a tree
 */

bool
insert(one_block *, void *, void *);

void *
get(one_block *, void *);

bool
update(one_block *, void *, void *);

one_block *
delete (one_block *, void *);

void *
min_key(one_block *);

void *
max_key(one_block *);

int
in_order_keyed(one_block *ob, void *context, fn_traversal_cb fn);

one_block *
keys(one_block *);

one_block *
values(one_block *);

bool
exists(one_block *, void *);

/**
 * priority queue -- probably built on a double linked list with a key.
 */
// is_empty

one_block *
add_with_priority(one_block *, long, void *);

one_block *
add_with_max(one_block *, void *);

one_block *
add_with_min(one_block *, void *);

long
max_priority(one_block *);

long
min_priority(one_block *);

bool
get_max(one_block *, long *, void *);

bool
get_min(one_block *, long *, void *);

bool
peek_max(one_block *, long *, void *);

bool
peek_min(one_block *, long *, void *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TXBONE_H_ */
/* txbone.h ends here */
