/* unitst.c -- tests for the stack header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "txbone.h"

/*
 * minunit setup and teardown.
 */

static
void
tesetup(void) {
}

static
void
teteardown(void) {
}

MU_TEST(test) {

	one_block *st = make_one(stack);
	mu_should(st);
	mu_should(empty(st));
	for (int i = 1; i < 5; i++) {
		push(st, (void *)(long)i);
		printf("%ld\n", (long)peek(st));
	}
	mu_should(depth(st) == 4);
	for (int i = 1; i < 5; i++) {
		long x = (long)pop(st);
		printf("%ld\n", x);
	}
	mu_should(free_one(st));
	st = NULL;

	st = make_one(stack);
	mu_should(st);
	mu_should(empty(st));
	push(st, "a");
	push(st, "b");
	mu_should(pop(st));
	mu_should(pop(st));
	mu_should(empty(st));
	mu_should(free_one(st));
	st = NULL;

	st = make_one(stack);
	mu_should(st && empty(st));
	push(st, "a");
	push(st, "b");
	mu_shouldnt(empty(st));
	mu_should(purge(st) == 2);
	mu_should(free_one(st));
	st = NULL;
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(tesetup, teteardown);

	MU_RUN_TEST(test);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitst.c ends here */
