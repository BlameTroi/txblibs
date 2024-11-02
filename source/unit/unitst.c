/* unitst.c -- tests for the stack header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "txbst.h"

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

MU_TEST(test_st) {

	hst *st = st_create();
	mu_should(st);
	mu_should(st_empty(st));
	for (int i = 1; i < 5; i++) {
		st_push(st, (void *)(long)i);
		printf("%ld\n", (long)st_peek(st));
	}
	mu_shouldnt(st_destroy(st));
	for (int i = 1; i < 5; i++) {
		long x = (long)st_pop(st);
		printf("%ld\n", x);
	}
	mu_should(st_destroy(st));
	st = NULL;

	st = st_create();
	mu_should(st);
	mu_should(st_empty(st));
	st_push(st, "a");
	st_push(st, "b");
	mu_should(st_pop(st));
	mu_should(st_pop(st));
	mu_should(st_empty(st));
	mu_should(st_destroy(st));
	st = NULL;

	st = st_create();
	mu_should(st && st_empty(st));
	st_push(st, "a");
	st_push(st, "b");
	mu_shouldnt(st_empty(st));
	mu_shouldnt(st_destroy(st));
	mu_should(st_reset(st) == 2);
	mu_should(st_destroy(st));
	st = NULL;
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_st);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitst.c ends here */
