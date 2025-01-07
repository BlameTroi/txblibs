/* unitll.c -- tests for singly linked list library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "../inc/str.h"
#include "../inc/one.h"

/*
 * minunit setup and teardown.
 */

static
void
test_setup(void) {
}

static
void
test_teardown(void) {
}

/*
 * test_ll
 *
 * a fifo queue is pretty simple, so just one test running through the
 * functions.
 */

MU_TEST(test_q) {
	one_block *q = NULL;

	q = make_one(queue);
	mu_should(q);
	mu_should(is_empty(q));
	enqueue(q, "one");
	enqueue(q, "two");
	mu_should(count(q) == 2);
	enqueue(q, "three");
	mu_should(equal_string("one", dequeue(q)));
	mu_should(equal_string("two", peek(q)));
	mu_should(count(q) == 2);
	mu_should(equal_string("two", dequeue(q)));
	mu_should(count(q) == 1);
	mu_should(purge(q) == 1);
	mu_should(is_empty(q));
	mu_shouldnt(equal_string("three", dequeue(q)));
	mu_should(count(q) == 0);
	mu_shouldnt(dequeue(q));
	free_one(q);

	q = make_one(queue);
	enqueue(q, "one");
	enqueue(q, "two");
	mu_should(purge(q) == 2);
	free_one(q);

	q = make_one(queue);
	enqueue(q, "enqueue_one");
	enqueue(q, "enqueue_two");
	enqueue(q, "enqueue_three");
	mu_should(count(q) == 3);
	mu_should(equal_string(dequeue(q), "enqueue_one"));
	mu_should(equal_string(dequeue(q), "enqueue_two"));
	mu_should(equal_string(dequeue(q), "enqueue_three"));
	mu_should(dequeue(q) == NULL);
	free_one(q);

	/* test add first (enqueue) and remove last (dequeue) */
	q = make_one(queue);
	enqueue(q, "add_first_one");
	enqueue(q, "add_first_two");
	enqueue(q, "add_first_three");
	mu_should(count(q) == 3);
	void *pl = dequeue(q);
	printf("\n%s", (char *)pl);
	mu_should(equal_string(pl, "add_first_one"));
	pl = dequeue(q);
	printf("\n%s", (char *)pl);
	mu_should(equal_string(pl, "add_first_two"));
	pl = dequeue(q);
	printf("\n%s", (char *)pl);
	mu_should(equal_string(pl, "add_first_three"));
	mu_should(dequeue(q) == NULL);
	free_one(q);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_q);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitll.c ends here */
