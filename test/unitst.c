/* unitst.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../inc/rand.h"

#include "../inc/st.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

static
void
test_setup(void) {
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
}

static
void
test_teardown(void) {
}

MU_TEST(test_st) {

	stcb *st = st_create();
	mu_should(st);
	mu_should(st_empty(st));
	for (int i = 0; i < 5; i++) {
		st_push(st, (void *)(long)i);
		printf("%ld\n", (long)st_peek(st));
	}
	mu_shouldnt(st_destroy(st));
	for (int i = 0; i < 5; i++) {
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

	MU_RUN_TEST(test_st);
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
