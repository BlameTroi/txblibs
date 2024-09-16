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

#include <pthread.h>
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
 * takes a function pointer to a comparator for keys with an
 * interface similar to the memcmp function.
 *
 * returns a klcb, the keyed linked list instance.
 */

klcb *
kl_create(
	int (*fn_compare_keys)(void *, void *)
);

/*
 * kl_destroy
 *
 * destroy an instance of a keyed linked list if it is empty.
 *
 * takes the keyed list instance as input.
 *
 * returns true if destruction successful, false otherwise.
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
 * returns a char * or NULL.
 *
 */

const char *
kl_get_error(
	klcb *kl
);

/*
 * kl_count
 *
 * how many entries are on the list?
 *
 * takes the keyed list instance as input.
 *
 * returns an int.
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
 * takes the keyed list instance as input.
 *
 * returns a bool.
 */

bool
kl_empty(
	klcb *kl
);

/*
 * kl_reset
 *
 * reset the keyed link list, deleting all entries.
 *
 * takes the keyed list instance as input.
 *
 * returns an int, the number of entries that were on the list.
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
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the key, as a void *.
 *
 * the address of the value, as a void *.
 *
 * returns a bool, true if the insert succeeds, false if it failed.
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
 * get an entry with a particular key on the list.
 *
 * if the key is found in the list, return the associated value and
 * mark the list as positioned at that key. if not, clear list
 * positioning and return NULL.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
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
 * get the first entry on the list.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
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
 * get the last entry on the list.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
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
 * get the entry following the last entry read by one of the kl_get functions.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
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
 * get the entry before the last entry read by one of the kl_get functions.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the address of the key, as a void *.
 *
 * the address of the address of the value, as a void *.
 *
 * returns a bool.
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
 * update an entry with a particular key and value on the list. the
 * entry key must match the key of the last entry retrieved via one of
 * the kl_get functions. the key may not be changed, but the value is
 * updated.
 *
 * note: as items are stored in memory, if you do not change the
 *       address of the value (ie, you updated its contents in place)
 *       there is no need to use kl_update.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the key, as a void *.
 *
 * the address of the value, as a void *.
 *
 * returns a bool, true if the update succeeds, false if it failed.
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
 * delete an entry with a particular key on the list. the entry key
 * must match the key of the last entry retrieved via one of the
 * kl_get functions.
 *
 * takes as input:
 *
 * the instance of this list.
 *
 * the address of the key, as a void *.
 *
 * returns a bool, true if the delete succeeds, false if it failed.
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
