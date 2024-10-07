/*
 * single file header generated via:
 * buildhdr --macro TXBDL --intro LICENSE --pub ./source/inc/dl.h --priv ./source/src/dl.c 
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
 * an opaque definition of an instance of the doubly linked list.
 */

typedef struct dlcb dlcb;

/*
 * dlnode
 *
 * client code should consider everything but the payload pointer as
 * read only. the whole node is returned from many functions and is
 * expected to be passed on a subsequent function call to check the
 * position within the list.
 *
 * a payload is expected to be a pointer to some client managed data.
 * if the data is malloced, it is the clients responsibility to free
 * it. if the data to store will fit in a void *, the client may
 * store it directly.
 *
 * the dlid is the synchronization token with the list. functions
 * return a dlid also mark the current position in the dlcb and when a
 * function receives a dlid it checks it against the position stored
 * in the dlid. if they differ, it is an error.
 *
 * for functions that return a dlid, if null_dlid is true then there
 * was an error. if the function should return a payload, the payload
 * pointer is set to NULL.
 */

typedef unsigned long dlid;
#define NULL_DLID 0
#define null_dlid(a) ((a) == NULL_DLID)

/*
 * dl_create
 *
 * create an instance of a doubly linked list.
 *
 *     in: nothing
 *
 * return: the new dl instance
 */

dlcb *
dl_create(
	void
);

/*
 * dl_destroy
 *
 * destroy a dl instance if it is empty.
 *
 *     in: the dl instance
 *
 * return: true if successful, false if the dl was not empty
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
 *     in: the dl instance
 *
 * return: constant string with a brief message or NULL
 */

const char *
dl_get_error(
	dlcb *dl
);

/*
 * dl_count
 *
 * how many items are on the list? the current list position
 * is not changed.
 *
 *     in: the dl instance
 *
 * return: int number of items on the list
 */

int
dl_count(
	dlcb *dl
);

/*
 * dl_empty
 *
 * is the list empty? the current list position is not changed.
 *
 *     in: the dl instance
 *
 * return: bool
 */

bool
dl_empty(
	dlcb *dl
);

/*
 * dl_reset
 *
 * reset the list, deleting all items. does not free payload storage.
 *
 *     in: the dl instance
 *
 * return: int number of items deleted
 */

int
dl_reset(
	dlcb *dl
);

/*
 * dl_insert_first
 *
 * insert a new item at the head of the list. this new item becomes
 * the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the dlid
 */

dlid
dl_insert_first(
	dlcb *dl,
	void *payload
);

/*
 * dl_insert_last
 *
 * insert a new item at the tail of the list. this new item becomes
 * the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the dlid
 */

dlid
dl_insert_last(
	dlcb *dl,
	void *payload
);

/*
 * dl_insert_before
 *
 * insert a new item immediately before the currently positioned item.
 * this new item becomes the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the current position in the dl
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the new dlid
 */

dlid
dl_insert_before(
	dlcb *dl,
	dlid id,
	void *payload
);

/*
 * dl_insert_after
 *
 * insert a new item immediately after the currently positioned item.
 * this new item becomes the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the current position in the dl
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the new dlid
 */

dlid
dl_insert_after(
	dlcb *dl,
	dlid id,
	void *payload
);

/*
 * dl_get_first
 *
 * get the first item in the list and set the position.
 *
 *     in: the dl instance
 *
 *     in: address of *payload
 *
 * return: dlid of the first item
 */

dlid
dl_get_first(
	dlcb *dl,
	void **payload
);

/*
 * dl_get_last
 *
 * get the last item in the list and set the position.
 *
 *     in: the dl instance
 *
 *     in: address of *payload
 *
 * return: dlid of the last item
 */

dlid
dl_get_last(
	dlcb *dl,
	void **payload
);

/*
 * dl_get_next
 *
 * get item after the current positioned item, advancing
 * the position to this item.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the positioned item
 *
 *     in: address of *payload
 *
 * return: the dlid or NULL_DLID if no item
 */

dlid
dl_get_next(
	dlcb *dl,
	dlid id,
	void **payload
);

/*
 * dl_get_previous
 *
 * get the item before the current positioned item, advancing
 * the position to this item.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the positioned item
 *
 *     in: address of *payload
 *
 * return: the dlid or NULL_DLID if no item
 */

dlid
dl_get_previous(
	dlcb *dl,
	dlid id,
	void **payload
);

/*
 * dl_delete
 *
 * remove the currently positioned item from the list.  clears
 * the list position.
 *
 *     in: the dl instance
 *
 *     in: the dlid
 *
 * return: boolean true if deleted, false on error
 */

bool
dl_delete(
	dlcb *dl,
	dlid id
);

/*
 * dl_update
 *
 * update an item's value in the list. the list should be positioned
 * on the node to update. and the position is not changed.
 *
 * as items are stored in memory, if you do not change the address of
 * the value (ie, you updated its contents in place) there is no need
 * to use dl_update.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the item to update
 *
 *     in: the new payload, typically a void * pointer to a value
 *
 * return: the dlid of the updated item
 */

bool
dl_update(
	dlcb *dl,
	dlid id,
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


typedef struct dlnode dlnode;

/*
 * dlcb.
 *
 * a transparent definition of an instance of the doubly linked list.
 * client code shouldn't be concerned with these variables.
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
	dlid id;
	int count;
	const char *error;
};

/*
 * next_id
 *
 * increment and return id. ids start at 0, so the first one assigned
 * will be 1. there's no realistic way this will wrap. a dlid of 0
 * means "null".
 *
 *     in: the dl instance
 *
 * return: the id
 */

static
dlid
next_id(dlcb *dl) {
	dl->id += 1;
	return dl->id;
}

/*
 * dlcb.error will reference one of these in certain conditions,
 * otherwise NULL.
 */

static const char *error_list_empty       = "list empty";
static const char *error_next_at_tail     = "get next reached tail of list";
static const char *error_previous_at_head = "get previous reached head of list";
static const char *error_not_positioned   = "get next/prev not positioned";

/*
 * dlnode
 *
 * the dlnode packages the client's payload for the dl.
 *
 * a payload is expected to be a pointer to some client managed data.
 * if the data is malloced, it is the clients responsibility to free
 * it. if the data to store will fit in a void *, the client may
 * store it directly.
 *
 * the dlnode is the position of the item in the list. functions that
 * return a dlid also mark the current position in the dlcb and when a
 * function receives a dlid it checks it against the position stored
 * in the dlcb. if they differ, it is an error.
 */

#define ASSERT_DLNODE(p, d, m) assert((p) && memcmp((p), DLNODE_TAG, DLNODE_TAG_LEN) == 0 && \
	(p)->owner == d && m)

#define DLNODE_TAG_LEN 8
#define DLNODE_TAG "_DLNODE_"

struct dlnode {
	char tag[DLNODE_TAG_LEN];
	dlid id;
	dlcb *owner;
	dlnode *next;
	dlnode *previous;
	void *payload;
};

/*
 * dl_create
 *
 * create an instance of a doubly linked list.
 *
 *     in: nothing
 *
 * return: the new dl instance
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
	dl->id = 0;
	dl->position = NULL;
	dl->first = NULL;
	dl->last = NULL;
	return dl;
}

/*
 * dl_destroy
 *
 * destroy a dl instance if it is empty.
 *
 *     in: the dl instance
 *
 * return: true if successful, false if the dl was not empty
 */

bool
dl_destroy(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	if (!dl_empty(dl))
		return false;
	memset(dl, 253, sizeof(*dl));
	free(dl);
	return true;
}

/*
 * dl_get_error
 *
 * get status of last command if there was an error.
 *
 *     in: the dl instance
 *
 * return: constant string with a brief message or NULL
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
 * how many items are on the list? the current list position
 * is not changed.
 *
 *     in: the dl instance
 *
 * return: int number of items on the list
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
 * is the list empty? the current list position is not changed.
 *
 *     in: the dl instance
 *
 * return: bool
 */

bool
dl_empty(
	dlcb *dl
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;
	return !dl->first;
}

/*
 * dl_reset
 *
 * reset the list, deleting all items. does not free payload storage.
 *
 *     in: the dl instance
 *
 * return: int number of items deleted
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
	dl->error = NULL;
	dl->position = NULL;
	/*
	 * dl->id is intentionally *not* reset
	 */

	assert(dl->count == deleted &&
			"dl_reset mismatch between deleted and count");
	dl->count = 0;

	return deleted;
}

/*
 * dl_create_node.
 *
 * allocate and initialize a node that can be linked into a dl.
 *
 *     in: the dl instance
 *
 * return: the new node
 */

static
dlnode *
create_dlnode(
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
	dn->id = next_id(dl);

	return dn;
}

/*
 * dl_insert_first
 *
 * insert a new item at the head of the list. this new item becomes
 * the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the dlid of the inserted item
 */

dlid
dl_insert_first(
	dlcb *dl,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	dlnode *new_dn = create_dlnode(dl, payload);
	if (!dl->first) {
		dl->first = new_dn;
		dl->last = new_dn;
	} else {
		new_dn->next = dl->first;
		dl->first->previous = new_dn;
		dl->first = new_dn;
	}

	dl->count += 1;
	dl->position = dl->first;
	return dl->first->id;
}

/*
 * dl_insert_last
 *
 * insert a new item at the tail of the list. this new item becomes
 * the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the dlid of the inserted item
 */

dlid
dl_insert_last(
	dlcb *dl,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	dlnode *new_dn = create_dlnode(dl, payload);
	if (!dl->first) {
		dl->first = new_dn;
		dl->last = new_dn;
	} else {
		new_dn->previous = dl->last;
		dl->last->next = new_dn;
		dl->last = new_dn;
	}

	dl->count += 1;
	dl->position = dl->last;

	return dl->last->id;
}

/*
 * dl_insert_before
 *
 * insert a new item immediately before the currently positioned item.
 * this new item becomes the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the current position in the dl
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the dlid of the inserted item
 */

dlid
dl_insert_before(
	dlcb *dl,
	dlid id,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	if (dl->position == NULL || dl->position->id != id) {
		dl->error = error_not_positioned;
		dl->position = NULL;
		return NULL_DLID;
	}

	dlnode *dn = dl->position;
	dlnode *new_dn = create_dlnode(dl, payload);

	/* link new node in front of current */
	new_dn->previous = dn->previous;
	new_dn->next = dn;
	dn->previous = new_dn;

	/* link to old current previous */
	if (dl->first == dn)
		dl->first = new_dn;
	else
		new_dn->previous->next = new_dn;

	dl->count += 1;
	dl->position = new_dn;
	return dl->position->id;
}

/*
 * dl_insert_after
 *
 * insert a new item immediately after the currently positioned item.
 * this new item becomes the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the current position in the dl
 *
 *     in: the client data must fit in in a void *, typically
 *         a pointer to the client data
 *
 * return: the dlid of the inserted item
 */

dlid
dl_insert_after(
	dlcb *dl,
	dlid id,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	if (dl->position == NULL || dl->position->id != id) {
		dl->error = error_not_positioned;
		dl->position = NULL;
		return NULL_DLID;
	}

	dlnode *dn = dl->position;
	dlnode *new_dn = create_dlnode(dl, payload);

	/* link new node after current */
	new_dn->next = dn->next;
	new_dn->previous = dn;
	dn->next = new_dn;

	/* link to old current next */
	if (dl->last == dn)
		dl->last = new_dn;
	else
		new_dn->next->previous = new_dn;

	dl->count += 1;
	dl->position = new_dn;
	return dl->position->id;

}

/*
 * dl_get_first
 *
 * get the first item in the list and set the position.
 *
 *     in: the dl instance
 *
 * return: dlid of the first item or NULL_DLID
 */

dlid
dl_get_first(
	dlcb *dl,
	void **payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	dl->position = dl->first;
	if (!dl->position) {
		dl->error = error_list_empty;
		*payload = NULL;
		return NULL_DLID;
	}

	*payload = dl->position->payload;
	return dl->position->id;
}

/*
 * dl_get_last
 *
 * get the last item in the list and set the position.
 *
 *     in: the dl instance
 *
 * return: dlid of the last item or NULL_DLID
 */

dlid
dl_get_last(
	dlcb *dl,
	void **payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	dl->position = dl->last;
	if (!dl->position) {
		dl->error = error_list_empty;
		*payload = NULL;
		return NULL_DLID;
	}

	*payload = dl->position->payload;
	return dl->position->id;
}

/*
 * dl_get_next
 *
 * get item after the current positioned item, advancing
 * the position to this item.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the positioned item
 *
 * return: the dlid of the next item or NULL_DLID
 */

dlid
dl_get_next(
	dlcb *dl,
	dlid id,
	void **payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	if (dl->position == NULL || id != dl->position->id) {
		dl->error = error_not_positioned;
		dl->position = NULL;
		*payload = NULL;
		return NULL_DLID;
	}

	dl->position = dl->position->next;
	if (!dl->position) {
		dl->error = error_next_at_tail;
		*payload = NULL;
		return NULL_DLID;
	}

	*payload = dl->position->payload;
	return dl->position->id;
}

/*
 * dl_get_previous
 *
 * get the item before the current positioned item, advancing
 * the position to this item.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the positioned item
 *
 * return: the dlid of the previous item or NULL_DLID
 */

dlid
dl_get_previous(
	dlcb *dl,
	dlid id,
	void **payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	if (dl->position == NULL || id != dl->position->id) {
		dl->error = error_not_positioned;
		dl->position = NULL;
		*payload = NULL;
		return NULL_DLID;
	}

	dl->position = dl->position->previous;
	if (!dl->position) {
		dl->error = error_previous_at_head;
		*payload = NULL;
		return NULL_DLID;
	}

	*payload = dl->position->payload;
	return dl->position->id;
}

/*
 * dl_delete
 *
 * remove the currently positioned item from the list.  clears
 * the list position.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the positioned item
 *
 * return: boolean true if deleted, false on error
 */

bool
dl_delete(
	dlcb *dl,
	dlid id
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	if (dl->position == NULL || id != dl->position->id) {
		dl->position = NULL;
		dl->error = error_not_positioned;
		return false;
	}

	/* deletes clear position */
	dlnode *dn = dl->position;
	dl->position = NULL;

	/* properly unlink the node while preserving the chain */
	if (dn->next == NULL && dn->previous == NULL) {   /* only node */
		dl->first = NULL;
		dl->last = NULL;
	} else if (dn->previous == NULL) {                /* first node */
		dl->first = dn->next;
		((dlnode *)dn->next)->previous = NULL;
	} else if (dn->next == NULL) {                    /* last node */
		dl->last = dn->previous;
		((dlnode *)dn->previous)->next = NULL;
	} else {                                          /* somewhere in the middle */
		((dlnode *)dn->previous)->next = dn->next;
		((dlnode *)dn->next)->previous = dn->previous;
	}

	memset(dn, 253, sizeof(*dn));
	free(dn);
	dl->count -= 1;
	return true;
}

/*
 * dl_update
 *
 * update an item's value in the list. the list should be positioned
 * on the node to update. and the position is not changed.
 *
 * as items are stored in memory, if you do not change the address of
 * the value (ie, you updated its contents in place) there is no need
 * to use dl_update.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the item to be updated
 *
 *     in: the new payload, typically a void * pointer to a value
 *
 * return: bool true on successful update
 */

bool
dl_update(
	dlcb *dl,
	dlid id,
	void *payload
) {
	ASSERT_DLCB(dl, "invalid DLCB");
	dl->error = NULL;

	dlnode *dn = dl->position;
	if (dn == NULL || id != dl->position->id) {
		dl->error = error_not_positioned;
		dl->position = NULL;
		return NULL_DLID;
	}

	dl->position->payload = payload;
	return true;
}

/* dl.c ends here */
/* *** end priv *** */

#endif /* TXBDL_IMPLEMENTATION */
