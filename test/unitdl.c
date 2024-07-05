/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../inc/misc.h"
#include "../inc/dl.h"
#include "../inc/str.h"


/*
 *
 * tests for:
 *
 * txblistd -- doubly linked list
 *
 */

/*
 * global state and constants. i want a better way to create an
 * instance of the list, but for now this is sufficient.
 */

/* change the following to control if using pthread calls */

const bool test_threaded = false;

/* for testing, the payload is a string, so payload compare is just a
 * wrapper around strcmp. */

int
payload_compare(void *s1, void *s2) {
   return strcmp(s1, s2);
}

/* for testing, just free any single block of allocated memory. */

void
payload_free(void *p) {
   free(p);
}

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
 * the doubly linked list tests:
 *
 * there's a lot of white box testing in here peeking at the structore of
 * list items and such from early testing. that may not be proper unit
 * testing practice, but i'm leaving the assertions in. they don't hurt
 * anything.
 */

MU_TEST(test_dl_id_create) {
   dlcb_t *dl = dl_create_by_id(
                   test_threaded,
                   NULL
                );
   mu_should(dl);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);
   mu_should(dl_destroy(dl));
}

MU_TEST(test_dl_key_create) {
   dlcb_t *dl = dl_create_by_key(
                   test_threaded,
                   payload_compare,
                   payload_free
                );
   mu_should(dl);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);
   mu_should(dl_destroy(dl));
}

MU_TEST(test_dl_id_add) {
   dlcb_t *dl = dl_create_by_id(
                   test_threaded,
                   NULL
                );
   mu_should(dl);

   /* add a single entry to empty list, then remove it. */
   mu_should(dl_add(dl, 1, "1234"));
   mu_shouldnt(dl_empty(dl));
   mu_should(dl_count(dl) == 1);
   mu_should(dl_delete_all(dl) == 1);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);

   /* add two unique entries, then remove them. */
   mu_should(dl_add(dl, 1, "first"));
   mu_should(dl_add(dl, 2, "second"));
   mu_should(dl_count(dl) == 2);
   mu_should(dl_delete_all(dl) == 2);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);

   /* it takes more than two entries to mess with linking. */
   mu_should(dl_add(dl, 1, "first"));
   mu_should(dl_add(dl, 4, "fourth, added second"));
   mu_should(dl_add(dl, 2, "second, added third"));
   mu_should(dl_add(dl, 3, "third, added fourth"));
   mu_should(dl_count(dl) == 4);
   /* now insert at front and then at back, knowing the ordering as we did above. */
   mu_should(dl_add(dl, -1, "zeroeth, added fifth"));
   mu_should(dl_add(dl, 9, "sixth, added sixth"));
   /* we'll confirm ordering in another set of tests. */
   mu_should(dl_count(dl) == 6);

   /* and now empty the list and we're done. */
   mu_should(dl_delete_all(dl) == 6);
   mu_should(dl_empty(dl));

   mu_should(dl_destroy(dl));
}

MU_TEST(test_dl_key_add) {
   dlcb_t *dl = dl_create_by_key(
                   test_threaded,
                   payload_compare,
                   NULL
                );
   mu_should(dl);
   mu_should(dl);
   mu_should(dl_add(dl, 1, "1234"));
   mu_shouldnt(dl_empty(dl));
   mu_should(dl_count(dl) == 1);
   mu_should(dl_delete_all(dl) == 1);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);
   mu_should(dl_destroy(dl));
}

MU_TEST(test_list_create) {
   dlentry_t *n = NULL;

   /* n = dl_new(&by_id, (void *)147); */
   /* mu_assert_int_eq(true, n->fwd == NULL); */
   /* mu_assert_int_eq(true, n->bwd == NULL); */
   /* mu_assert_int_eq(147, n->id); */
   /* dl_free(&by_id, &n); */
   /* mu_assert_int_eq(true, n == NULL); */

   /* char *s = strdup("this is a sentence"); */
   /* n = dl_new(&by_payload, s); */
   /* mu_assert_int_eq(true, n->fwd == NULL); */
   /* mu_assert_int_eq(true, n->bwd == NULL); */
   /* mu_assert_string_eq("this is a sentence", n->payload); */
   /* mu_assert_int_eq(strlen(s), strlen(n->payload)); */
   /* dl_free(&by_payload, &n); */
   /* free(s); */
   /* mu_assert_int_eq(true, n == NULL); */

}


/* add one item and confirm that count_items finds it */

MU_TEST(test_list_count) {
   dlentry_t *n = NULL;
   bool r;

   /* n = dl_new(&by_id, (void *)15); */
   /* mu_assert_int_eq(0, by_id.count); */
   /* mu_assert_int_eq(0, dl_count(&by_id)); */
   /* r = dl_add(&by_id, n); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(1, by_id.count); */
   /* mu_assert_int_eq(1, dl_count(&by_id)); */

   /* n = dl_new(&by_payload, "qwerty"); */
   /* mu_assert_int_eq(0, by_payload.count); */
   /* mu_assert_int_eq(0, dl_count(&by_payload)); */
   /* r = dl_add(&by_payload, n); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(1, by_payload.count); */
   /* mu_assert_int_eq(1, dl_count(&by_payload)); */

}


/* in a non-empty list, test inserting items at the head and tail
   of the list */

MU_TEST(test_list_insert) {
   dlentry_t *n = NULL;
   dlentry_t *head = NULL;
   dlentry_t *tail = NULL;
   bool r;

   /* n = dl_new(&by_id, (void *)15); */
   /* r = dl_add(&by_id, n); */
   /* head = dl_new(&by_id, (void *)10); */
   /* r = dl_add(&by_id, head); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(2, by_id.count); */
   /* mu_assert_int_eq(2, dl_count(&by_id)); */
   /* mu_assert_int_eq(true, n->bwd == head); */
   /* mu_assert_int_eq(true, n->fwd == NULL); */
   /* mu_assert_int_eq(true, head->bwd == NULL); */
   /* mu_assert_int_eq(true, head->fwd == n); */
   /* tail = dl_new(&by_id, (void *)20); */
   /* r = dl_add(&by_id, tail); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(3, by_id.count); */
   /* mu_assert_int_eq(3, dl_count(&by_id)); */
   /* mu_assert_int_eq(true, n->bwd == head); */
   /* mu_assert_int_eq(true, n->fwd == tail); */
   /* mu_assert_int_eq(true, tail->fwd == NULL); */
   /* mu_assert_int_eq(true, tail->bwd == n); */

   /* n = dl_new(&by_payload, "bbbb"); */
   /* r = dl_add(&by_payload, n); */
   /* head = dl_new(&by_payload, "aaaa"); */
   /* r = dl_add(&by_payload, head); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(2, by_payload.count); */
   /* mu_assert_int_eq(2, dl_count(&by_payload)); */
   /* mu_assert_int_eq(true, n->bwd == head); */
   /* mu_assert_int_eq(true, n->fwd == NULL); */
   /* mu_assert_int_eq(true, head->bwd == NULL); */
   /* mu_assert_int_eq(true, head->fwd == n); */
   /* tail = dl_new(&by_payload, "zzzz"); */
   /* r = dl_add(&by_payload, tail); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(3, by_payload.count); */
   /* mu_assert_int_eq(3, dl_count(&by_payload)); */
   /* mu_assert_int_eq(true, n->bwd == head); */
   /* mu_assert_int_eq(true, n->fwd == tail); */
   /* mu_assert_int_eq(true, tail->fwd == NULL); */
   /* mu_assert_int_eq(true, tail->bwd == n); */

}


/* confirm that attempting to add an duplicate item id is not allowed */

MU_TEST(test_list_duplicates) {
   dlentry_t *n = NULL;
   dlentry_t *d = NULL;
   bool r;

   /* n = dl_new(&by_id, (void *)10); */
   /* d = dl_new(&by_id, (void *)10); */
   /* r = dl_add(&by_id, n); */
   /* mu_assert_int_eq(true, r); */
   /* r = dl_add(&by_id, d); */
   /* mu_assert_int_eq(false, r); */
   /* mu_assert_int_eq(1, by_id.count); */
   /* mu_assert_int_eq(1, dl_count(&by_id)); */
   /* dl_free(&by_id, &d); */

   /* n = dl_new(&by_payload, "asdf"); */
   /* d = dl_new(&by_payload, "asdf"); */
   /* r = dl_add(&by_payload, n); */
   /* mu_assert_int_eq(true, r); */
   /* r = dl_add(&by_payload, d); */
   /* mu_assert_int_eq(false, r); */
   /* mu_assert_int_eq(1, by_payload.count); */
   /* mu_assert_int_eq(1, dl_count(&by_payload)); */
   /* dl_free(&by_payload, &d); */

}


/* try to insert between existing items */

MU_TEST(test_list_chaining) {
   dlentry_t *head = NULL;
   dlentry_t *tail = NULL;
   dlentry_t *inside = NULL;
   bool r;

   /* head = dl_new(&by_id, (void *)10); */
   /* tail = dl_new(&by_id, (void *)90); */
   /* r = dl_add(&by_id, head); */
   /* mu_assert_int_eq(true, r); */
   /* r = dl_add(&by_id, tail); */
   /* mu_assert_int_eq(true, r); */
   /* inside = dl_new(&by_id, (void *)20); */
   /* r = dl_add(&by_id, inside); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(3, by_id.count); */
   /* mu_assert_int_eq(3, dl_count(&by_id)); */

   /* head = dl_new(&by_payload, "head"); */
   /* tail = dl_new(&by_payload, "tail"); */
   /* r = dl_add(&by_payload, head); */
   /* mu_assert_int_eq(true, r); */
   /* r = dl_add(&by_payload, tail); */
   /* mu_assert_int_eq(true, r); */
   /* inside = dl_new(&by_payload, "middle"); */
   /* r = dl_add(&by_payload, inside); */
   /* mu_assert_int_eq(true, r); */
   /* mu_assert_int_eq(3, by_payload.count); */
   /* mu_assert_int_eq(3, dl_count(&by_payload)); */
}


/* volume tests, sometimes you just gotta throw spaghetti at the
 * wall */

MU_TEST(test_list_many_asc) {
   dlentry_t *n = NULL;
   bool r;
   char *digits[100];

   for (long i = 0; i < 100; i++) {
      /* n = dl_new(&by_id, (void *)i); */
      /* r = dl_add(&by_id, n); */
      /* if (!r) { */
      /*    mu_assert_int_eq(true, r); */
      /* } */
   }
   /* mu_assert_int_eq(100, by_id.count); */
   /* mu_assert_int_eq(100, dl_count(&by_id)); */

   for (long i = 0; i < 100; i++) {
      digits[i] = calloc(16, 1);
      snprintf(digits[i], 15, "%ld", i);
      /* n = dl_new(&by_payload, digits[i]); */
      /* r = dl_add(&by_payload, n); */
      /* if (!r) { */
      /*    mu_assert_int_eq(true, r); */
      /* } */
   }
   /* mu_assert_int_eq(100, by_payload.count); */
   /* mu_assert_int_eq(100, dl_count(&by_payload)); */
   for (long i = 0; i < 100; i++) {
      free(digits[i]);
   }
}

MU_TEST(test_list_many_dsc) {
   for (long i = 0; i < 100; i++) {
      /* dlentry_t *n = dl_new(&by_id, (void *)(100 - i)); */
      /* bool r = dl_add(&by_id, n); */
      /* if (r == false) { */
      /*    mu_assert_int_eq(true, r); */
      /* } */
   }
   /* mu_assert_int_eq(100, by_id.count); */
   /* mu_assert_int_eq(100, dl_count(&by_id)); */
}

MU_TEST(test_list_many_random) {

   /* srand() is reseeded every test_setup */
   /* mu_assert_int_eq(0, dl_count(&by_id)); */

   /* int inserted = 0; */

   /* dlentry_t *n; */
   /* while (dl_count(&by_id) < 1000) { */
   /*    n = dl_new(&by_id, (void *)(long)rand_between(0, 10000)); */
   /*    if (dl_add(&by_id, n)) { */
   /*       inserted += 1; */
   /*    } else { */
   /*       dl_free(&by_id, &n); /\* item was never linked *\/ */
   /*    } */
   /* } */
   /* mu_assert_int_eq(inserted, dl_count(&by_id)); */
}

/* test iterations. */

MU_TEST(test_list_iteration) {

   /* dlentry_t *first = NULL; */
   /* dlentry_t *last = NULL; */
   /* dlentry_t *middle = NULL; */
   /* dlentry_t *n = NULL; */

   /* /\* load some items *\/ */
   /* for (long i = 0; i < 10; i++) { */
   /*    n = dl_new(&by_id, (void *)i); */
   /*    if (i == 0) { */
   /*       first = n; */
   /*    } */
   /*    if (n->id == 5) { */
   /*       middle = n; */
   /*    } */
   /*    dl_add(&by_id, n); */
   /*    last = n; */
   /* } */

   /* dlentry_t *curr = NULL; */

   /* /\* iterate from front *\/ */
   /* n = dl_next(&by_id, &curr); */
   /* mu_assert_int_eq(true, n == first); */
   /* int count = 0; */
   /* mu_assert_int_eq(true, n == curr); */
   /* mu_assert_int_eq(true, n->fwd != NULL); */
   /* mu_assert_int_eq(true, n->bwd == NULL); */
   /* first = n; */
   /* while (n) { */
   /*    count += 1; */
   /*    if (n->fwd == NULL) { */
   /*       last = n; */
   /*    } */
   /*    n = dl_next(&by_id, &curr); */
   /* } */
   /* mu_assert_int_eq(count, dl_count(&by_id)); */

   /* /\* iterate from back *\/ */
   /* curr = NULL; */
   /* n = dl_prev(&by_id, &curr); */
   /* mu_assert_int_eq(true, n == last); */
   /* count = 0; */
   /* while (n) { */
   /*    count += 1; */
   /*    n = dl_prev(&by_id, &curr); */
   /* } */
   /* mu_assert_int_eq(count, dl_count(&by_id)); */

   /* /\* navigate from middle *\/ */
   /* curr = middle; */
   /* n = dl_prev(&by_id, &curr); */
   /* mu_assert_int_eq(n->id, middle->id - 1); */
   /* curr = middle; */
   /* n = dl_next(&by_id, &curr); */
   /* mu_assert_int_eq(n->id, middle->id + 1); */
}

/* test finding items. */

MU_TEST(test_list_find) {
   dlentry_t *middle = NULL;
   dlentry_t *n = NULL;

   /* /\* load some items *\/ */
   /* for (long i = 0; i < 100; i += 10) { */
   /*    n = dl_new(&by_id, (void *)i); */
   /*    if (n->id == 50) { */
   /*       middle = n; */
   /*    } */
   /*    dl_add(&by_id, n); */
   /* } */

   /* /\* can we find the middle? *\/ */
   /* n = dl_find(&by_id, (void *)middle->id); */
   /* mu_assert_int_eq(true, middle == n); */

   /* /\* other items *\/ */
   /* mu_assert_int_eq(true, NULL != dl_find(&by_id, (void *)10)); */
   /* mu_assert_int_eq(true, NULL != dl_find(&by_id, (void *)90)); */
   /* mu_assert_int_eq(true, NULL == dl_find(&by_id, (void *)15)); */
   /* mu_assert_int_eq(true, NULL == dl_find(&by_id, (void *)-1)); */
   /* mu_assert_int_eq(true, NULL == dl_find(&by_id, (void *)101)); */
}

/* and of course removal */

MU_TEST(test_list_remove) {
   dlentry_t *first = NULL;
   dlentry_t *last = NULL;
   dlentry_t *middle = NULL;
   dlentry_t *n = NULL;
   dlentry_t *removed = NULL;
   char *digits[100];
   long r;

   /* /\* load up 100 items *\/ */

   /* for (long i = 0; i < 100; i++) { */
   /*    digits[i] = calloc(16, 1); */
   /*    snprintf(digits[i], 15, "%ld", i); */
   /*    n = dl_new(&by_payload, digits[i]); */
   /*    if (i == 0) { */
   /*       first = n; */
   /*    } else if (i == 50) { */
   /*       middle = n; */
   /*    } else if (i == 99) { */
   /*       last = n; */
   /*    } */
   /*    r = dl_add(&by_payload, n); */
   /*    if (!r) { */
   /*       mu_assert_int_eq(true, r); */
   /*    } */
   /* } */
   /* mu_assert_int_eq(100, by_payload.count); */
   /* mu_assert_int_eq(100, dl_count(&by_payload)); */

   /* /\* try to remove a item that isn't there *\/ */
   /* removed = dl_remove(&by_payload, "123456"); */
   /* mu_assert_int_eq((long)NULL, (long)removed); */
   /* mu_assert_int_eq(100, dl_count(&by_payload)); */

   /* /\* try to remove the first item *\/ */
   /* char *s = strdup(first->payload); */
   /* removed = dl_remove(&by_payload, s); */
   /* mu_assert_int_eq(false, removed == NULL); */
   /* mu_assert_int_eq(true, removed == first); */
   /* mu_assert_string_eq(s, removed->payload); */
   /* mu_assert_int_eq(99, dl_count(&by_payload)); */
   /* dl_free(&by_payload, &removed); */
   /* mu_assert_int_eq(true, removed == NULL); */
   /* free(s); */

   /* /\* try to remove the last item *\/ */
   /* s = strdup(last->payload); */
   /* removed = dl_remove(&by_payload, s); */
   /* mu_assert_int_eq(false, removed == NULL); */
   /* mu_assert_int_eq(true, removed == last); */
   /* mu_assert_string_eq(s, removed->payload); */
   /* mu_assert_int_eq(98, dl_count(&by_payload)); */
   /* dl_free(&by_payload, &removed); */
   /* mu_assert_int_eq(true, removed == NULL); */
   /* free(s); */

   /* /\* and now a item in the middle *\/ */
   /* s = strdup(middle->payload); */
   /* removed = dl_remove(&by_payload, s); */
   /* mu_assert_int_eq(false, removed == NULL); */
   /* mu_assert_int_eq(true, removed == middle); */
   /* mu_assert_string_eq(s, removed->payload); */
   /* mu_assert_int_eq(97, dl_count(&by_payload)); */
   /* dl_free(&by_payload, &removed); */
   /* mu_assert_int_eq(true, removed == NULL); */

   /* /\* and just to be sure it's really gone *\/ */
   /* removed = dl_remove(&by_payload, s); */
   /* mu_assert_int_eq(true, removed == NULL); */

   /* free(s); */

   /* for (long i = 0; i < 100; i++) { */
   /*    free(digits[i]); */
   /* } */
}

/* test freeing all the items in a populated list. */

MU_TEST(test_list_free) {
   /* dlentry_t *n = NULL; */
   /* char *digits[100]; */
   /* long r; */

   /* /\* load up 100 items *\/ */

   /* for (long i = 0; i < 100; i++) { */
   /*    digits[i] = calloc(16, 1); */
   /*    snprintf(digits[i], 15, "%ld", i); */
   /*    n = dl_new(&by_payload, digits[i]); */
   /*    r = dl_add(&by_payload, n); */
   /*    if (!r) { */
   /*       mu_assert_int_eq(true, r); */
   /*    } */
   /* } */
   /* mu_assert_int_eq(100, by_payload.count); */
   /* mu_assert_int_eq(100, dl_count(&by_payload)); */

   /* dl_free_all(&by_payload); */

   /* mu_assert_int_eq(0, by_payload.count); */
   /* mu_assert_int_eq(0, dl_count(&by_payload)); */

   /* for (long i = 0; i < 100; i++) { */
   /*    free(digits[i]); */
   /* } */
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

   printf("\n\ndoubly linked list\n\n");
   MU_RUN_TEST(test_dl_id_create);
   MU_RUN_TEST(test_dl_key_create);

   MU_RUN_TEST(test_dl_id_add);
   MU_RUN_TEST(test_dl_key_add);


   MU_RUN_TEST(test_list_create);
   MU_RUN_TEST(test_list_count);
   MU_RUN_TEST(test_list_insert);
   MU_RUN_TEST(test_list_duplicates);
   MU_RUN_TEST(test_list_chaining);
   MU_RUN_TEST(test_list_many_asc);
   MU_RUN_TEST(test_list_many_dsc);
   MU_RUN_TEST(test_list_many_random);
   MU_RUN_TEST(test_list_iteration);
   MU_RUN_TEST(test_list_find);
   MU_RUN_TEST(test_list_remove);
   MU_RUN_TEST(test_list_free);

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
