/* txbdl.h -- blametroi's doubly linked list functions -- */

/*
 * a header only implementation of a doubly linked list.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TXBDL_H
#define TXBDL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * hdl
 *
 * handle to a doubly linked list instance.
 */

typedef struct dlcb hdl;

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
 * dlid
 *
 * the dlid is a synchronization token between the client and the
 * library. functions that return a dlid also mark the current
 * position in the dlcb and when a function receives a dlid it checks
 * it against the position stored in the dlid. if they differ, it is
 * an error.
 *
 * for functions that return a dlid, if 'null_dlid' is true then there
 * was an error. if the function should return a payload, the payload
 * pointer is set to NULL.
 *
 * use the null_dlid macro for this check in case the dlid structure
 * ever changes.
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

hdl *
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
 * return: boolean, was the dl destroyed?
 */

bool
dl_destroy(
	hdl *dl
);

/*
 * dl_get_error
 *
 * get a brief description of any error produced by the last dl_xxx
 * function.
 *
 *     in: the dl instance
 *
 * return: constant string with a brief message or NULL
 */

const
char *
dl_get_error(
	hdl *dl
);

/*
 * dl_count
 *
 * how many items are on the list? the current list position is not
 * changed.
 *
 *     in: the dl instance
 *
 * return: int number of items on the list
 */

int
dl_count(
	hdl *dl
);

/*
 * dl_empty
 *
 * is the list empty? the current list position is not changed.
 *
 *     in: the dl instance
 *
 * return: boolean
 */

bool
dl_empty(
	hdl *dl
);

/*
 * dl_reset
 *
 * reset the list an empty state.
 *
 *     in: the dl instance
 *
 * return: int number of items deleted
 *
 * note that this does not issue frees on the payloads.
 */

int
dl_reset(
	hdl *dl
);

/*
 * dl_insert_first
 *
 * insert a new item at the head of the list. this new item becomes
 * the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: a payload
 *
 * return: dlid of the item or NULL_DLID
 */

dlid
dl_insert_first(
	hdl *dl,
	ppayload payload
);

/*
 * dl_insert_last
 *
 * insert a new item at the tail of the list. this new item becomes
 * the current position in the list.
 *
 *     in: the dl instance
 *
 *     in: a payload
 *
 * return: dlid of the item or NULL_DLID
 */

dlid
dl_insert_last(
	hdl *dl,
	ppayload payload
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
 *     in: a payload
 *
 * return: dlid of the item or NULL_DLID
 */

dlid
dl_insert_before(
	hdl *dl,
	dlid id,
	ppayload payload
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
 *     in: a payload
 *
 * return: dlid of the item or NULL_DLID
 */

dlid
dl_insert_after(
	hdl *dl,
	dlid id,
	ppayload payload
);

/*
 * dl_get_first
 *
 * get the first item in the list and set the position.
 *
 *     in: the dl instance
 *
 *     in: pointer to a payload
 *
 * return: dlid of the item or NULL_DLID
 *
 * if there was an item, the payload is delivered via the
 * pointer.
 */

dlid
dl_get_first(
	hdl *dl,
	ppayload *payload
);

/*
 * dl_get_last
 *
 * get the last item in the list and set the position.
 *
 *     in: the dl instance
 *
 *     in: pointer to a payload
 *
 * return: dlid of the item or NULL_DLID
 *
 * if there was an item, the payload is delivered via the
 * pointer.
 */

dlid
dl_get_last(
	hdl *dl,
	ppayload *payload
);

/*
 * dl_get_next
 *
 * get item after the current positioned item, advancing the position
 * to this item.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the positioned item
 *
 *     in: pointer to a payload
 *
 * return: the dlid or NULL_DLID if no item
 *
 * if there was an item, the payload is delivered via the
 * pointer.
 */

dlid
dl_get_next(
	hdl *dl,
	dlid id,
	ppayload *payload
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
 *
 * if there was an item, the payload is delivered via the
 * pointer.
 */

dlid
dl_get_previous(
	hdl *dl,
	dlid id,
	ppayload *payload
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
	hdl *dl,
	dlid id
);

/*
 * dl_update
 *
 * update an item's value in the list. the list should be positioned
 * on the node to update. and the position is not changed.
 *
 *     in: the dl instance
 *
 *     in: the dlid of the item to update
 *
 *     in: the new payload, typically a void * pointer to a value
 *
 * return: the dlid of the updated item
 *
 * if your payloads are pointers and the referenced client data does
 * not move, there is no need to use the dl_update function.
 */

bool
dl_update(
	hdl *dl,
	dlid id,
	ppayload payload
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBDL_H */

#ifdef TXBDL_IMPLEMENTATION
#undef TXBDL_IMPLEMENTATION

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "txbabort.h"

/*
 * dlcb.
 *
 * a transparent definition of an instance of the doubly linked list.
 * client code shouldn't be concerned with these variables.
 */

#define HDL_TAG      "__HDL___"
#define HDL_TAG_LEN  8

#define ASSERT_HDL(p, m) \
	abort_if(!(p) || memcmp((p), HDL_TAG, HDL_TAG_LEN) != 0, (m));

#define ASSERT_HDL_OR_NULL(p, m) \
	abort_if(p && memcmp((p), HDL_TAG, HDL_TAG_LEN) != 0, (m));

typedef struct dlnode dlnode;

struct dlcb {
	char tag[HDL_TAG_LEN];
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
next_id(hdl *dl) {
	dl->id += 1;
	return dl->id;
}

/*
 * dlcb.error will reference one of these in certain conditions,
 * otherwise NULL.
 */

static const char *dl_err_list_empty       = "list empty";
static const char *dl_err_next_at_tail     = "get next reached tail of list";
static const char *dl_err_previous_at_head =
	"get previous reached head of list";
static const char *dl_err_not_positioned   = "get next/prev not positioned";

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
 * the dlid is the position of the item in the list. functions that
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
	hdl *owner;
	dlnode *next;
	dlnode *previous;
	ppayload payload;
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

hdl *
dl_create(
	void
) {
	hdl *dl = malloc(sizeof(*dl));
	abort_if(!dl,
		"dl_create could not allocate HDL");
	memset(dl, 0, sizeof(*dl));
	memcpy(dl->tag, HDL_TAG, sizeof(dl->tag));
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
	hdl *dl
) {
	ASSERT_HDL(dl, "invalid HDL");
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

const
char *
dl_get_error(
	hdl *dl
) {
	ASSERT_HDL(dl, "invalid HDL");
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
	hdl *dl
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;
	int n = 0;
	dlnode *dn = dl->first;
	while (dn) {
		n += 1;
		dn = dn->next;
	}
	abort_if(n != dl->count,
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
	hdl *dl
) {
	ASSERT_HDL(dl, "invalid HDL");
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
	hdl *dl
) {
	ASSERT_HDL(dl, "invalid HDL");
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

	abort_if(dl->count != deleted,
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
	hdl *dl,
	ppayload payload
) {
	ASSERT_HDL(dl, "invalid HDL");

	dlnode *dn = malloc(sizeof(*dn));
	abort_if(!dn,
		"dl create_dlnode could not allocate DLNODE");

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
	hdl *dl,
	ppayload payload
) {
	ASSERT_HDL(dl, "invalid HDL");
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
	hdl *dl,
	ppayload payload
) {
	ASSERT_HDL(dl, "invalid HDL");
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
	hdl *dl,
	dlid id,
	ppayload payload
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	if (dl->position == NULL || dl->position->id != id) {
		dl->error = dl_err_not_positioned;
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
	hdl *dl,
	dlid id,
	ppayload payload
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	if (dl->position == NULL || dl->position->id != id) {
		dl->error = dl_err_not_positioned;
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
	hdl *dl,
	ppayload *payload
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	dl->position = dl->first;
	if (!dl->position) {
		dl->error = dl_err_list_empty;
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
	hdl *dl,
	ppayload *payload
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	dl->position = dl->last;
	if (!dl->position) {
		dl->error = dl_err_list_empty;
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
	hdl *dl,
	dlid id,
	ppayload *payload
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	if (dl->position == NULL || id != dl->position->id) {
		dl->error = dl_err_not_positioned;
		dl->position = NULL;
		*payload = NULL;
		return NULL_DLID;
	}

	dl->position = dl->position->next;
	if (!dl->position) {
		dl->error = dl_err_next_at_tail;
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
	hdl *dl,
	dlid id,
	ppayload *payload
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	if (dl->position == NULL || id != dl->position->id) {
		dl->error = dl_err_not_positioned;
		dl->position = NULL;
		*payload = NULL;
		return NULL_DLID;
	}

	dl->position = dl->position->previous;
	if (!dl->position) {
		dl->error = dl_err_previous_at_head;
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
	hdl *dl,
	dlid id
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	if (dl->position == NULL || id != dl->position->id) {
		dl->position = NULL;
		dl->error = dl_err_not_positioned;
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
	hdl *dl,
	dlid id,
	ppayload payload
) {
	ASSERT_HDL(dl, "invalid HDL");
	dl->error = NULL;

	dlnode *dn = dl->position;
	if (dn == NULL || id != dl->position->id) {
		dl->error = dl_err_not_positioned;
		dl->position = NULL;
		return NULL_DLID;
	}

	dl->position->payload = payload;
	return true;
}
#endif /* TXBDL_IMPLEMENTATION */
/* txbdl.h ends here */
