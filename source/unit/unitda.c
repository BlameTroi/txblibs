/* unitda.c -- tests for the dynamic array library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "../inc/one.h"
#include "../inc/rand.h"

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
 * will grow as items are added to it.
 */

MU_TEST(test_da) {
	one_block *da = NULL;
	da = make_one(dynarray);
	mu_should(da && high_index(da) < 1);

	void *item = NULL;
	int sum = 0;
	for (int i = 0; i < 10000; i++) {
		item = malloc(sizeof(int));
		*(int *)item = random_between(100, 900);
		sum += *(int *)item;
		put_at(da, item, i);
	}
	mu_should(sum != 0);
	mu_should(high_index(da) == 9999);

	for (int i = 0; i < 10000; i++) {
		int *n = NULL;
		n = get_from(da, i);
		sum -= *n;
		free(n);
	}

	mu_should(sum == 0);

	free_one(da);
}

/*
 * master control:
 */

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_da);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitda.c ends here */
