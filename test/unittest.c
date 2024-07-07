/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../inc/misc.h"
#include "../inc/str.h"


/*
 * this just keeps growing and growing, but i resist splitting it out
 * into separate files just yet.
 *
 * tests for:
 *
 * txbstr -- string and character in string functions
 *
 * txbmisc -- factor
 *            min/max
 */

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

/* each test will have id and payload lists available to work with. */

void
test_setup(void) {
   /* let's use a different seed than 1, but not time() because i want
    * repeatable tests. */
   srand(RAND_SEED);
}


/* after each test, release the items and lists properly. */

void
test_teardown(void) {
}

/*
 * begin tests for miscellaneous functions:
 *
 * txbmisc is a catch all of things that don't warrant their own distinct
 * library header.
 */

/* min and max */

MU_TEST(test_min_max) {
   mu_assert_int_eq(1, min(1, 2));
   mu_assert_int_eq(1, min(2, 1));
   mu_assert_int_eq(2, max(1, 2));
   mu_assert_int_eq(2, max(2, 1));
}

MU_TEST(test_even_odd) {
   mu_assert(is_odd(1), "should be odd");
   mu_assert(is_even(2), "should be even");
}

/* still to do: shuffle, can we even? */

/* tests for factors_of */

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

   result = factors_of(0);
   mu_assert_int_eq(true, result == NULL);
   result = factors_of(-33);
   mu_assert_int_eq(true, result == NULL);

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

/*
 * begin tests for string and character functions:
 *
 * these are all in txbstr.
 */

/* test split_string and friends */

MU_TEST(test_split_string) {
   char *s = "this is a test string";
   const char **splits = split_string(s, " ");
   char *ver[] = {NULL, "this", "is", "a", "test", "string", NULL};
   long i = 1;
   while (splits[i]) {
      mu_assert_string_eq(ver[i], splits[i]);
      i += 1;
   }
   free((void *)splits[0]);
   free(splits);

   s = "and, now, for, something! else?";
   splits = split_string(s, " ,?");
   char *ver2[] = {NULL, "and", "now", "for", "something!", "else", NULL};
   i = 1;
   while (splits[i]) {
      mu_assert_string_eq(ver2[i], splits[i]);
      i += 1;
   }
   free((void *)splits[0]);
   free(splits);
}

MU_TEST(test_chars) {
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

   printf("\n\nstring and character\n\n");
   MU_RUN_TEST(test_split_string);
   MU_RUN_TEST(test_chars);

   printf("\n\nfactor\n\n");
   MU_RUN_TEST(test_min_max);
   MU_RUN_TEST(test_even_odd);
   MU_RUN_TEST(test_factor);

   printf("\n\npermute\n\n");
   /* to be provided */
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
