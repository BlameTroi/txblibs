/*
 * single file header generated via:
 * buildhdr --macro TXBDL --intro LICENSE --pub inc/dl.h --priv src/dl.c 
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Troy Brumley 
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   *** end intro ***
 */

#ifndef TXBDL_SINGLE_HEADER
#define TXBDL_SINGLE_HEADER
/* *** begin pub *** */
/* dl.h -- blametroi's doubly linked list functions -- */

/*
 * a header only implementation of a doubly linked list.
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
 * dlcb
 *
 * an opaque definition of an instance of the linked list.
 */

typedef struct dlcb dlcb;

/*
 * dlnode
 *
 * client code should consider everything but the payload pointer as
 * read only. the whole node is returned from many functions and is
 * expected to be passed on a subsequent function call that assumes
 * the position within the list.
 *
 * payload will typically be a pointer to some client managed data.
 * if the data is malloced, it is the clients responsibility to free
 * it. if the data to store will fit in a void *, the client may
 * store it directly.
 *
 * the dlnode is the position of the item in the list. functions that
 * return a dlnode also mark the current position in the dlcb and
 * when a function receives a dlnode it checks it against the
 * position stored in the dlcb. if they differ, it is an error.
 */

typedef struct dlnode dlnode;

#define DLNODE_TAG_LEN 8
#define DLNODE_TAG "_DLNODE_"

struct dlnode {
	char tag[DLNODE_TAG_LEN];  /* read only */
	dlcb *owner;               /* read only */
	dlnode *next;              /* read only */
	dlnode *previous;          /* read only */
	void *payload;             /* pointer to item to store or the item if it will fit in a void */
};

/*
 * dl_create
 *
 * create an instance of a keyed linked list.
 *
 * return: the new dl instance.
 */

dlcb *
dl_create(
	void
);

/*
 * dl_destroy
 *
 * destroy an instance of a keyed linked list if it is empty.
 *
 *     in: the dl instance.
 *
 * return: true if successful, false if the dl was not empty.
 */

bool
dl_destroy(
	dlcb *dl
);

/*
 * dl_get_error
 *
 * get status of last command if there was an error.
 *
 *     in: the dl instance.
 *
 * return: constant string with a brief message or NULL.
 */

const char *
dl_get_error(
	dlcb *dl
);

/*
 * dl_count
 *
 * how many items are on the list? does not change the current
 * positioned item in the list.
 *
 *     in: the dl instance.
 *
 * return: int number of items on the list.
 */

int
dl_count(
	dlcb *dl
);

/*
 * dl_empty
 *
 * is the list empty? does not change the current positioned item in
 * the list.
 *
 *     in: the dl instance.
 *
 * return: bool.
 */

bool
dl_empty(
	dlcb *dl
);

/*
 * dl_reset
 *
 * reset the keyed link list, deleting all items. does not free payload
 * storage.
 *
 *     in: the dl instance.
 *
 * return: int number of items deleted.
 */

int
dl_reset(
	dlcb *dl
);

/*
 * dl_insert_first
 *
 * insert a new item at the head of the list. the dl is positioned
 * at this new item.
 *
 *     in: the dl instance.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_first(
	dlcb *dl,
	void *payload
);

/*
 * dl_insert_last
 *
 * insert a new item at the tail of the list. the dl is positioned
 * at this new item.
 *
 *     in: the dl instance.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_last(
	dlcb *dl,
	void *payload
);

/*
 * dl_insert_before
 *
 * insert a new item immediately before the current item. the
 * dl is positioned at this new item.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the current position in the dl.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_before(
	dlcb *dl,
	dlnode *node,
	void *payload
);

/*
 * dl_insert_after
 *
 * insert a new item immediately after the current item. the
 * dl is positioned at this new item.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the current position in the dl.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_after(
	dlcb *dl,
	dlnode *node,
	void *payload
);

/*
 * dl_get_first
 *
 * get the first item in the list and set the position.
 *
 *     in: the dl instance.
 *
 * return: dl node of that item.
 */

dlnode *
dl_get_first(
	dlcb *dl
);

/*
 * dl_get_last
 *
 * get the last item in the list and set the position.
 *
 *     in: the dl instance.
 *
 * return: the dl node of that item.
 */

dlnode *
dl_get_last(
	dlcb *dl
);

/*
 * dl_get_next
 *
 * get item after the current positioned item, updating
 * the position in the list.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the positioned item.
 *
 * return: the dl node of the next item or NULL.
 */

dlnode *
dl_get_next(
	dlcb *dl,
	dlnode *dn
);

/*
 * dl_get_previous
 *
 * get the item before the current positioned item, updating
 * the position in the list.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the positioned item.
 *
 * return: the dl node of the previous item or NULL.
 */

dlnode *
dl_get_previous(
	dlcb *dl,
	dlnode *dn
);

/*
 * dl_delete
 *
 * remove the currently positioned item from the list. clears
 * the list position.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the positioned item.
 *
 * return: boolean true if deleted, false on error.
 */

bool
dl_delete(
	dlcb *dl,
	dlnode *dn
);

/*
 * dl_update
 *
 * update an item's value in the list. the list should be positioned
 * on the node to update. the dl position is unchanged.
 *
 * as items are stored in memory, if you do not change the address of
 * the value (ie, you updated its contents in place) there is no need
 * to use dl_update.
 *
 *     in: the dl instance.
 *
 *     in: the dl node to be updated
 *
 *     in: the new payload, typically a void * pointer to a value.
 *
 * return: the dl node of the updated item.
 */

dlnode *
dl_update(
	dlcb *dl,
	dlnode *dn,
	void *payload
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* *** end pub *** */

#endif /* TXBDL_SINGLE_HEADER */

#ifdef TXBDL_IMPLEMENTATION
#undef TXBDL_IMPLEMENTATION
/* *** begin priv *** */
/* dl.c -- blametroi's doubly linked list functions -- */

/*
 * a header only implementation of a doubly linked list.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#undef NDEBUG
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


/*
 * dlcb.
 *
 * a transparent definition of an instance of the linked list. client
 * code isn't expected to see these fields.
 */

#define DLCB_TAG      "__DLCB__"
#define DLCB_TAG_LEN  8

#define ASSERT_DLCB(p, m) assert((p) && memcmp((p), DLCB_TAG, DLCB_TAG_LEN) == 0 && (m))
#define ASSERT_DLCB_OR_NULL(p) assert((p) == NULL || memcmp((p), DLCB_TAG, DLCB_TAG_LEN) == 0)

struct dlcb {
	char tag[8];
	dlnode *first;
	dlnode *last;
	dlnode *position;
	int count;
	const char *error;
};

/*
 * dlcb.error will reference one of these in certain conditions,
 * otherwise NULL.
 */

static const char *error_list_empty       = "list empty";
static const char *error_next_at_tail     = "get next reached tail of list";
static const char *error_previous_at_head = "get previous reached head of list";
static const char *error_not_positioned   = "get next/prev not positioned";
static const char *error_bad_dlnode       = "malformed dlnode";

/*
 * dlnode.
 *
 * client code should consider everything but the payload pointer as
 * read only. the whole node is returned from many functions and is
 * expected to be passed on a subsequent function call that assumes
 * the position within the list.
 *
 * payload will typically be a pointer to some client managed data.
 * if the data is malloced, it is the clients responsibility to free
 * it.
 */

#define ASSERT_DLNODE(p, d, m) assert((p) && memcmp((p), DLNODE_TAG, DLNODE_TAG_LEN) == 0 && \
        (p)->owner == d && m)

/*
 * dl_create
 *
 * create an instance of a keyed linked list.
 *
 *     in: function pointer to a comparator for keys with an
 *         interface similar to the memcmp function.
 *
 * return: the new dl instance.
 */

dlcb *
dl_create(
	void
) {
	dlcb *dl = malloc(sizeof(*dl));
	assert(dl &&
		"could not allocate DLCB");
	memset(dl, 0, sizeof(*dl));
	memcpy(dl->tag, DLCB_TAG, sizeof(dl->tag));
	dl->count = 0;
	dl->position = NULL;
	dl->first = NULL;
	dl->last = NULL;
	return dl;
}

/*
 * dl_destroy
 *
 * destroy an instance of a keyed linked list if it is empty.
 *
 *     in: the dl instance.
 *
 * return: true if successful, false if dl was not empty.
 */

bool
dl_destroy(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (dl_empty(dl)) {
		memset(dl, 253, sizeof(*dl));
		free(dl);
		return true;
	}
	return false;
}

/*
 * dl_get_error
 *
 * get status of last command if there was an error.
 *
 *     in: the dl instance.
 *
 * return: constant string with a brief message or NULL.
 *
 */

const char *
dl_get_error(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	return dl->error;
}

/*
 * dl_count
 *
 * how many items are on the list?
 *
 *     in: the dl instance.
 *
 * return: int number of items on the list.
 */

int
dl_count(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;
	int n = 0;
	dlnode *dn = dl->first;
	while (dn) {
		n += 1;
		dn = dn->next;
	}
	assert(n == dl->count &&
		"dl_count error in node count");
	return dl->count;
}

/*
 * dl_empty
 *
 * is the list empty?
 *
 *     in: the dl instance.
 *
 * return: bool.
 */

bool
dl_empty(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;
	return dl->first == NULL;
}

/*
 * dl_reset
 *
 * reset the keyed link list, deleting all items. does not free payload
 * storage.
 *
 *     in: the dl instance.
 *
 * return: int number of items deleted.
 */

int
dl_reset(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;
	dlnode *dn = dl->first;
	dlnode *next = NULL;
	int deleted = 0;
	while (dn) {
		next = dn->next;
		memset(dn, 253, sizeof(*dn));
		free(dn);
		deleted += 1;
		dn = next;
	}
	dl->first = NULL;
	dl->last = NULL;
	dl->position = NULL;
	dl->error = NULL;
	assert(dl->count == deleted &&
		"dl_reset mismatch between deleted and count");
	dl->count = 0;
	return deleted;
}

/*
 * dl_create_node.
 *
 * allocate and initialize a node that can be linked
 * into a dl.
 *
 *     in: the dl instance.
 *
 * return: the new node.
 */

static
dlnode *
dl_create_node(
	dlcb *dl,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dlnode *dn = malloc(sizeof(*dn));
	assert(dn &&
		"could not allocate DLNODE");
	memset(dn, 0, sizeof(*dn));
	memcpy(dn->tag, DLNODE_TAG, sizeof(dl->tag));
	dn->owner = dl;
	dn->payload = payload;
	return dn;
}

/*
 * dl_insert_first.
 *
 * insert a new item at the head of the list.
 *
 *     in: the dl instance.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_first(
	dlcb *dl,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;
	dlnode *new_dn = dl_create_node(dl, payload);
	if (!dl->first) {
		dl->first = new_dn;
		dl->last = new_dn;
	} else {
		new_dn->next = dl->first;
		dl->first->previous = new_dn;
		dl->position = new_dn;
		dl->first = new_dn;
	}
	dl->position = dl->first;
	dl->count += 1;
	return dl->position;
}

/*
 * dl_insert_last.
 *
 * insert a new item at the tail of the list.
 *
 *     in: the dl instance.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_last(
	dlcb *dl,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;
	dlnode *new_dn = dl_create_node(dl, payload);
	if (!dl->first) {
		dl->first = new_dn;
		dl->last = new_dn;
	} else {
		new_dn->previous = dl->last;
		dl->last->next = new_dn;
		dl->position = new_dn;
		dl->last = new_dn;
	}
	dl->position = dl->last;
	dl->count += 1;
	return dl->position;
}

/*
 * dl_insert_before.
 *
 * insert a new item immediately before the current item.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the current position in the dl.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_before(
	dlcb *dl,
	dlnode *dn,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	ASSERT_DLNODE(dn, dl, "invalid DLNODE");

	dl->error = NULL;

	if (dl->position != dn) {
		dl->error = error_bad_dlnode;
		dl->position = NULL;
		return NULL;
	}

	dlnode *new_dn = dl_create_node(dl, payload);

	new_dn->previous = dn->previous;
	new_dn->next = dn;

	dn->previous = new_dn;
	if (dl->first == dn)
		dl->first = new_dn;
	else
		new_dn->previous->next = new_dn;

	dl->count += 1;
	dl->position = new_dn;
	return new_dn;
}

/*
 * dl_insert_after.
 *
 * insert a new item immediately after the current item.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the current position in the dl.
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data.
 *
 * return: the dl node.
 */

dlnode *
dl_insert_after(
	dlcb *dl,
	dlnode *dn,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	ASSERT_DLNODE(dn, dl, "invalid DLNODE");

	dl->error = NULL;

	if (dl->position != dn) {
		dl->error = error_bad_dlnode;
		dl->position = NULL;
		return NULL;
	}

	dlnode *new_dn = dl_create_node(dl, payload);

	new_dn->next = dn->next;
	new_dn->previous = dn;

	dn->next = new_dn;
	if (dl->last == dn) {
		dl->last = new_dn;
	} else {
		new_dn->next->previous = new_dn;
	}

	dl->count += 1;
	dl->position = new_dn;
	return new_dn;

}

/*
 * dl_get_first
 *
 * get the first item in the list.
 *
 *     in: the dl instance.
 *
 * return: dl node of that item.
 */

dlnode *
dl_get_first(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->position = dl->first;
	dl->error = NULL;
	if (!dl->position)
		dl->error = error_list_empty;
	return dl->position;
}

/*
 * dl_get_last
 *
 * get the last item in the list.
 *
 *     in: the dl instance.
 *
 * return: the dl node of that item.
 */

dlnode *
dl_get_last(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->position = dl->last;
	dl->error = NULL;
	if (!dl->position)
		dl->error = error_list_empty;
	return dl->position;
}

/*
 * dl_get_next
 *
 * get item after the current positioned item.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the positioned item.
 *
 * return: the dl node of the next item or NULL.
 */

dlnode *
dl_get_next(
	dlcb *dl,
	dlnode *dn
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	ASSERT_DLNODE(dn, dl, "invalid DLNODE");
	dl->error = NULL;
	if (dn == NULL || dl->position == NULL || dn != dl->position) {
		dl->error = error_not_positioned;
		dl->position = NULL;
		return NULL;
	}
	dl->position = dl->position->next;
	if (!dl->position)
		dl->error = error_next_at_tail;
	return dl->position;
}

/*
 * dl_get_previous
 *
 * get the item before the current positioned item.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the positioned item.
 *
 * return: the dl node of the previous item or NULL.
 */

dlnode *
dl_get_previous(
	dlcb *dl,
	dlnode *dn
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	ASSERT_DLNODE(dn, dl, "invalid DLNODE");
	dl->error = NULL;
	if (dn == NULL || dl->position == NULL || dn != dl->position) {
		dl->error = error_not_positioned;
		dl->position = NULL;
		return NULL;
	}
	dl->position = dl->position->previous;
	if (!dl->position)
		dl->error = error_previous_at_head;
	return dl->position;
}

/*
 * dl_delete.
 *
 * remove the currently positioned item from the list.
 *
 *     in: the dl instance.
 *
 *     in: the dl node of the positioned item.
 *
 * return: boolean true if deleted, false on error.
 */

bool
dl_delete(
	dlcb *dl,
	dlnode *dn
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	ASSERT_DLNODE(dn, dl, "invalid DLNODE");
	dl->error = NULL;
	if (dl->position == NULL || dn != dl->position) {
		dl->position = NULL;
		dl->error = error_bad_dlnode;
		return false;
	}
	/* deletes clear position */
	dl->position = NULL;
	if (dn->next == NULL && dn->previous == NULL) {
		/* this is the only item */
		dl->first = NULL;
		dl->last = NULL;
	} else if (dn->previous == NULL) {
		/* this is the head */
		dl->first = dn->next;
		((dlnode *)dn->next)->previous = NULL;
	} else if (dn->next == NULL) {
		/* is this the tail? */
		dl->last = dn->previous;
		((dlnode *)dn->previous)->next = NULL;
	} else {
		/* somewhere in the middle */
		((dlnode *)dn->previous)->next = dn->next;
		((dlnode *)dn->next)->previous = dn->previous;
	}
	memset(dn, 253, sizeof(*dn));
	free(dn);
	dl->count -= 1;
	return true;
}

/*
 * dl_update.
 *
 * update an item's value in the list. the list should be positioned
 * on the node to update.
 *
 * as items are stored in memory, if you do not change the address of
 * the value (ie, you updated its contents in place) there is no need
 * to use dl_update.
 *
 *     in: the dl instance.
 *
 *     in: the dl node to be updated
 *
 *     in: the new payload, typically a void * pointer to a value.
 *
 * return: the dl node of the updated item.
 */

dlnode *
dl_update(
	dlcb *dl,
	dlnode *dn,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	ASSERT_DLNODE(dn, dl, "invalid DLNODE");
	dl->error = NULL;
	if (dn == NULL || dl->position == NULL || dn != dl->position) {
		dl->error = error_not_positioned;
		dl->position = NULL;
	} else {
		dl->position->payload = payload;
	}
	return dl->position;
}

/* dl.c ends here */
/* *** end priv *** */

#endif /* TXBDL_IMPLEMENTATION */
