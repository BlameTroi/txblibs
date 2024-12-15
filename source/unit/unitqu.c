/* unitqu.c -- tests for the fifo queue header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "txbstr.h"
#include "txbone.h"

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
 * test_qu
 *
 * a fifo queue is pretty simple, so just one test running through the
 * functions.
 */

MU_TEST(test_qu) {
	one_block *qu = NULL;

	qu = make_one(queue);
	mu_should(qu);
	mu_should(is_empty(qu));
	enqueue(qu, "one");
	enqueue(qu, "two");
	mu_should(count(qu) == 2);
	enqueue(qu, "three");
	mu_should(equal_string("one", dequeue(qu)));
	mu_should(equal_string("two", peek(qu)));
	mu_should(count(qu) == 2);
	mu_should(equal_string("two", dequeue(qu)));
	mu_should(count(qu) == 1);
	mu_shouldnt(is_empty(qu));
	mu_should(equal_string("three", dequeue(qu)));
	mu_should(count(qu) == 0);
	mu_shouldnt(dequeue(qu));
	free_one(qu);

	qu = make_one(queue);
	enqueue(qu, "one");
	enqueue(qu, "two");
	mu_should(purge(qu) == 2);
	free_one(qu);

}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_qu);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitqu.c ends here */
