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
 * sb_create
 *
 * create a new empty stack instance.
 *
 *     in: nothing
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
 * push a new item on the stack.
 *
 *     in: the st instance
 *
 *     in: the item as a void *
 *
 * return: nothing
 */

void
st_push(
	stcb *st,
	void *item
);
/*
 * st_pop
 *
 * pop an item off the stack.
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
 * get the top item from the stack without removing it.
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
 *    in: the st instance
 *
 * return: bool
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
 *    in: the st instance
 *
 * return: int
 */

int
st_depth(
	stcb *st
);

/*
 * st_reset
 *
 * delete all items from the stack.
 *
 *     in: the st instance
 *
 * return: int number of items deleted
 */

int
st_reset(
	stcb *st
);

/*
 * sb_destroy
 *
 * if the stack is empty, release its resources.
 *
 *     in: the st instance
 *
 * return: bool was the st destroyed and freed
 */

bool
st_destroy(
	stcb *st
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* st.h ends here */
