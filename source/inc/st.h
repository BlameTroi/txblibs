/* st.h -- blametroi's simple stack -- */

/*
 * a header only implementation of a stack.
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
 * an instance of a stack.
 */

typedef struct stcb stcb;

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
 * st_empty
 *
 * are there any items on the stack?
 *
 *     in: the st instance
 *
 * return: boolean, true if empty
 */

bool
st_empty(
	stcb *
);

/*
 * st_push
 *
 * add an item to the top of the stack.
 *
 *     in: the st instance
 *
 *     in: ppayload
 *
 * return: nothing
 */

void
st_push(
	stcb *,
	ppayload
);

/*
 * st_pop
 *
 * remove and return the top item on the stack.
 *
 *     in: the st instance
 *
 * return: ppayload or NULL if the stack is empty
 */

ppayload
st_pop(
	stcb *
);

/*
 * st_peek
 *
 * return but do not remove the top item on the stack.
 *
 *     in: the st instance
 *
 * return: ppayload or NULL If the stack is empty
 */

ppayload
st_peek(
	stcb *
);

/*
 * st_create
 *
 * create a new stack.
 *
 *     in: nothing
 *
 * return: the new st instance
 */

stcb *
st_create(
	void
);

/*
 * st_destroy
 *
 * free the stack control block if the stack is empty.
 *
 *     in: the st instance
 *
 * return: boolean, true if successful
 */

bool
st_destroy(
	stcb *
);

/*
 * st_reset
 *
 * remove all items from the stack.
 *
 *     in: the st instance
 *
 * return: integer number of items removed
 */

int
st_reset(
	stcb *
);

/*
 * st_depth
 *
 * how many items are in the stack?
 *
 *     in: the st instance
 *
 * return: inegert, number of items on the stack
 */

int
st_depth(
	stcb *
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* st.h ends here */
