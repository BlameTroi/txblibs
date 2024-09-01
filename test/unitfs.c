/* unitfs.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../inc/fs.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

/* each test will have id and payload lists available to work with. */

void
test_setup(void) {

	/* let's use a different seed than 1, but not time() because i want
	   repeatable tests. */
	srand(RAND_SEED);
}

void
test_teardown(void) {

}

MU_TEST(test_fs) {

	fscb *fs = fs_create(5);
	mu_should(fs);
	mu_should(fs_empty(fs));
	for (int i = 0; i < 5; i++) {
		fs_push(fs, (void *)(long)i);
		printf("%ld\n", (long)fs_peek(fs));
	}
	mu_shouldnt(fs_destroy(fs));
	for (int i = 0; i < 5; i++) {
		long x = (long)fs_pop(fs);
		printf("%ld\n", x);
	}
	mu_should(fs_destroy(fs));
	fs = NULL;

	fs = fs_create(2);
	mu_should(fs);
	mu_should(fs_empty(fs));
	fs_push(fs, "a");
	fs_push(fs, "b");
	mu_should(fs_full(fs));
	mu_should(fs_pop(fs));
	mu_should(fs_pop(fs));
	mu_should(fs_empty(fs));
	mu_should(fs_destroy(fs));

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

	MU_RUN_TEST(test_fs);
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
