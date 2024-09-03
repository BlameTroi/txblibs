/* unitda.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "minunit.h"

#include "../inc/misc.h"

#include "../inc/rand.h"

#include "../inc/da.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

/* each test will have id and payload lists available to work with. */

void
test_setup(void) {

	/* let's use a different seed than 1, but not time() because i want
	   repeatable tests. */
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
}

void
test_teardown(void) {

}

MU_TEST(test_da) {
	dacb *da = NULL;

	da = da_create(10);

	mu_should(da);
	mu_should(da_length(da) == 0);

	int *leak = NULL;
	int sum = 0;
	for (int i = 0; i < 1000; i++) {
		leak = malloc(sizeof(int));
		*leak = random_between(100, 900);
		sum += *leak;
		da_put(da, i, leak);
		printf("%d %d\n", i, *leak);
		leak = NULL;
	}
	printf("%d\n", sum);

	for (int i = 0; i < 1000; i++) {
		int *n = NULL;
		n = da_get(da, i);
		printf("%d %d\n", i, *n);
		sum -= *n;
		n = NULL;
	}
	printf("%d\n", sum);
	mu_should(sum == 0);
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

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

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
