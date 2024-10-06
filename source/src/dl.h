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
