/* unitrand.c -- tests for the random numbers header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include "minunit.h"
#include "txbrand.h"
#include <stdio.h>
#include <stdint.h>

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * basic functionality tests.
 */

MU_TEST(test_rand) {

	/* select and seed rand() */
	mu_should(set_random_generator(RAND_DEFAULT));
	mu_should(seed_random_generator(RAND_SEED));

	/* select and attempt to seed arc4random() */
	mu_should(set_random_generator(RAND_RANDOM));
	mu_shouldnt(seed_random_generator(RAND_SEED));

	/* just beat up the arc generator */
	int n100 = 0;
	int n1000 = 0;
	int counts[1001];
	memset(counts, 0, sizeof(counts));
	for (int i = 0; i < 100000; i++) {
		uint32_t r = random_between(100, 1000);
		if (r < 100 || r > 1000) mu_shouldnt(true);
		if (r == 100) {
			mu_should(true);
			n100 += 1;
		}
		if (r == 1000) {
			mu_should(true);
			n1000 += 1;
		}
		counts[r] += 1;
	}
	printf("\n100 -> %d     1000 -> %d\n", n100, n1000);
	printf("\n");
	for (int i = 100; i <= 1000; i++)
		printf("%4d %6d\n", i, counts[i]);

	/* now switch to rand() and see if we're back */
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
	uint32_t first[10000];
	uint32_t last[10000];
	memset(first, 0, sizeof(first));
	memset(last, 0, sizeof(last));
	for (int i = 0; i < 10000; i++)
		first[i] = random_between(100, 99999);
	seed_random_generator(RAND_SEED);
	for (int i = 0; i < 10000; i++)
		last[i] = random_between(100, 99999);
	mu_should(memcmp(first, last, sizeof(first)) == 0);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_rand);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitrand.c ends here */
