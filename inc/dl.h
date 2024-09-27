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
