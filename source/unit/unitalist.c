/* unitalist.c -- unit tests for troi's simple array list */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "../inc/alloc.h"
#include "../inc/misc.h"
#include "../inc/rand.h"
#include "../inc/one.h"

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
	tsinitialize(4000, txballoc_f_errors, stderr);
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
	one_block *xs = make_one(alist);
	mu_should(count(xs) == 0);
	mu_should(is_empty(xs));
	xs = free_one(xs);
	mu_shouldnt(xs);
}

/*
 * create, add one, clone, and check that clone has same value but
 * different address.
 */

MU_TEST(test_add_one) {
	one_block *xs = make_one(alist);
	xs = cons(xs, (uintptr_t)1);
	mu_should(count(xs) == 1);
	one_block *ys = clone(xs);
	mu_should(count(ys) == 1);
	mu_shouldnt(xs == ys);
	xs = free_one(xs);
	ys = free_one(ys);
	mu_shouldnt(xs || ys);
}

/*
 * much as in test_add_one, but three is more generally more
 * interesting than one. display what we see.
 */

MU_TEST(test_add_three) {
	one_block *xs = make_one(alist);
	xs = cons(xs, (uintptr_t)1);
	mu_should(count(xs) == 1);
	one_block *ys = clone(xs);
	mu_should(count(ys) == 1);
	mu_shouldnt(xs == ys);
	xs = free_one(xs);
	ys = cons(ys, (uintptr_t)2);
	ys = cons(ys, (uintptr_t)3);
	fprintf(stderr, "\nexamining a list\n");
	fprintf(stderr, "ys: %p  capacity: %d  used: %d\n",
		(void*)ys, ys->u.acc.capacity, ys->u.acc.used);
	fprintf(stderr, "ys[0] %lu\n", ys->u.acc.list[0]);
	fprintf(stderr, "ys[1] %lu\n", ys->u.acc.list[1]);
	fprintf(stderr, "ys[2] %lu\n", ys->u.acc.list[2]);
	ys = free_one(ys);
	mu_shouldnt(xs || ys);
}

/*
 * add enough items to a list to force it to expand. catch
 * it doing so. ONE_BLOCK_DEFAULT_CAP sets the initial size.
 */

MU_TEST(test_expansion) {
	one_block *xs = make_one(alist);
	mu_should(xs->u.acc.capacity == ONE_ALIST_DEFAULT_CAPACITY);
	mu_should(xs->u.acc.used == 0);
	one_block *original_xs_pointer = xs;
	bool split_seen = false;
	fprintf(stderr, "\ngrowing a list\n");
	int original_capacity = xs->u.acc.capacity;
	for (int p = 0; p < original_capacity + 4; p += 1) {
		if (xs != original_xs_pointer) {
			fprintf(stderr, "one_block split detected after %d\n", p-1);
			original_xs_pointer = xs;
			split_seen = true;
		}
		xs = cons(xs, p);
		fprintf(stderr, "iter: %d  xs: %p  cap: %d  used: %d  holds: %lu\n",
			p, (void*)xs, xs->u.acc.capacity, xs->u.acc.used, xs->u.acc.list[p]);
	}
	mu_should(split_seen);
	xs = free_one(xs);
	mu_shouldnt(xs);
}

/*
 * check out the iterator, does it stop correctly?
 */

MU_TEST(test_iterator) {
	one_block *xs = make_one(alist);
	mu_should(xs->u.acc.capacity == ONE_ALIST_DEFAULT_CAPACITY);
	mu_should(xs->u.acc.used == 0);
	one_block *original_xs_pointer = xs;
	bool split_seen = false;
	fprintf(stderr, "\ncreating expanded list\n");
	int original_capacity = xs->u.acc.capacity;
	for (int p = 0; p < original_capacity + 4; p += 1) {
		if (xs != original_xs_pointer) {
			fprintf(stderr, "one_block split confirmed at %d\n", p-1);
			original_xs_pointer = xs;
			split_seen = true;
		}
		xs = cons(xs, p);
		fprintf(stderr, "consed %d\n", p);
	}
	mu_should(split_seen);
	int iterator = 0;
	while (iterator > -1) {
		/* remember the iterate is increments so it points to next */
		uintptr_t p = iterate(xs, &iterator);
		fprintf(stderr, "iterator counter %d  retrieved %lu\n", iterator - 1, p);
	}
	mu_should(iterator == -1);
	for (int i = 0; i < xs->u.acc.used; i++) {
		uintptr_t p = xs->u.acc.list[i];
		fprintf(stderr, "via for %d = %lu\n", i, p);
	}
	xs = free_one(xs);
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
