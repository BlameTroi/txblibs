/* st.h -- blametroi's fixed size stack -- */

/*
 * a header only implementation of a stack.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>

/*
 * an instance of a stack.
 */

typedef struct stcb stcb;

/*
 * st_create
 *
 * create a new stack instance. there is no hard limit on the number of
 * items on the stack.the memory allocation is large enough to hold
 *
 * return: the st instance
 */

stcb *
st_create(
	void
);

/*
 * st_push
 *
 * place a new item on the top of the stack.
 *
 *     in: the st instance
 *
 *     in: the item as a void *
 *
 * return: none
 */

void
st_push(
	stcb *st,
	void *item
);

/*
 * st_pop
 *
 * remove the top item from the stack.
 *
 *     in: the st instance
 *
 * return: the item as a void *
 */

void *
st_pop(
	stcb *st
);

/*
 * st_peek
 *
 * return the value of the top item on the stack, leaving it
 * on the stack.
 *
 *     in: the st instance
 *
 * return: the item as a void *
 */

void *
st_peek(
	stcb *st
);

/*
 * st_empty
 *
 * is the stack empty?
 *
 *     in: the st instance
 *
 * return: boolean
 */

bool
st_empty(
	stcb *st
);

/*
 * st_depth
 *
 * how many items are on the stack?
 *
 *     in: the st instance
 *
 * return: int number of items
 */

int
st_depth(
	stcb *st
);

/*
 * st_destroy
 *
 * free stack resources if the stack is empty.
 *
 *     in: the st instance
 *
 * return: boolean true if successful false if not
 */

bool
st_destroy(
	stcb *st
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* st.h ends here */
