/* unitpmute.c -- tests for the permutations generator library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "minunit.h"
#include "../inc/pmute.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

static
void
test_setup(void) {
}

static
void
test_teardown(void) {
}

/*
 * print_current
 */

static
void
print_current(int *iter, int n, int *ints) {
	*iter += 1;
	printf("%6d: ", *iter);
	for (int i = 0; i < n; i++)
		printf("%1d", ints[i]);
	printf("\n");
}

/*
 * permute
 */

static
int
permute(const int n) {

	int *ints = (int *)calloc(n, sizeof(int));
	assert(ints);

	for (int i = 0; i < n; i++)
		ints[i] = i;

	int iter = 0;
	print_current(&iter, n, ints);
	while (permute_next(n, ints))
		print_current(&iter, n, ints);

	free(ints);

	return iter;
}

/*
 * test_pmute
 *
 * there's not much to test here, we just exercise the api and confirm
 * that it created the correct number of permutations.
 */

MU_TEST(test_pmute) {
	printf("\npermute 4\n");
	mu_should(24 == permute(4));
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_pmute);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitpmute.c ends here */
