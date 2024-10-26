/* ll.h -- blametroi's simple singly linked list -- */

/*
 * a header only implementation of a singly linked list.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * an instance of a linked list.
 */

typedef struct llcb llcb;

/*
 * ppayload, pkey, pvalue
 *
 * these libraries manage client 'payloads'. these are void * sized
 * and are generally assumed to be a pointer to client managed data,
 * but anything that will fit in a void * pointer (typically eight
 * bytes) is allowed.
 *
 * it is the client's responsibility to free any of its dynamically
 * allocated memory. library code provides 'destroy' methods to clear
 * and release library data structures.
 *
 * these type helpers are all synonyms for void *.
 */

typedef void * pkey;
typedef void * pvalue;
typedef void * ppayload;

/*
 * ll_empty
 *
 * are there items in the linked list?
 *
 *     in: the ll instance
 *
 * return: boolean are there items?
 */

bool
ll_empty(
	llcb *
);

/*
 * ll_add_first, _last
 *
 * add an item to either end of the list.
 *
 *     in: the ll instance
 *
 *     in: ppayload
 *
 * return: nothing
 */

void
ll_add_first(
	llcb *,
	ppayload
);

void
ll_add_last(
	llcb *,
	ppayload
);

/*
 * ll_remove_first, _last
 *
 * remove and return a payload from either end of the linked list.
 *
 *     in: the ll instance
 *
 * return: ppayload or NULL if the list is empty
 */

ppayload
ll_remove_first(
	llcb *
);

ppayload
ll_remove_last(
	llcb *
);

/*
 * ll_peek_first, _last
 *
 * return but do not remove a payload of from either end of linked list,
 *
 *     in: the ll instance
 *
 * return: ppayload or NULL if the list is empty
 */

ppayload
ll_peek_first(
	llcb *
);

ppayload
ll_peek_last(
	llcb *
);

/*
 * ll_create
 *
 * create a new singly linked list.
 *
 *     in: nothing
 *
 * return: the new ll instance.
 */

llcb *
ll_create(
	void
);

/*
 * ll_destroy
 *
 * free the linked list if it is empty.
 *
 *     in: the ll instance
 *
 * return: boolean, true if successful
 */

bool
ll_destroy(
	llcb *
);

/*
 * ll_reset
 *
 * remove all items from the linked list.
 *
 *     in: the ll instance
 *
 * return: integer number of items removed
 */

int
ll_reset(
	llcb *
);

/*
 * ll_count
 *
 * how many items are in the linked list?
 *
 *     in: the ll instance
 *
 * return: integer number of items in the list
 */

int
ll_count(
	llcb *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* ll.h ends here */
