/* unitrs.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "minunit.h"

#include "../inc/str.h"
#include "../inc/rs.h"

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

MU_TEST(test_rs) {
   const char *testing = "this is a test";
   char *copy = dup_string(testing);
   rscb *rs = rs_create_string(copy);
   free(copy);
   copy = "changed";
   mu_should(rs_position(rs) == 0);
   int c = rs_getc(rs);
   mu_should(c == 't');
   mu_shouldnt(c == copy[0]);
   rs_ungetc(rs);
   const char *p = testing;
   while (*p) {
      c = rs_getc(rs);
      mu_should(c == *p);
      p += 1;
   }
   printf("\n%c %ld %s\n", c, rs_position(rs), rs_at_end(rs) ? "true" : "false");
   /* semantics, we have not actually read the end yet. */
   mu_shouldnt(rs_at_end(rs));
   c = rs_getc(rs);
   mu_should(c == EOF);
   mu_should(rs_at_end(rs));
   rs_rewind(rs);
   mu_shouldnt(rs_at_end(rs));
   char fwd[4] = { 0, 0, 0, 0 };
   char bwd[4] = { 0, 0, 0, 0 };
   for (int i = 0; i < 4; i++) {
      fwd[i] = rs_getc(rs);
   }
   for (int i = 0; i < 4; i++) {
      rs_ungetc(rs);
      bwd[i] = rs_peekc(rs);
   }
   printf("\n");
   for (int i = 0; i < 4; i++) {
      printf("%d %c %c\n", i, fwd[i], bwd[i]);
   }
   for (int i = 0; i < 4; i++) {
      mu_should(fwd[i] == bwd[3-i]);
   }
   rs_destroy_string(rs);
}

MU_TEST(test_clone) {
   rscb *original = rs_create_string("this is a test");
   rscb *clone = rs_clone(original);
   int c;
   int n = 0;
   while ((c = rs_getc(original)) != EOF) {
      n += 1;
   }
   int k = 0;
   while ((c = rs_getc(clone)) != EOF) {
      k += 1;
   }
   mu_should(k == n);
   rs_destroy_string(original);
   rs_destroy_string(clone);
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

   MU_RUN_TEST(test_rs);
   MU_RUN_TEST(test_clone);
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
