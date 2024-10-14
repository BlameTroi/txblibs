/* unitda.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "../inc/rand.h"

#include "../inc/da.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

static
void
test_setup(void) {
	/* let's use a different seed than 1, but not time() because i want
	   repeatable tests. */
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
}

static
void
test_teardown(void) {
}

/*
 * test_da
 *
 * the dynamic array is very simple, so only one test function
 * is needed. the small size on create ensures that the array
 * will grow as items are added to it. i didn't bother to clean
 * up the dangling int references as this ends quickly.
 */

MU_TEST(test_da) {
	dacb *da = NULL;
	da = da_create(10);
	mu_should(da && da_count(da) == 0);

	int *item = NULL;
	int sum = 0;
	for (int i = 0; i < 1000; i++) {
		item = malloc(sizeof(int));
		*item = random_between(100, 900);
		sum += *item;
		da_put(da, i, item);
		item = NULL;
	}

	for (int i = 0; i < 1000; i++) {
		int *n = NULL;
		n = da_get(da, i);
		sum -= *n;
		n = NULL;
	}

	mu_should(sum == 0);
	mu_should(da_count(da) == 1000);

	da_destroy(da);
}

/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */

MU_TEST_SUITE(test_suite) {

	/* always have a setup and teardown, even if they */
	/* do nothing. */

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	/* run your tests here */

	MU_RUN_TEST(test_da);
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
