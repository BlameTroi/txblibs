/* txbtree.h -- a binary search tree */

/*
 * a header only implementation of a scapegoat tree. this is a self
 * balancing binary search tree that rebalances only parts of the tree
 * that are too deep, which is a ratio of depth and log2 nodes which
 * gives the optimal depth.
 *
 * such trees are said to be loosely height and width balanced.
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
#include "txbalist.h"

#ifndef TXBTREE_H
#define TXBTREE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/********************************************************************
 * the scapegoat tree
 ********************************************************************/

/*
 * a self balancing binary search tree.
 *
 * externally this functions like any other binary search tree.
 *
 * keys and values can be of arbitrary types via the use of void *
 * pointers.
 *
 * there is one key:value pair per node. extra information for key
 * comparison and scapegoating is may be stored in the Tree block or
 * in the Nodes holding keys and values.
 *
 * i originally tried to use sedgewick's left-leaning red-black tree
 * but even after converting his java to c i can't get a working
 * solution. the best i got is that the my c of his java loses 23 keys
 * out of my 10k dataset. i have no idea why. after a couple of weeks
 * (yes, weeks) of tracing and banging my head, i gave up.
 *
 * next i looked at a plain red-black tree but the implementations are
 * fugly. it works but i don't really get procedure. at least with
 * sedgewick i understood what was going on.
 *
 * i had heard about scapegoat trees a while back and after reviewing
 * some documentation on them i know i can put an implementation
 * together that will work and be comprehensible.
 */


/********************************************************************
 * all the client needs to know about our internals:
 ********************************************************************/

typedef struct Node Node;
typedef struct Tree Tree;


/********************************************************************
 * keys and comparisons
 ********************************************************************/

/*
 * a client can have any key for tree nodes that will fit into a `void
 * *` and that can be compared for order and equality. we provide
 * automatic support for integer keys and character string keys. for
 * integers, the client should cast the integer up to a `void *` from
 * a `long` to use the actual key and not a reference to it in the
 * `Tree`.
 *
 * for character strings, pass a `char *` as the key and `strcmp` will be
 * used for the comparison.
 *
 * if the client's key is a derived type, select `function` or
 * `client` as the key type and pass a function pointer to a function
 * that follows the conventions of `memcmp` for returning <0, 0,
 * or >0 for left : right key ordering.
 *
 * a typedef for key_comparator is provided for convenience. the api is
 * that of `strcmp` and `memcmp`.
 *
 * left < right ... return < 0
 * left = right ... return 0
 * left > right ... return > 0
 */

typedef int (*key_comparator)(
	const void *left,           /* usually the key argument to api  */
	const void *right           /* usually the key of a `Tree` item */
);

enum Key_Type {
	kt_unknown = 0,             /* always have an invalid value  */
	INTEGER_KEY,                /* an integer cast to a (void *) */
	STRING_KEY,                 /* a (char *)                    */
	CUSTOM_KEY                  /* user provides function        */
};
typedef enum Key_Type Key_Type;


/********************************************************************
 * api -- operations at the tree level
 ********************************************************************/

/*
 * create and return a Tree instance with the specified key type.
 */

Tree *
make_Tree(
	Key_Type kt,
	key_comparator func_or_NULL
);

/*
 * free the Tree instance, releasing all storage managed by the `Tree`
 * library. client data referenced via key and value pointers is not
 * freed, that being the client's responsibility.
 */

Tree *
free_Tree(
	Tree *tree
);

/*
 * do a full rebalance of the tree on demand. this can
 * be time consuming but is available for those times
 * when you are done updating the tree and wish to speed
 * up access times.
 */

Tree *
rebalance_Tree(
	Tree *tree
);

/********************************************************************
 * information about the tree and its contents
 ********************************************************************/

/*
 * is the tree empty?
 */

bool
is_empty(
	Tree *tree
);

/*
 * how many key:value pairs are in the tree.
 */

int
count(
	Tree *tree
);

/*
 * does an item with this key exist in the Tree?
 */

bool
exists(
	Tree *tree,
	void *key
);


/********************************************************************
 * the four usual operations on 'records' or key:value pairs
 ********************************************************************/

/*
 * functions that update the tree return a boolean to report their
 * success or failure.
 *
 * `get` returns the value if the key is found. as `NULL` does not have
 * special meaning for keys and values, a `NULL` could be returned.
 *
 * to check existence without worrying about `NULL` values, use the `exists`
 * function.
 */

bool
insert(
	Tree *self,
	void *key,
	void *value
);

void *
get(
	Tree *tree,
	void *key
);

bool
update(
	Tree *tree,
	void *key,
	void *value
);

bool
delete (
	Tree *tree,
	void *key
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

/*
 * the traversals return an `int`, but its value is meaningless.
 */

int
pre_order_traversal(
	Tree *tree,
	void *context,
	fn_traversal_cb fn
);

int
in_order_traversal(
	Tree *tree,
	void *context,
	fn_traversal_cb fn
);

int
post_order_traversal(
	Tree *tree,
	void *context,
	fn_traversal_cb fn
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BTREE_H */


#ifdef TXBTREE_INTERNAL_H
#undef TXBTREE_INTERNAL_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * these are internal and should not be used by
 * client programs.
 */

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
	Key_Type kt;                /* are provided at creation      */
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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* internal */
/* btree.h ends here */
