/* unitda.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "../inc/str.h"

#include "../inc/ll.h"

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
 * a fifo lleue is pretty simple, so just one test running through the
 * functions.
 */

MU_TEST(test_ll) {
	llcb *ll = NULL;

	ll = ll_create();
	mu_should(ll);
	mu_should(ll_empty(ll));
	ll_add_last(ll, "one");
	ll_add_last(ll, "two");
	mu_should(ll_count(ll) == 2);
	ll_add_last(ll, "three");
	mu_should(equal_string("one", ll_remove_first(ll)));
	mu_should(equal_string("two", ll_peek_first(ll)));
	mu_should(ll_count(ll) == 2);
	mu_should(equal_string("two", ll_remove_first(ll)));
	mu_should(ll_count(ll) == 1);
	mu_shouldnt(ll_destroy(ll));
	mu_shouldnt(ll_empty(ll));
	mu_should(equal_string("three", ll_remove_first(ll)));
	mu_should(ll_count(ll) == 0);
	mu_shouldnt(ll_remove_first(ll));
	mu_should(ll_destroy(ll));

	ll = ll_create();
	ll_add_last(ll, "one");
	ll_add_last(ll, "two");
	mu_shouldnt(ll_destroy(ll));
	mu_should(ll_reset(ll) == 2);
	mu_should(ll_destroy(ll));

	/* test add last (push) and remove first (pop) */
	ll = ll_create();
	ll_add_last(ll, "add_last_one");
	ll_add_last(ll, "add_last_two");
	ll_add_last(ll, "add_last_three");
	mu_should(ll_count(ll) == 3);
	mu_should(equal_string(ll_remove_first(ll), "add_last_one"));
	mu_should(equal_string(ll_remove_first(ll), "add_last_two"));
	mu_should(equal_string(ll_remove_first(ll), "add_last_three"));
	mu_should(ll_remove_first(ll) == NULL);
	ll_destroy(ll);

	/* test add first (enqueue) and remove last (dequeue) */
	ll = ll_create();
	ll_add_first(ll, "add_first_one");
	ll_add_first(ll, "add_first_two");
	ll_add_first(ll, "add_first_three");
	mu_should(ll_count(ll) == 3);
	ppayload pl = ll_remove_last(ll);
	printf("\n%s", (char *)pl);
	mu_should(equal_string(pl, "add_first_one"));
	pl = ll_remove_last(ll);
	printf("\n%s", (char *)pl);
	mu_should(equal_string(pl, "add_first_two"));
	pl = ll_remove_last(ll);
	printf("\n%s", (char *)pl);
	mu_should(equal_string(pl, "add_first_three"));
	mu_should(ll_remove_first(ll) == NULL);
	ll_destroy(ll);
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

	MU_RUN_TEST(test_ll);
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
