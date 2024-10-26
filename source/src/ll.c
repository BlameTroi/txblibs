/* ll.c -- blametroi's simple singly linked list -- */

/*
 * a header only implementation of a simple singly linked list.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/abort_if.h"

#include "../inc/ll.h"

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
	llcb *ll
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
	llcb *ll
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
	llcb *ll,
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
	llcb *ll,
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
	llcb *ll
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
	llcb *ll
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
	llcb *ll
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
	llcb *ll
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

llcb *
ll_create(
	void
) {
	llcb *ll = malloc(sizeof(*ll));
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
	llcb *ll
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
	llcb *ll
) {
	ASSERT_LLCB(ll, "invalid LLCB");

	if (ll->first != NULL)
		return false;
	memset(ll, 253, sizeof(*ll));
	free(ll);
	return true;
}

/* ll.c ends here */
