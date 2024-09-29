/* unitmute.c -- units for my header libraries -- troy brumley */

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

	MU_RUN_TEST(test_pmute);
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
/* unitmute.c ends here */
