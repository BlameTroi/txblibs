/* txbone.h -- one data structure library to rule them all */

/*
 * a header only implementation several data structures (linked lists,
 * queues, dynamic arrays, etc.) that i have written while working on
 * advent of code problems. this is a 'grand unified' library as i
 * rewrite several of the prior implementations for consistency and
 * some actual reuse. along the way i've added simplistic memory leak
 * tracking.
 *
 * this library has one external dependency, and that's my memory leak
 * tracker 'txballoc.h'. it is provided as a single header library
 * as well.
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
 * all store client data as pointer sized items, either as `void *` or
 * `uintptr_t`. memory management of these items are the
 * responsibility of client.
 *
 * errors return invalid values (negatives or NULLs, see each
 * function) and can print a diagnostic message on stderr.
 */

/**
 * configurable settings
 *
 * you may tune these to change various defaults and limits.
 *
 * no reasonableness check is performed. they're your feet, shoot
 * them off if you like.
 */

/*
 * dynamic arrays grow by doubling. while some libraries start at the
 * minimum of 1 or 2, if you are using one you probably want more than
 * some tens of items.
 */

#ifndef ONE_DYNARRAY_DEFAULT_CAPACITY
#define ONE_DYNARRAY_DEFAULT_CAPACITY 512
#endif

/*
 * accumulator lists grow in chunks of their default capacity. a value
 * of 100 has worked well for my tests.
 */

#ifndef ONE_ALIST_DEFAULT_CAPACITY
#define ONE_ALIST_DEFAULT_CAPACITY 100
#endif

/*
 * key:value stores are backed by a binary search tree. rebalancing is
 * not strict or absolute since i'm using the scapegoat tree approach.
 *
 * ONE_REBALANCE_ALPHA is the fudge factor for rebalance triggering on
 * insertion. the optimal depth of a bst is log2(N). multiply by this
 * to find the depth that should trigger a rebalance. this is a
 * partial rebalance that finds a scapegoat along the current path
 * from root to a node, and rebalances the subtree under that
 * scapegoat.
 *
 * ONE_REBALANCE_DELETE_PERCENT controls rebalancing after multiple
 * deletions from the bst. if the accumulated number of nodes deleted
 * is this percent of the current nodes in the bst, perform a full
 * rebalance of the tree. deletes are deferred and deleted nodes are
 * not removed until a full rebalance is done.
 *
 * it's a garbage collection.
 */

#ifndef ONE_REBALANCE_ALPHA
#define ONE_REBALANCE_ALPHA 1.5
#endif

#ifndef ONE_REBALANCE_DELETE_PERCENT
#define ONE_REBALANCE_DELETE_PERCENT 10
#endif

#ifndef ONE_REBALANCE_MINIMUM
#define ONE_REBALANCE_MINIMUM 64
#endif

/**
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

/**
 * core data structures/control blocks
 *
 * i prefer working with typedefs of structs. the one_block carries
 * all the information needed for an instance of any of the data
 * structures in these libraries.
 *
 * the one_details is a union of all the lower level control blocks.
 */

typedef struct  one_block     one_block;

typedef union   one_details   one_details;

typedef struct  sgl_item      sgl_item;
typedef struct  one_singly    one_singly;
typedef struct  dbl_item      dbl_item;
typedef struct  one_doubly    one_doubly;
typedef         one_singly    one_stack;
typedef         one_doubly    one_deque;
typedef         one_doubly    one_queue;
typedef struct  one_alist     one_alist;
typedef struct  one_dynarray  one_dynarray;
typedef struct  one_node      one_node;
typedef struct  one_tree      one_tree;
typedef         one_tree      one_keyval;
typedef struct  one_dynarray  one_pqueue;

/*
 * a singly linked list and its nodes.
 */

struct sgl_item {
	sgl_item *next;
	void *item;
};

struct one_singly {
	sgl_item *first;
};

/*
 * a doubly linked list and it's nodes.
 */

struct dbl_item {
	dbl_item *next;
	dbl_item *previous;
	void *item;
};

struct one_doubly {
	dbl_item *first;
	dbl_item *last;
};

/*
 * a stack (lifo) is just an different api for a singly linked list.
 */

/*
 * both a deque (double ended queue) and queue (fifo) are just
 * different apis for a doubly linked list.
 */

/*
 * the dynamic array is aa dynamically resizing array.
 */

struct one_dynarray {
	int length;                 /* last used via put */
	int capacity;               /* current maximum capacity */
	void **array;               /* and where can it be now */
};

/*
 * an accumulator list provides a small subset of array list
 * functionality. it is lighter than a dynamic array even though they
 * are similar. its intended use is as an accumulator during
 * recursion, as you might see in lisp or sml.
 */

struct one_alist {
	int capacity;                /* starts at ALIST_DEFAULT_CAP */
	int used;                    /* how many things are there   */
	uintptr_t *list;             /* open eneded array to anchor */
};

/*
 * a balanced binary search tree is used for managing keyed data. i
 * used the scapegoat tree approach for balancing.
 *
 * so far this only provides backing for the key:value store, an
 * associative array.
 *
 * all you really need for a scapegoat tree are keys, values, and
 * child pointers. for tree walking i find having a parent pointer
 * helpful but it isn't strictly needed.
 *
 * delete's are deferred so a deleted flag is needed.
 */

struct one_node {
	one_node *left;             /* these four fields are all you */
	one_node *right;            /* really need for a sgt.        */
	void *key;                  /* ..                            */
	void *value;                /* ..                            */
	one_node *parent;           /* not required but helpful      */
	bool deleted;               /* defer deletes to rebalance    */
};

/*
 * the api for comparing key values is that of memcmp or strcmp.
 */

typedef int (*one_key_comparator)(
	const void *left,           /* usually the key argument to api  */
	const void *right           /* usually the key of a `Tree` item */
);

enum one_key_type {
	kt_unknown,
	integral,                   /* keys are signed longs */
	string,                     /* keys are standard strings */
	custom                      /* keys require a custom function */
};
typedef enum one_key_type one_key_type;

/*
 * the scapegoat tree. in addition to the root pointer and key
 * comparison information counters are kept to help decide when
 * to rebalance the tree.
 */

struct one_tree {
	one_node *root;             /* a tree grows here             */
	one_key_comparator fn_cmp;  /* comparator function and type  */
	one_key_type kt;            /* are provided at creation      */
	bool rebalance_allowed;     /* mosty for testing             */
	int nodes;                  /* might drop this               */
	int inserts;                /* count of specific api calls,  */
	int deletes;                /* used to decide when to        */
	int updates;                /* rebalance                     */
	int marked_deleted;         /* actual node removal deferred  */
	int full_rebalances;        /* how many?                     */
	int partial_rebalances;     /* just for fun                  */
};

/*
 * a key value store, or an associative array, is just an api over
 * a binary search tree.
 */

/*
 * a priority queue. it relies on some of the key comparison suppport
 * in the binary search tree. the keys don't have to be unique, but
 * we do want to properly order them.
 */

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

/**
 * the 'one_block' is a control block used as a handle for the client
 * code. think of it as 'an instance of a <whatever>'. it is kept
 * small and the details of each specific data structure are hidden
 * behind a union.
 */

struct one_block {
	one_type isa;                /* this is-a what? */
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
 * make_one -- all but keyval and pqueue
 *
 * create an instance of one of the data structure types. allocates and
 * initializes the 'one block' and returns it to the client. the client
 * passes this back on subsequent calls as a handle.
 *
 * a constructor, if you will.
 */

one_block *
make_one(
	enum one_type isa
);

/**
 * make_one_keyed -- keyval, pqueue
 *
 * as `make_one` but with key information. for integral and string
 * key types, the comparator function should be NULL.
 */

one_block *
make_one_keyed(
	one_type isa,
	one_key_type kt,
	one_key_comparator fncb
);

/**
 * free_one -- all
 *
 * destroy an instance of a data structure, releasing library managed
 * memory.
 */

one_block *
free_one(
	one_block *ob
);

/**
 * count -- all
 *
 * how many things are managed by the data structure. for a stack, use
 * depth. has no meaning for a dynamic array.
 *
 * returns the count or -1 on error.
 */

int
count(
	one_block *ob
);

/**
 * is_empty -- all
 *
 * predicate is this data structure empty (count/depth == 0)?
 */

bool
is_empty(
	one_block *ob
);

/**
 * purge -- all but dynarray
 *
 * empty the data structure. deletes all storage for items/nodes
 * managed by the structure. client data is left alone. this has no
 * meaning for a dynamic array.
 *
 * returns number of items deleted or -1 on error.
 */

int
purge(
	one_block *ob
);

/**
 * clone -- all
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
 * add_first -- singly
 *
 * add an item to the front/top of all items held.
 *
 * returns NULL on error.
 */

one_block *
add_first(
	one_block *ob,
	void *item
);

/**
 * add_last -- singly
 *
 * add an item to the back/bottom of all items held.
 *
 * returns NULL on error.
 */

one_block *
add_last(
	one_block *ob,
	void *item
);

/**
 * peek_first -- singly
 *
 * return but do not remove the item at the front/top of all
 * items held.
 *
 * returns NULL on error.
 */

void *
peek_first(
	one_block *ob
);

/**
 * peek_last -- singly
 *
 * return but do not remove the item at the back/bottom of all
 * items held.
 *
 * returns NULL on error.
 */

void *
peek_last(
	one_block *ob
);

/**
 * get_first -- singly
 *
 * remove and return the item at the front/top of all items held.
 *
 * returns NULL on error.
 */

void *
get_first(
	one_block *ob
);

/**
 * get_last -- singly
 *
 * remove and return the item at the back/bottom of all items held.
 *
 * returns NULL on error.
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
 * push -- stack
 *
 * an item onto the stack.
 *
 * returns NULL on error.
 */

one_block *
push(
	one_block *ob,
	void *item
);

/**
 * pop -- stack
 *
 * an item off of the stack.
 *
 * returns NULL on error.
 */

void *
pop(
	one_block *ob
);

/**
 * peek -- stack
 *
 * return but do hot remove the top item on the stack.
 *
 * returns NULL on error.
 */

void *
peek(
	one_block *ob
);

/**
 * depth -- stack
 *
 * an idiomatic count.
 *
 * returns NULL on error.
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
 * enqueue -- queue
 *
 * add an item to the back of the queue.
 *
 * returns NULL on error.
 */

one_block *
enqueue(
	one_block *ob,
	void *item
);

/**
 * dequeue
 *
 * remove an item from the front of the queue.
 *
 * returns NULL on error.
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
 *
 * returns NULL on error.
 */

one_block *
push_back(
	one_block *ob,
	void *item
);

one_block *
push_front(
	one_block *ob,
	void *item
);

/**
 * pop_back, _front
 *
 * pull an item from either end of the deque.
 *
 * returns NULL on error.
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
 *
 * returns NULL on error.
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
 * using uintptr_t instead of void * worked out better here.
 *
 * the api convention is the returned list is meant to replace the
 * primary list in arguments. if the list has been significantly
 * mutated, the original primary list is freed and an updated copy is
 * returned.
 *
 * always use the array list you get back from an api call, not the
 * one you sent in. for example, in the following, the assert will
 * succeed as the ALIST_DEFAULT_CAPACITY'th pass through the for loop
 * will require an expansion of the array list.
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
 * the api is inspired by both the java array list and a lisp list.
 *
 * it's pseudo lisp lists with fake garbage collection.
 */

one_block *
cons(
	one_block *ob,
	uintptr_t atom
);

/**
 * car -- alist
 *
 * return the first (0th) item from the list. does not alter the
 * list.
 *
 * equivalent to nth(list, 0);
 *
 * returns NULL on error.
 */

uintptr_t
car(
	one_block *ob
);

/**
 * cdr -- alist
 *
 * 'coulder', return a new alist with all the items except the first
 * one. the original list is unchanged.
 *
 * equivalent to slice(list, 1, count(list));
 *
 * returns NULL on error.
 */

one_block *
cdr(
	one_block *ob
);

/**
 * append -- alist
 *
 * append the contents of one structure to another. the structures
 * must be of the same type. this returns a new copy of the holding all
 * the items of both structures.
 *
 * returns NULL on error.
 */

one_block *
append(
	one_block *ob,
	one_block *other
);

/**
 * slice -- alist
 *
 * return a copy of the list holding the items [from, to) (0 based).
 *
 * the original list is unchanged, the slice is a proper alist.
 *
 * returns NULL on error.
 */

one_block *
slice(
	one_block *ob,
	int from_inclusive,
	int to_exclusive
);

/**
 * setnth -- alist
 *
 * replace the 'n'th item (0 based) in the list with this
 * new item. the list is otherwise unchanged.
 *
 * returns NULL on error.
 */

one_block *
setnth(
	one_block *ob,
	int n,
	uintptr_t atom
);

/**
 * nth -- alist
 *
 * return the 'n'th item (0 based) from the list. does not
 * alter the list.
 *
 * returns NULL on error.
 */

uintptr_t
nth(
	one_block *ob,
	int n
);

/**
 * iterate -- alist
 *
 * a simple iterator. set the starting index and call repeatedly.
 * sets the index to -1 when all items have been passed.
 *
 * returns NULL on error.
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
 * high_index -- dynarray
 *
 * the highest used (via put_at) index in the array. while a payload
 * may be put anywhere with a non-negative index, a get is only
 * valid for an index in the range 0->high index.
 *
 * returns index or -1 on error.
 */

int
high_index(
	one_block *ob
);

/**
 * put_at -- dynarray
 *
 * place a payload at a particular index in the array. if the array's
 * capacity is less than the index, double the capacity until the
 * index is valid.
 *
 * returns NULL on error.
 */

one_block *
put_at(
	one_block *ob,
	void *item,
	int n
);

/**
 * get_from -- dynarray
 *
 * return the payload from a particular index in the array. if the index
 * is either negative or greater than high_index, it is an error. if
 * the index is between 0 .. high_index (inclusive) but nothing has been
 * put_at that index yet, return NULL.
 *
 * returns NULL on error.
 */

void *
get_from(
	one_block *ob,
	int n
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
	one_tree *self
);

/**
 * key:value -- this is built on the scapegoat binary search tree.
 *
 * returns NULL on error.
 */

bool
insert(
	one_block *ob,
	void *key,
	void *value
);

void *
get(
	one_block *ob,
	void *key
);

bool
update(
	one_block *ob,
	void *key,
	void *value
);

one_block *
delete (
	one_block *ob,
	void *key
);

bool
exists(
	one_block *ob,
	void *key
);

void *
min_key(
	one_block *ob
);

void *
max_key(
	one_block *ob
);

int
in_order_keyed(
	one_block *ob,
	void *context,
	fn_traversal_cb fn);

one_block *
keys(
	one_block *ob
);

one_block *
values(
	one_block *ob
);

/**
 * priority queue -- probably built on a double linked list with a key.
 *
 * returns NULL on error.
 */

one_block *
add_with_priority(
	one_block *ob,
	long priority,
	void *item);

one_block *
add_with_max(
	one_block *ob,
	void *item
);

one_block *
add_with_min(
	one_block *ob,
	void *item
);

long
max_priority(
	one_block *ob
);

long
min_priority(
	one_block *ob
);

bool
get_max(
	one_block *ob,
	long *priority,
	void *item
);

bool
get_min(
	one_block *ob,
	long *priority,
	void *item
);

bool
peek_max(
	one_block *ob,
	long *priority,
	void *item);

bool
peek_min(
	one_block *ob,
	long *priority,
	void *item
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TXBONE_H_ */
/* txbone.h ends here */
