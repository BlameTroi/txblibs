/* kl.h -- blametroi's utility functions -- */

/*
 * a header only implementation of a keyed doubly linked list.
 *
 * the list is kept in order by a unique key using a client supplied
 * comparison function that returns an integer as memcmp would.
 *
 * keys and the values to add to the list are passed by reference
 * as void *. if the value would fit in sizeof(void *) it may be
 * passed directly.
 *
 * storage management for keys and values is the responsibility of
 * the client.
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

typedef struct klcb klcb;

/*
 * kl_create
 *
 * create an instance of a keyed linked list.
 *
 *     in: function pointer to a comparator for keys with an
 *         interface similar to the memcmp function.
 *
 * return: the new kl instance.
 */

klcb *
kl_create(
	int (*fn_compare_keys)(void *, void *)
);

/*
 * kl_clone
 *
 * create a copy of a kl instance.
 *
 *     in: the kl instance to copy.
 *
 * return: the copy lk instance.
 */

klcb *
kl_clone(
	klcb *kl
);

/*
 * kl_destroy
 *
 * destroy an instance of a keyed linked list if it is empty.
 *
 *     in: the kl instance.
 *
 * return: true if successful, false if kl was not empty.
 */

bool
kl_destroy(
	klcb *kl
);

/*
 * kl_get_error
 *
 * get status of last command if there was an error.
 *
 *     in: the kl instance.
 *
 * return: constant string with a brief message or NULL.
 *
 */

const char *
kl_get_error(
	klcb *kl
);

/*
 * kl_count
 *
 * how many items are on the list?
 *
 *     in: the kl instance.
 *
 * return: int number of items on the list.
 */

int
kl_count(
	klcb *kl
);

/*
 * kl_empty
 *
 * is the list empty?
 *
 *     in: the kl instance.
 *
 * return: bool.
 */

bool
kl_empty(
	klcb *kl
);

/*
 * kl_reset
 *
 * reset the keyed link list, deleting all items.
 *
 *     in: the kl instance.
 *
 * return: int number of items deleted.
 */

int
kl_reset(
	klcb *kl
);

/*
 * kl_insert
 *
 * insert an item with a particular key and value into the list.
 *
 *     in: the kl instance.
 *
 *     in: pointer to the key as a void *.
 *
 *     in: pointer to the value as a void *.
 *
 * return: bool was the insert successful?
 */

bool
kl_insert(
	klcb *kl,
	void *key,
	void *value
);

/*
 * kl_get
 *
 * get an item with a particular key from the list.
 *
 * if the key is found in the list, return the associated value and
 * mark the list as positioned at that key. if not, clear list
 * positioning and return NULL.
 *
 *     in: the kl instance.
 *
 *     in: pointer to the address of the key.
 *
 * in/out: pointer to the address to store the value as a void *.
 *
 * return: bool was the key found.
 */

bool
kl_get(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_first
 *
 * get the first item on the list.
 *
 *     in: the kl instance.
 *
 * in/out: pointer to the address to store the key.
 *
 * in/out: pointer to the address to store the value.
 *
 * return: bool was there a first item.
 */

bool
kl_get_first(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_last
 *
 * get the last item on the list.
 *
 *     in: the kl instance.
 *
 * in/out: pointer to the address to store the key.
 *
 * in/out: pointer to the address to store the value.
 *
 * return: bool was there a last item?
 */

bool
kl_get_last(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_next
 *
 * get the item following the last item read by one of the kl_get functions.
 *
 *     in: the kl instance.
 *
 * in/out: pointer to the address to store the key.
 *
 * in/out: pointer to the address to store the value.
 *
 * return: bool was there a next item?
 */

bool
kl_get_next(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_get_previous
 *
 * get the item before the last item read by one of the kl_get functions.
 *
 *     in: the kl instance.
 *
 * in/out: pointer to the address to store the key.
 *
 * in/out: pointer to the address to store the value.
 *
 * return: bool was there a previous item?
 */

bool
kl_get_previous(
	klcb *kl,
	void **key,
	void **value
);

/*
 * kl_update
 *
 * update an item with a particular key and value on the list. the
 * item key must match the key of the last item retrieved via one of
 * the kl_get functions. the key may not be changed, but the value can
 * be.
 *
 * as items are stored in memory, if you do not change the address of
 * the value (ie, you updated its contents in place) there is no need
 * to use kl_update.
 *
 *     in: the kl instance.
 *
 *     in: pointer to the key.
 *
 *     in: pointer to the value.
 *
 * return: did the update succeed.
 */

bool
kl_update(
	klcb *kl,
	void *key,
	void *value
);

/*
 * kl_delete
 *
 * delete an item with a particular key on the list. the item key
 * must match the key of the last item retrieved via one of the
 * kl_get functions.
 *
 *     in: the kl instance.
 *
 * in/out: pointer to the key.
 *
 * in/out: pointer to the value.
 *
 * return: did the delete succeed.
 */

bool
kl_delete(
	klcb *kl,
	void *key
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* kl.h ends here */
