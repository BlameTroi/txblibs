/* btree.c -- code for btree experiments -- */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "txballoc.h"
#include "txbmisc.h"
#include "txblog2.h"
// #include "txbalist.h"
#include "txbone.h"

#define TXBTREE_INTERNAL_H
#include "txbtree.h"

/* a helpful macro for infrequently needed traces */
#ifndef FPRINTF_INFO
#define FPRINTF_INFO if (false)
#endif

/*
 * a self balancing binary search tree.
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
 * implementation notes -- error handling for broken or 'impossible'
 * link sequences
 *
 * i'm leaving 'holes in the bucket' and not erroring on situations
 * that should not or can not occur. these are things like impossible
 * sequences of links (eg., root node has a parent, non-root node
 * doesn't, etc.).
 *
 * we'll deal with those cases as bugs to be fixed elswhere.
 */


/********************************************************************
 ********************************************************************
 *
 * difficult to avoid forward references
 *
 * these should be all of the internal functions and they are
 * meant to be static.
 *
 ********************************************************************
 ********************************************************************/

static
void
node_free(Tree *, Node *);

static
int
node_children_free(Tree *, Node *);

static
Node *
get_Node_or_parent(Tree *, void *);

static
Node *
get_Node_or_NULL(Tree *, void *);

static
Node *
make_Node(Tree *, void *, void *);

static
int
pre_order_traversal_r(Tree *, Node *, void *, fn_traversal_cb);

static
int
in_order_traversal_r(Tree *, Node *, void *, fn_traversal_cb);

static
int
post_order_traversal_r(Tree *, Node *, void *, fn_traversal_cb);

static
int
height(Tree *, Node *);

static
int
size(Tree *, Node *);

static
bool
is_unbalanced(Tree *, Node *);

static
bool
is_scapegoat(Tree *, Node *);

static
bool
btree_insert_r(Tree *, Node *, Node *);

static
Node *
rebalance_r(Tree *, Node *);

static
void
reset_subtree_r(Tree *, Node *);

static
Node *
make_subtree_r(Tree *, one_block *);

static
one_block *
internal_collector_r(Tree *, Node *, one_block *);


/********************************************************************
 ********************************************************************
 *
 * external api and their immediate helpers
 *
 ********************************************************************
 ********************************************************************/


/********************************************************************
 * key comparison
 ********************************************************************/

/*
 * as keys and values are most usually passed as `void *` pointers, the
 * client must let us know how to compare the keys. the comparator
 * function has the same basic interface as the standard `strcmp`.
 *
 * the key's type and comparator function are set at tree creation.
 *
 * integral and string compares are set automatically to either
 * `integral_comp` or `strcmp`.
 *
 * argument 'left' tends to be searched for key, and argument 'right'
 * tends to be the key of the node being checked against.
 */

static
int
integral_comp(
	void *left,
	void *right
) {
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
keycmp(
	Tree *self,
	void *left,
	void *right
) {
	int cmp = self->fn_cmp(left, right);
	if (cmp < 0) return LESS;
	else if (cmp > 0) return GREATER;
	else return EQUAL;
}


/********************************************************************
 * create and destroy a tree
 ********************************************************************/

/*
 * create a new empty tree instance.
 */

Tree *
make_Tree(
	Key_Type kt,
	key_comparator func_or_NULL
) {
	Tree *self = tsmalloc(sizeof(*self));
	memset(self, 0, sizeof(*self));
	self->rebalance_allowed = true;

	self->kt = kt;
	switch (kt) {

	case INTEGER_KEY:     /* treat the key as a void * sized integer, a long */
		self->fn_cmp = (key_comparator)integral_comp;
		if (func_or_NULL)
			fprintf(stderr,
				"WARNING make_Tree: client provided comparator function for integral keys ignored.\n");
		break;

	case STRING_KEY:      /* strings are standard char * bytestrings */
		self->fn_cmp = (key_comparator)strcmp;
		if (func_or_NULL)
			fprintf(stderr,
				"WARNING make_Tree: client provided comparator function for string keys ignored.\n");
		break;

	case CUSTOM_KEY:    /* client provides comparator */
		self->fn_cmp = func_or_NULL;
		if (self->fn_cmp)
			break;
		fprintf(stderr, "ERROR make_Tree: missing comparator function.\n");

	default:
		fprintf(stderr, "ERROR make_Tree: error in key type or function\n");
		tsfree(self);
		self = NULL;
	}

	return self;
}


/*
 * full Tree rebalance.
 */

Tree *
rebalance_Tree(
	Tree *self
) {
	if (!self || !self->root)
		return self;
	self->odometer += 1;
	rebalance_r(self, self->root);
	return self;
}

/*
 * free memory for the tree and its Nodes. returns the now invalid
 * pointer to the old tree.
 */

Tree *
free_Tree(
	Tree *self
) {
	int freed = 0;
	if (self->root) {
		freed += node_children_free(self, self->root);
		node_free(self, self->root);
		freed += 1;
	}
	memset(self, 253, sizeof(*self));
	tsfree(self);
	FPRINTF_INFO fprintf(stderr, "INFO free_Tree %d nodes freed\n", freed);
	return self;
}


/********************************************************************
 * insert a new Node into a Tree
 ********************************************************************/

Node *
get_Node_or_parent(
	Tree *self,
	void *key
) {
	/* we can only return a NULL if the tree is empty */
	if (!self->root)
		return NULL;

	/* walk to find node or parent */
	Node *prior = NULL;
	Node *curr = self->root;
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

Node *
get_Node_or_NULL(
	Tree *self,
	void *key

) {
	Node *n = get_Node_or_parent(self, key);
	if (!n) return NULL;
	if (keycmp(self, key, n->key) != EQUAL) return NULL;
	return n;
}

int
height(Tree *self, Node *n) {
	int height = 0;
	while (n->parent) {
		height += 1;
		n = n->parent;
	}
	return height;
}

int
size(Tree *self, Node *n) {
	if (!n) return 0;
	return 1 + size(self, n->left) + size(self, n->right);
}

bool
is_unbalanced(Tree *self, Node *n) {
	int h = height(self, n);
	int s = size(self, self->root);
	return (h > ALPHA * uint32_log2(s));
}

bool
is_scapegoat(Tree *self, Node *n) {
	return n && (3*size(self, n) > 2*size(self, n->parent));
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

bool
btree_insert_r(
	Tree *self,
	Node *parent,
	Node *new
) {

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
			node_free(self, new);
			return true;
		}
		node_free(self, new);
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
		node_free(self, new);
		return false;
	}

	return true;
}

bool
btree_insert(
	Tree *self,
	void *key,
	void *value
) {
	self->odometer += 1;
	Node *parent = get_Node_or_parent(self, key);
	Node *n = make_Node(self, key, value);
	bool did = btree_insert_r(self, parent, n);
	if (did) {
		self->nodes += 1;
		self->inserts += 1;
		if (self->rebalance_allowed && is_unbalanced(self, n)) {
			FPRINTF_INFO fprintf(stderr, "INFO insert: unbalanced@ %d %d %d %d %ld\n",
				self->nodes, self->inserts,
				size(self, self->root), height(self, n), (uintptr_t)key);
			Node *s = n->parent;
			while (s) {
				if (!is_scapegoat(self, s)) {
					s = s->parent;
					continue;
				}
				FPRINTF_INFO fprintf(stderr, "INFO insert:  scapegoat@ %d %d %d %d %ld\n",
					self->nodes, self->inserts,
					size(self, self->root), height(self, s), (uintptr_t)s->key);
				s = rebalance_r(self, s);
				break;
			}
			if (!s) FPRINTF_INFO fprintf(stderr, "INFO insert: no scapegoat found!\n");
		}
	}
	return did;
}


/********************************************************************
 * delete a Node from the Tree
 ********************************************************************/

/*
 * most deletes are deferred. rather than juggle pointers we'll drop the
 * deleted Nodes during a rebalance.
 */

bool
btree_delete(
	Tree *self,
	void *key
) {
	self->odometer += 1;
	Node *n = get_Node_or_NULL(self, key);
	if (n && n->deleted) {
		fprintf(stderr, "WARNING delete: key not found in tree.\n");
		return false;
	}

	/*
	 * if it's a terminal Node that isn't root, really delete it.
	 */

	if (!n->left && !n->right) {
		if (n->parent && n->parent->left == n) n->parent->left = NULL;
		if (n->parent && n->parent->right == n) n->parent->right = NULL;
		if (n->parent == NULL && self->root == n)
			self->root = NULL;
		n->parent = NULL;
		node_free(self, n);
		self->deletes += 1;
		self->nodes -= 1;
		return true;
	};

	/*
	 * flag the Node as deleted, it will be removed during rebalancing.
	 */

	n->deleted = true;
	n->value = NULL;
	self->marked_deleted += 1;
	self->deletes += 1;
	self->nodes -= 1;

	/*
	 * experiment, just rebalance on every delete, but only under node's
	 * parent.
	 */

	if (n->parent) {
		rebalance_r(self, n);
	} else {
		rebalance_r(self, self->root);
	}

	return true;
}


/********************************************************************
 * get and return the value associated with a key
 ********************************************************************/

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
btree_get(
	Tree *self,
	void *key
) {
	self->odometer += 1;
	Node *n = get_Node_or_NULL(self, key);
	if (!n || n->deleted) return NULL;
	return n->value;
}


/********************************************************************
 * update the value associated with a key
 ********************************************************************/

/*
 * if value is a pointer, this isn't strictly needed.
 */

bool
btree_update(
	Tree *self,
	void *key,
	void *value
) {
	self->odometer += 1;
	Node *n = get_Node_or_NULL(self, key);
	if (n == NULL) return false;
	n->value = value;
	self->updates += 1;
	return true;
}


/********************************************************************
 * predicates and queries on the Tree or its contents
 ********************************************************************/

bool
tree_is_empty(
	Tree *self
) {
	self->odometer += 1;
	return self->nodes == 0;
}

int
tree_count(
	Tree* self
) {
	self->odometer += 1;
	return self->nodes;
}

bool
btree_exists(
	Tree *self,
	void *key
) {
	self->odometer += 1;
	Node *n = get_Node_or_NULL(self, key);
	if (!n || n->deleted) return NULL;
	return n;
}


/********************************************************************
 * the standard tree traversal functions. these require a client
 * provided callback function. that function's api is documented in
 * the library header.
 ********************************************************************/

int
pre_order_traversal(
	Tree *self,
	void *context,
	fn_traversal_cb fn
) {
	self->odometer += 1;
	return pre_order_traversal_r(self, self->root, context, fn);
}

/*
 * in_order_traversal is also used during Tree rebalancing.
 */

int
in_order_traversal(
	Tree *self,
	void *context,
	fn_traversal_cb fn
) {
	self->odometer += 1;
	return in_order_traversal_r(self, self->root, context, fn);
}

int
post_order_traversal(
	Tree *self,
	void *context,
	fn_traversal_cb fn
) {
	self->odometer += 1;
	return post_order_traversal_r(self, self->root, context, fn);
}



/********************************************************************
 ********************************************************************
 *
 * client traversal recursions
 *
 ********************************************************************
 ********************************************************************/

int
pre_order_traversal_r(
	Tree *self,
	Node *node,
	void *context,
	fn_traversal_cb fn
) {
	if (!node) return 0;
	if (!node->deleted) fn(node->key, node->value, context, self, node);
	pre_order_traversal_r(self, node->left, context, fn);
	pre_order_traversal_r(self, node->right, context, fn);
	return 1;
}

int
in_order_traversal_r(
	Tree *self,
	Node *n,
	void *context,
	fn_traversal_cb fn
) {
	if (!n) return 0;
	in_order_traversal_r(self, n->left, context, fn);
	if (!n->deleted) fn(n->key, n->value, context, self, n);
	in_order_traversal_r(self, n->right, context, fn);
	return 1;
}

int
post_order_traversal_r(
	Tree *self,
	Node *n,
	void *context,
	fn_traversal_cb fn
) {
	if (!n) return 0;
	post_order_traversal_r(self, n->left, context, fn);
	post_order_traversal_r(self, n->right, context, fn);
	if (!n->deleted) fn(n->key, n->value, context, self, n);
	return 0;
}


/********************************************************************
 ********************************************************************
 *
 * Node creation and deletion
 *
 ********************************************************************
 ********************************************************************/

/*
 * create a new unlinked Node
 */

Node *
make_Node(
	Tree *self,
	void *key,
	void *value
) {
	Node *n = tsmalloc(sizeof(*n));
	memset(n, 0, sizeof(*n));
	n->key = key;
	n->value = value;
	return n;
}

/*
 * free an individual Node, warning if it has children and
 * clearing the parent's link to it if there is one.
 */

void
node_free(
	Tree *self,
	Node *n
) {
	/* warn on linked children but allow it through for now */
	if (n->left || n->right) {
		abort();
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
node_children_free(
	Tree *self,
	Node *n
) {
	if (!n) return 0;
	int freed = 0;
	if (n->left) {
		freed += node_children_free(self, n->left);
		n->left->left = NULL;
		n->left->right =NULL;
		freed += 1;
		node_free(self, n->left);
	}
	if (n->right) {
		freed += node_children_free(self, n->right);
		n->right->left = NULL;
		n->right->right =NULL;
		freed += 1;
		node_free(self, n->right);
	}
	return freed + 1;
}


/********************************************************************
 ********************************************************************
 *
 * rebalancing and analysis
 *
 ********************************************************************
 ********************************************************************/

/*
 * recursively snip off all the branches of a (sub)tree for the
 * rebuild.
 */

void
reset_subtree_r(Tree *self, Node *subtree) {
	if (!subtree)
		return;

	if (subtree->left) {
		reset_subtree_r(self, subtree->left);
		subtree->left = NULL;
	}
	if (subtree->right) {
		reset_subtree_r(self, subtree->right);
		subtree->right = NULL;
	}
	if (subtree->parent) {
		if (subtree->parent->left && subtree->parent->left == subtree)
			subtree->parent->left = NULL;
		if (subtree->parent->right && subtree->parent->right == subtree)
			subtree->parent->right = NULL;
		subtree->parent = NULL;
	}

	node_free(self, subtree);
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

Node *
make_subtree_r(
	Tree *self,
	one_block *nodes
) {
	int k = count(nodes);
	if (k == 0)
		return NULL;

	/* next subtree root */
	int j = k/2;
	Node *old = (Node *)nth(nodes, j);
	Node *new = make_Node(self, old->key, old->value);

	/* and to either side of the root, we have further subtrees */
	one_block *left_side = slice(nodes, 0, j);
	one_block *right_side = slice(nodes, j+1, k);

	/* so do the left children */
	new->left = make_subtree_r(self, left_side);
	free_one(left_side);
	if (new->left)
		new->left->parent = new;

	/* and the right children */
	new->right = make_subtree_r(self, right_side);
	free_one(right_side);
	if (new->right)
		new->right->parent = new;

	/* and return our subtree root */
	return new;
}

/*
 * an in order traversal that collects an alist of Nodes. this is
 * useful for rebalancing and tree analysis. note that deleted Nodes
 * are not collected, but their children are.
 */

one_block *
internal_collector_r(
	Tree *self,
	Node *n,
	one_block *xs
) {
	if (!n)
		return xs;
	xs = internal_collector_r(self, n->left, xs);
	if (!n->deleted)
		xs = cons(xs, (uintptr_t)n);
	xs = internal_collector_r(self, n->right, xs);
	return xs;
}

/*
 * once we know where we need to rebalance from, use the old (sub)tree
 * to build a balanced (sub)tree and and replace the old (sub)tree.
 *
 * to rebalance the whole Tree, pass self->root as the subtree.
 */

Node *
rebalance_r(
	Tree *self,
	Node *subtree
) {
	/* if (!self->rebalance_allowed) { */
	/*      fprintf(stderr, "\n*** REBALANCE SUPPRESSED ***\n"); */
	/*      return false; */
	/* } */

	FPRINTF_INFO fprintf(stderr, "INFO rebalance begin rebalancing\n");

	/* remember where to hang the subtree. if parent is NULL,
	 * this is root. if it isn't remember if the subtree was
	 * on the left or right. */

	Node *parent = subtree->parent;
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
	xs = internal_collector_r(self, subtree, xs);

	/* build a new subtree by going over the node list in the
	 * optimal order for insertion: that is, repeatedly halving
	 * the list to create the ordered list and then linking the
	 * nodes into a subtree. */

	FPRINTF_INFO fprintf(stderr, "INFO rebalance nodes in subtree %d\n", count(xs));
	Node *new_subtree = make_subtree_r(self, xs);
	free_one(xs);

	/* destroy the old nodes and replace them with the new
	 * nodes. */

	reset_subtree_r(self, subtree);

	if (!parent) {
		self->root = new_subtree;
	} else {
		if (left_side) parent->left = new_subtree;
		else           parent->right = new_subtree;
	}

	FPRINTF_INFO fprintf(stderr, "INFO rebalance end rebalancing\n");
	return new_subtree;
}

/* btree.c ends here */
