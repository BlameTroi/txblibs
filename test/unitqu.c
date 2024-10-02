/* unitda.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "../inc/str.h"

#include "../inc/qu.h"

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
	qucb *qu = NULL;

	qu = qu_create();
	mu_should(qu);
	mu_should(qu_empty(qu));
	qu_enqueue(qu, "one");
	qu_enqueue(qu, "two");
	mu_should(qu_count(qu) == 2);
	qu_enqueue(qu, "three");
	mu_should(equal_string("one", qu_dequeue(qu)));
	mu_should(equal_string("two", qu_peek(qu)));
	mu_should(qu_count(qu) == 2);
	mu_should(equal_string("two", qu_dequeue(qu)));
	mu_should(qu_count(qu) == 1);
	mu_shouldnt(qu_destroy(qu));
	mu_shouldnt(qu_empty(qu));
	mu_should(equal_string("three", qu_dequeue(qu)));
	mu_should(qu_count(qu) == 0);
	mu_shouldnt(qu_dequeue(qu));
	mu_should(qu_destroy(qu));

	qu = qu_create();
	qu_enqueue(qu, "one");
	qu_enqueue(qu, "two");
	mu_shouldnt(qu_destroy(qu));
	mu_should(qu_reset(qu) == 2);
	mu_should(qu_destroy(qu));

}

/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */

MU_TEST_SUITE(test_suite) {

	/* always have a setup and teardown, even if they
	 * do nothing. */

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	/* run your tests here */

	MU_RUN_TEST(test_qu);
}

/*
 * master control:
 */

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitqu.c ends here */
