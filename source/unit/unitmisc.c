/*  unitmisc.c -- tests for the miscellany header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "../inc/misc.h"

/*
 * minunit setup and teardown.
 */

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * begin tests for miscellaneous functions:
 *
 * txbmisc is a catch all of things that don't warrant their own distinct
 * library header.
 */

MU_TEST(test_min_max) {
	mu_assert_int_eq(1, min(1, 2));
	mu_assert_int_eq(1, min(2, 1));
	mu_assert_int_eq(2, max(1, 2));
	mu_assert_int_eq(2, max(2, 1));
	double mx = 3.50;
	double mn = 3.25;
	mu_should(min(mx, mn) == mn);
}

MU_TEST(test_even_odd) {
	mu_should(is_odd(1));
	mu_shouldnt(is_even(1));
	mu_should(is_even(2));
	mu_shouldnt(is_odd(2));
}

MU_TEST(test_factor) {
	int factors2[] = {1, 2, 0};
	int factors4[] = {1, 2, 4, 0};
	int factors20[] = {1, 2, 4, 5, 10, 20, 0};
	long *result = NULL;
	int i = 0;

	result = factors_of(2);
	i = 0;
	while (factors2[i]) {
		mu_assert_int_eq(factors2[i], result[i]);
		i += 1;
	}
	free(result);

	mu_shouldnt(factors_of(0));
	mu_shouldnt(factors_of(-33));

	result = factors_of(4);
	i = 0;
	while (factors4[i]) {
		mu_assert_int_eq(factors4[i], result[i]);
		i += 1;
	}
	free(result);

	result = factors_of(20);
	i = 0;
	while (factors4[i]) {
		mu_assert_int_eq(factors20[i], result[i]);
		i += 1;
	}
	free(result);

	/* left overs left in just to exercise the code */
	result = factors_of(99);
	free(result);
	result = factors_of(100);
	free(result);
	result = factors_of(999);
	free(result);
	result = factors_of(1000);
	free(result);
	result = factors_of(9999);
	free(result);
	result = factors_of(10000);
	free(result);
	result = factors_of(99999);
	free(result);
	result = factors_of(100000);
	free(result);
	result = factors_of(999999);
	free(result);
	result = factors_of(1000000);
	free(result);
	result = factors_of(33100000);
	free(result);
}

MU_TEST(test_pack) {
	char chr_buffer[64];
	uint8_t hex_buffer[32];
	memset(hex_buffer, 0, 32);
	memset(chr_buffer, 0, 64);
	hex_pack(hex_buffer, 32, "0123456789abcdef", 16);
	hex_unpack(chr_buffer, 64, hex_buffer, 8);
	mu_should(strcmp(chr_buffer, "0123456789abcdef") == 0);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_min_max);
	MU_RUN_TEST(test_even_odd);
	MU_RUN_TEST(test_factor);
	MU_RUN_TEST(test_pack);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitmisc.c ends here */
