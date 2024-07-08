/*  unitpq.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"


#include "../inc/misc.h"

#include "../inc/pq.h"

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

/* each test will have id and payload lists available to work with. */

void
test_setup(void) {

   /* let's use a different seed than 1, but not time() because i want
      repeatable tests. */
   srand(RAND_SEED);
}


/* after each test, release the items and lists properly. */

void
test_teardown(void) {

}

bool test_threaded = false;

/*
 * the priority queue tests:
 */

MU_TEST(test_pq_create) {
   pqcb *pq = pq_create(test_threaded);
   mu_should(pq);
   mu_should(pq_destroy(pq));
}

MU_TEST(test_pq_empty) {
   pqcb *pq = pq_create(test_threaded);
   mu_should(pq_empty(pq));
   mu_should(pq_count(pq) == 0);
   mu_should(pq_destroy(pq));
}

MU_TEST(test_pq_access_empty) {
   pqcb *pq = pq_create(test_threaded);
   mu_should(pq_peek(pq) == NULL);
   mu_should(pq_get(pq) == NULL);
   mu_should(pq_destroy(pq));
}

MU_TEST(test_pq_add_first) {
   pqcb *pq = pq_create(test_threaded);
   pq_put(pq, 100, "100");
   mu_shouldnt(pq_empty(pq));
   mu_should(pq_count(pq) == 1);
   mu_shouldnt(pq_destroy(pq));
   char *str = (char *)pq_get(pq);
   assert(str);
   mu_should(strcmp(str, "100") == 0);
   mu_should(pq_empty(pq));
   mu_should(pq_count(pq) == 0);
   mu_should(pq_destroy(pq));
}

MU_TEST(test_pq_read_loop) {
   pqcb *pq = pq_create(true);
   pq_put(pq, 100, "100");
   pq_put(pq, 99, "99");
   pq_put(pq, 101, "101");
   char *str = NULL;
   int i = 0;
   while (str = pq_get(pq), str) {
      i += 1;
   }
   mu_should(i = 3);
   mu_should(pq_empty(pq));
   mu_should(pq_destroy(pq));
}

MU_TEST(test_pq_rand_volume) {
   pqcb *pq = pq_create(test_threaded);
   pq_put(pq, 1024, (void *)1024);
   pq_put(pq, 8888, (void *)8888);
   pq_put(pq, -3, (void *)-3);
   mu_shouldnt(pq_empty(pq));
   mu_should(pq_count(pq) == 3);
   for (int i = 0; i < 10000; i++) {
      long j = rand_between(0, 99999);
      pq_put(pq, j, (void *)j);
   }
   mu_shouldnt(pq_empty(pq));
   mu_should(pq_count(pq) == 10003);
   void *last_pri = pq_peek(pq);
   while (!pq_empty(pq)) {
      void *this_pri =  pq_get(pq);
      if ((long)this_pri > (long)last_pri) {
         mu_should((long)this_pri <= (long)last_pri);
      }
      last_pri = this_pri;
   }
   printf("\n");
   mu_should(pq_empty(pq));
   mu_should(pq_count(pq) == 0);
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

   test_threaded = false;
   printf("\n\npriority queue -- not threaded \n\n");
   MU_RUN_TEST(test_pq_create);
   MU_RUN_TEST(test_pq_empty);
   MU_RUN_TEST(test_pq_access_empty);
   MU_RUN_TEST(test_pq_add_first);
   MU_RUN_TEST(test_pq_read_loop);
   MU_RUN_TEST(test_pq_rand_volume);

   test_threaded = true;
   printf("\n\npriority queue -- threaded \n\n");
   MU_RUN_TEST(test_pq_create);
   MU_RUN_TEST(test_pq_empty);
   MU_RUN_TEST(test_pq_access_empty);
   MU_RUN_TEST(test_pq_add_first);
   MU_RUN_TEST(test_pq_read_loop);
   MU_RUN_TEST(test_pq_rand_volume);

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
