/*  unitmd5.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "minunit.h"

#include "../inc/md5.h"

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

static
void
test_setup(void) {
	/* let's use a different seed than 1, but not time() because i want
	   repeatable tests. */
	srand(RAND_SEED);
}

static
void
test_teardown(void) {
}

static
void
print_hash(
	uint8_t *p
) {
	for (unsigned int i = 0; i < 16; ++i)
		printf("%02x", p[i]);
	printf("\n");
}

/*
 * i'm just testing to make sure things are wired together and working
 * as expected. a common problem class in advent of code uses md5 hash
 * to find candidate passwords by hashing a string with an odometering
 * value and at every hash result that begins with some number of
 * zeros, the first non-zero hex digit is used for additional work in
 * the problem. the following three tests are from AOC 2016 day 05. if
 * they work, the wiring works.
 */

MU_TEST(test_test) {
	uint8_t result[16];

	printf("\n");
	char *str1 = "abc3231929";
	md5_string(str1, result);
	print_hash(result);
	mu_should((result[2] & 0x0f) == 1);

	printf("\n");
	char *str2 = "abc5017308";
	md5_string(str2, result);
	print_hash(result);
	mu_should((result[2] & 0x0f) == 8);

	printf("\n");
	char *str3 = "abc5278568";
	md5_string(str3, result);
	print_hash(result);
	mu_should((result[2] & 0x0f) == 0x0f);
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

	MU_RUN_TEST(test_test);

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
