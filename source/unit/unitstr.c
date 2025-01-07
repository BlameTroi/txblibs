/*  unitstr.c -- tests for the string utilities header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "../inc/str.h"

/*
 * minunit setup and teardown of listd infratstructure.
 */

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * test split_string and friends.
 */

MU_TEST(test_split_string) {
	char *s = "this is a test string";
	const char **splits = split_string(s, " ");
	char *ver[] = {NULL, "this", "is", "a", "test", "string", NULL};
	long i = 1;
	while (splits[i]) {
		mu_should(equal_string(ver[i], splits[i]));
		i += 1;
	}
	free((void *)splits[0]);
	free(splits);

	s = "and, now, for, something! else?";
	splits = split_string(s, " ,?");
	char *ver2[] = {NULL, "and", "now", "for", "something!", "else", NULL};
	i = 1;
	while (splits[i]) {
		mu_should(equal_string(ver2[i], splits[i]));
		i += 1;
	}
	free((void *)splits[0]);
	free(splits);
}

MU_TEST(test_pos_char) {
	mu_assert_int_eq(1, pos_char("asdf", 0, 's'));
	mu_assert_int_eq(-1, pos_char("qwerty", 0, 's'));
	mu_assert_int_eq(-1, pos_char("asdf", 2, 's'));
	mu_assert_int_eq(0, pos_char("this not that", 0, 't'));
	mu_assert_int_eq(7, pos_char("this not that", 1, 't'));
	mu_assert_int_eq(7, pos_char("this not that", 7, 't'));
	mu_assert_int_eq(9, pos_char("this not that", 8, 't'));
	mu_assert_int_eq(12, pos_char("this not that", 10, 't'));
	mu_assert_int_eq(12, pos_char("this not that", 12, 't'));
	mu_assert_int_eq(-1, pos_char("", 0, 'x'));
	mu_assert_int_eq(-1, pos_char("asdf", 5, 'f'));
	mu_assert_int_eq(-1, pos_char("zxcvb", -3, 'g'));
}

MU_TEST(test_count_char) {
	mu_should(count_char("asdfijkl", 'a') == 1);
	mu_should(count_char("asdfijkl", 'l') == 1);
	mu_should(count_char("asdfasdfasdf", 'a') == 3);
	mu_should(count_char("asdfasdfasdf", 'z') == 0);
}

MU_TEST(test_compare_string) {
	char *dup = strdup("hello");
	mu_should(equal_string("hello", dup));
	mu_shouldnt(less_than_string("hello", dup));
	mu_shouldnt(greater_than_string("hello", dup));
	mu_should(less_than_string("asdf", "f"));
	mu_shouldnt(equal_string("asdf", "f"));
	mu_shouldnt(greater_than_string("asdf", "f"));
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	printf("\n\nstring and character\n\n");
	MU_RUN_TEST(test_split_string);
	MU_RUN_TEST(test_pos_char);
	MU_RUN_TEST(test_count_char);
	MU_RUN_TEST(test_compare_string);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitstr.c ends here */
