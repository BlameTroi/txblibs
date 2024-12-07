/* unitlog2.c -- whitebox tests for balancing a scapegoat tree */

#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "txballoc.h"
#include "txbrand.h"

#include "txblog2.h"

/********************************************************************
 * utility code and common data
 ********************************************************************/

/*
 * main() drops these here for anyone who wants them.
 */

static
int argc;

static
char **argv;

/********************************************************************
 * unit test setup and teardown, run before/after each MU_TEST()
 ********************************************************************/

static
void
test_setup(void) {
	seed_random_generator(6803);
	tsinitialize(55000, txballoc_f_errors, stderr);
}

static
void
test_teardown(void) {
	tsterminate();
}

/********************************************************************
 * unit tests, ideally focused functions to exercise code. i do both
 * white and black box testing. each test contains one or more
 * assertions. while minunit provides many, i tend to use mu_should()
 * and mu_shouldnt().
 ********************************************************************/

/*
 * test the log 2 function, just crank some out, these are integer
 * logs.
 */

MU_TEST(test_log2) {
	for (uint32_t i = 0; i < 1024; i++) {
		fprintf(stdout, "%d %d\n", i, uint32_log2(i));
	}
	mu_should(true);
}

/********************************************************************
 * a test suite invokes one or more tests. while it is possible to
 * have multiple suites, i don't.
 ********************************************************************/

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_log2);

	return;

}

/********************************************************************
 * start me up
 ********************************************************************/

int
main(int aargc, char *aargv[]) {
	argc = aargc;
	argv = aargv;

	MU_RUN_SUITE(test_suite);

	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitbal.c ends here */
