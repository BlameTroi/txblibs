/* unitsb.c -- tests for the string builder header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <errno.h>
#include <stdlib.h>
#include "minunit.h"
#include "txbstr.h"
#include "txbsb.h"

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
	hsb *sb = sb_create();
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
	hsb *sb = sb_create_null();
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
	hsb *sb = sb_create_blksize(32);
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

static char *filename = NULL;

MU_TEST(test_file) {
	if (filename == NULL) {
		fprintf(stderr, "no file provided, test skipped.");
		return;
	}
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		int keep = errno;
		fprintf(stderr, "could not open file %s, error %d, test skipped", filename,
			keep);
		return;
	}
	hsb *source = sb_create_file(file);
	mu_should(source);
	mu_should(sb_length(source) > 2000); /* just a did we get it check? */
	fclose(file);
	sb_destroy(source);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_basic);
	MU_RUN_TEST(test_null);
	MU_RUN_TEST(test_abusive);
	MU_RUN_TEST(test_file);
}

int
main(int argc, char *argv[]) {
	if (argc > 1)
		filename = argv[1];
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitsb.c ends here */
