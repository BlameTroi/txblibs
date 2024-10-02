/* unitrs.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */


#include <stdlib.h>

#include "minunit.h"

#include "../inc/str.h"

#include "../inc/sb.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

void
test_setup(void) {
	srand(RAND_SEED);
}

void
test_teardown(void) {
}

/*
 * basic functionality tests.
 */

MU_TEST(test_basic) {
	sbcb *sb = sb_create();
	mu_should(sb);
	mu_should(sb_length(sb) == 0);

	char *temp = sb_to_string(sb);
	mu_should(temp);
	mu_should(strlen(temp) == 0);
	free(temp);

	sb_puts(sb, "one");
	mu_should(sb_length(sb) == 3);
	sb_putc(sb, ' ');
	mu_should(sb_length(sb) == 4);
	sb_puts(sb, "two");
	mu_should(sb_length(sb) == 7);
	temp = sb_to_string(sb);
	mu_should(temp);
	mu_should(strlen(temp) == sb_length(sb));
	mu_should(equal_string(temp, "one two"));
	free(temp);

	sb_destroy(sb);
}

/*
 * the same as test_sb except use the null sink string builder
 */
MU_TEST(test_null) {
	sbcb *sb = sb_create_null();
	mu_should(sb);
	mu_should(sb_length(sb) == 0);

	char *temp = sb_to_string(sb);
	mu_should(temp);
	mu_should(strlen(temp) == 0);
	free(temp);

	sb_puts(sb, "one");
	mu_should(sb_length(sb) == 3);
	sb_putc(sb, ' ');
	mu_should(sb_length(sb) == 4);
	sb_puts(sb, "two");
	mu_should(sb_length(sb) == 7);
	temp = sb_to_string(sb);
	mu_should(temp);
	mu_should(strlen(temp) == 0);
	mu_shouldnt(strlen(temp) == sb_length(sb));
	mu_shouldnt(equal_string(temp, "one two"));
	free(temp);

	sb_destroy(sb);
}

/*
 * small blocksizes and large additions.
 */

MU_TEST(test_abusive) {
	sbcb *sb = sb_create_blksize(32);
	for (int i = 0; i < 100; i++)
		sb_puts(sb, "four");
	mu_should(sb_length(sb) == 400);
	sb_destroy(sb);

	sb = sb_create_blksize(4);
	for (int i = 0; i < 100; i++)
		sb_puts(sb, "i'm bigger than two blocksizes, yeah");
	mu_should(sb_length(sb) == 100 *
		strlen("i'm bigger than two blocksizes, yeah"));
	sb_destroy(sb);
}

MU_TEST(test_file) {
	FILE *file = fopen("unitrs.c", "r");
	sbcb *source = sb_create_file(file);
	mu_should(source);
	mu_should(sb_length(source) > 2000); /* just a did we get it check? */
	fclose(file);
	sb_destroy(source);
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

	MU_RUN_TEST(test_basic);
	MU_RUN_TEST(test_null);
	MU_RUN_TEST(test_abusive);
	MU_RUN_TEST(test_file);
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
/* unitsb.c ends here */
