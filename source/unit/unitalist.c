/* unitalist.c -- unit tests for troi's simple array list */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "txballoc.h"
#include "txbmisc.h"
#include "txbrand.h"

#include "txbalist.h"

/*
 * main() drops these here for anyone who wants them, keeping the code
 * in main() to a minimum.
 */

static
int argc;

static
char **argv;

/*
 * minunit setup and teardown.
 */

void
test_setup(void) {
	srand(6803); /* predictable but different sequence when using the bad generator. */
	tsinitialize(4000, txballoc_f_full, stderr);
}

void
test_teardown(void) {
	tsterminate();
}

/*
 * create and free an empty alist:
 */

MU_TEST(test_create) {
	mu_should(true);
	mu_shouldnt(false);
	alist *xs = make_alist();
	mu_should(alist_length(xs) == 0);
	xs = free_alist(xs);
	mu_shouldnt(xs);
}

/*
 * create, add one, clone, and check that clone has same value but
 * different address.
 */

MU_TEST(test_add_one) {
	alist *xs = make_alist();
	xs = cons_to_alist(xs, (uintptr_t)1);
	mu_should(alist_length(xs) == 1);
	alist *ys = clone_alist(xs);
	mu_should(alist_length(ys) == 1);
	mu_shouldnt(xs == ys);
	xs = free_alist(xs);
	ys = free_alist(ys);
	mu_shouldnt(xs || ys);
}

/*
 * much as in test_add_one, but three is more generally more
 * interesting than one. display what we see.
 */

MU_TEST(test_add_three) {
	alist *xs = make_alist();
	xs = cons_to_alist(xs, (uintptr_t)1);
	mu_should(alist_length(xs) == 1);
	alist *ys = clone_alist(xs);
	mu_should(alist_length(ys) == 1);
	mu_shouldnt(xs == ys);
	xs = free_alist(xs);
	ys = cons_to_alist(ys, (uintptr_t)2);
	ys = cons_to_alist(ys, (uintptr_t)3);
	fprintf(stdout, "\nexamining a list\n");
	fprintf(stdout, "ys: %p  capacity: %d  used: %d\n",
		(void*)ys, ys->capacity, ys->used);
	fprintf(stdout, "ys[0] %lu\n", ys->list[0]);
	fprintf(stdout, "ys[1] %lu\n", ys->list[1]);
	fprintf(stdout, "ys[2] %lu\n", ys->list[2]);
	ys = free_alist(ys);
	mu_shouldnt(xs || ys);
}

/*
 * add enough items to a list to force it to expand. catch
 * it doing so. ALIST_DEFAULT_CAP sets the initial size.
 */

MU_TEST(test_expansion) {
	alist *xs = make_alist();
	mu_should(xs->capacity == ALIST_DEFAULT_CAP);
	mu_should(xs->used == 0);
	alist *original_xs_pointer = xs;
	bool split_seen = false;
	fprintf(stdout, "\ngrowing a list\n");
	int original_capacity = xs->capacity;
	for (int p = 0; p < original_capacity + 4; p += 1) {
		if (xs != original_xs_pointer) {
			fprintf(stdout, "alist split detected after %d\n", p-1);
			original_xs_pointer = xs;
			split_seen = true;
		}
		xs = cons_to_alist(xs, p);
		fprintf(stdout, "iter: %d  xs: %p  cap: %d  used: %d  holds: %lu\n",
			p, (void*)xs, xs->capacity, xs->used, xs->list[p]);
	}
	mu_should(split_seen);
	xs = free_alist(xs);
	mu_shouldnt(xs);
}

/*
 * check out the iterator, does it stop correctly?
 */

MU_TEST(test_iterator) {
	alist *xs = make_alist();
	mu_should(xs->capacity == ALIST_DEFAULT_CAP);
	mu_should(xs->used == 0);
	alist *original_xs_pointer = xs;
	bool split_seen = false;
	fprintf(stdout, "\ncreating expanded list\n");
	int original_capacity = xs->capacity;
	for (int p = 0; p < original_capacity + 4; p += 1) {
		if (xs != original_xs_pointer) {
			fprintf(stdout, "alist split confirmed at %d\n", p-1);
			original_xs_pointer = xs;
			split_seen = true;
		}
		xs = cons_to_alist(xs, p);
		fprintf(stdout, "consed %d\n", p);
	}
	mu_should(split_seen);
	int iterator = 0;
	while (iterator > -1) {
		uintptr_t p = iterate_alist(xs, &iterator);
		fprintf(stdout, "iterator counter %d  retrieved %lu\n", iterator, p);
	}
	mu_should(iterator == -1);
	for (int i = 0; i < xs->used; i++) {
		uintptr_t p = xs->list[i];
		fprintf(stdout, "via for %d = %lu\n", i, p);
	}
	xs = free_alist(xs);
	mu_shouldnt(xs);
}

/*
 * a test suite is made up of tests.
 */

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_add_one);
	MU_RUN_TEST(test_add_three);
	MU_RUN_TEST(test_expansion);
	MU_RUN_TEST(test_iterator);
}

/*
 * and main runs the suites.
 */

int
main(int aargc, char *aargv[]) {
	argc = aargc;
	argv = aargv;

	MU_RUN_SUITE(test_suite);

	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitalist.c ends here */
