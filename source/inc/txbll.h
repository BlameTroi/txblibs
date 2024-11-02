/* txbll.h -- blametroi's singly linked list library -- */

/*
 * a header only implementation of a singly linked list.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TXBLL_H
#define TXBLL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * a handle to a singly linked list instance.
 */

typedef struct llcb hll;

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
	hll *
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
	hll *,
	ppayload
);

void
ll_add_last(
	hll *,
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
	hll *
);

ppayload
ll_remove_last(
	hll *
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
	hll *
);

ppayload
ll_peek_last(
	hll *
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

hll *
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
	hll *
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
	hll *
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
	hll *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBLL_H */
#ifdef TXBLL_IMPLEMENTATION
#undef TXBLL_IMPLEMENTATION
/*
 * a header only implementation of a simple singly linked list.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "txbabort.h"

/*
 * transparent control block definitions.
 */

/* list item including client payload */
typedef struct llitem llitem;
struct llitem {
	llitem *next;
	ppayload payload;
};

#define LLCB_TAG "__LLCB__"
#define LLCB_TAG_LEN 8

#define ASSERT_LLCB(p, m) \
	abort_if(!(p) || memcmp((p), LLCB_TAG, LLCB_TAG_LEN) != 0, (m));

#define ASSERT_LLCB_OR_NULL(p, m) \
	abort_if(p && memcmp((p), LLCB_TAG, LLCB_TAG_LEN) != 0, (m));

struct llcb {
	char tag[LLCB_TAG_LEN];
	llitem *first;
	llitem *last;
};

/*
 * ll_empty
 *
 * are there items in the linked list?
 *
 *     in: the ll instance
 *
 * return: boolean, true if empty
 */

bool
ll_empty(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");
	return ll->first == NULL;
}

/*
 * ll_count
 *
 * how many items are in the linked list?
 *
 *     in: the ll instance
 *
 * return: integer number of items
 */

int
ll_count(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	if (ll->first == NULL)
		return 0;
	if (ll->first == ll->last)
		return 1;

	int i = 0;
	llitem *curr = ll->first;
	while (curr) {
		i += 1;
		curr = curr->next;
	}
	return i;
}

/*
 * ll_new_item
 *
 * package the client payload for entry in linked list.
 *
 *     in: void * client payload
 *
 * return: the new llitem
 */

static
llitem *
ll_new_item(
	ppayload payload
) {
	llitem *item = malloc(sizeof(*item));
	memset(item, 0, sizeof(*item));
	item->payload = payload;
	item->next = NULL;
	return item;
}

/*
 * ll_add_first
 *
 * add an item to the front of the linked list.
 *
 *     in: the ll instance
 *
 *     in: ppayload client payload
 *
 * return: nothing
 */

void
ll_add_first(
	hll *ll,
	ppayload payload
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	llitem *item = ll_new_item(payload);

	if (ll->first == NULL) {
		ll->first = item;
		ll->last = item;
		return;
	}

	item->next = ll->first;
	ll->first = item;
	return;
}

/*
 * ll_add_last
 *
 * add an item to the back of the linked list.
 *
 *     in: the ll instance
 *
 *     in: ppayload client payload
 *
 * return: nothing
 */

void
ll_add_last(
	hll *ll,
	ppayload payload
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	llitem *item = ll_new_item(payload);

	if (ll->first == NULL) {
		ll->first = item;
		ll->last = item;
		return;
	}

	ll->last->next = item;
	ll->last = item;
	return;
}

/*
 * ll_remove_first
 *
 * remove and return the first item on the linked list.
 *
 *     in: the ll instance
 *
 * return: ppayload client payload or NULL if linked list is empty
 */

ppayload
ll_remove_first(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	/* empty? */
	if (ll->first == NULL)
		return NULL;

	/* minimal link munging */
	llitem *li = ll->first;
	ll->first = li->next;

	/* return payload and clean up */
	ppayload res = li->payload;
	memset(li, 253, sizeof(*li));
	free(li);
	return res;
}

/*
 * ll_remove_last
 *
 * remove and return the last item on the linked list.
 *
 *     in: the ll instance
 *
 * return: ppayload client payload or NULL if linked list is empty
 */

ppayload
ll_remove_last(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	/* empty? */
	if (ll->last == NULL)
		return NULL;

	/* only one item is a special case when looking backward */
	if (ll->first == ll->last) {
		ppayload res = ll->first->payload;
		memset(ll->first, 253, sizeof(*(ll->first)));
		free(ll->first);
		ll->first = NULL;
		ll->last = NULL;
		return res;
	}

	/* unlinking from the last touches the penultimate. since this
	 * is a forward link only list, we have to chase to find it. */
	llitem *prior = NULL;
	llitem *curr = ll->first;
	while (curr != ll->last) {
		prior = curr;
		curr = curr->next;
	}
	prior->next = NULL;
	ll->last = prior;

	/* return the payload and delete the dangling item block */
	ppayload res = curr->payload;
	memset(curr, 253, sizeof(*curr));
	free(curr);
	return res;
}

/*
 * ll_peek_first
 *
 * return but do not remove the first item on the linked list.
 *
 *     in: the ll instance
 *
 * return: void * client payload or NULL if linked list is empty.
 */

void *
ll_peek_first(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	return ll->first == NULL ? NULL : ll->first->payload;
}

/*
 * ll_peek_last
 *
 * return but do not remove the last item on the linked list.
 *
 *     in: the ll instance
 *
 * return: void * client payload or NULL if linked list is empty.
 */

void *
ll_peek_last(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	return ll->last == NULL ? NULL : ll->last->payload;
}

/*
 * ll_create
 *
 * create a new linked list.
 *
 *     in: nothing
 *
 * return: the new ll instance.
 */

hll *
ll_create(
	void
) {
	hll *ll = malloc(sizeof(*ll));
	memset(ll, 0, sizeof(*ll));
	memcpy(ll->tag, LLCB_TAG, sizeof(ll->tag));
	ll->first = NULL;
	ll->last = NULL;
	return ll;
}

/*
 * ll_reset
 *
 * remove all items from the linked list.
 *
 *     in: the ll instance
 *
 * return: int number of items removed
 */

int
ll_reset(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");
	if (ll->first == NULL)
		return 0;
	int i = 0;
	while (ll->first) {
		llitem *item = ll->first;
		ll->first = item->next;
		memset(item, 253, sizeof(*item));
		free(item);
		i += 1;
	}
	return i;
}

/*
 * ll_destroy
 *
 * free the linked list control block if the linked list is empty.
 *
 *     in: the ll instance
 *
 * return: boolean, true if successful, false if linked list is not empty
 */

bool
ll_destroy(
	hll *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	if (ll->first != NULL)
		return false;
	memset(ll, 253, sizeof(*ll));
	free(ll);
	return true;
}
#endif /* TXBLL_IMPLEMENTATION */
/* txbll.h ends here */
