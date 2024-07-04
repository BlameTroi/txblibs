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

/* two lists, one by id one by payload */

ldcb_t list_by_id;
ldcb_t list_by_payload;

/* for testing, the payload is a string, so payload compare is just a
   wrapper around strcmp. */

long
payload_compare(void *s1, void *s2) {
   return strcmp(s1, s2);
}

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

/* each test will have id and payload lists available to work with. */

void
test_setup(void) {

   /* set up for using ids */
   memset(&list_by_id, 0, sizeof(list_by_id));
   list_by_id.dynamic_payload = false;
   list_by_id.has_payload = false;
   list_by_id.use_id = true;

   /* set up for using static string payloads (literals) */
   memset(&list_by_payload, 0, sizeof(list_by_payload));
   list_by_payload.dynamic_payload = false;
   list_by_payload.has_payload = true;
   list_by_payload.use_id = false;
   list_by_payload.compare_payload = payload_compare;

   /* let's use a different seed than 1, but not time() because i want
      repeatable tests. */
   srand(RAND_SEED);
}


/* after each test, release the items and lists properly. */

void
test_teardown(void) {

   lditem_t *p = list_by_id.first;
   lditem_t *n = NULL;
   while (p) {
      n = p->fwd;
      ld_free(&list_by_id, &p);
      p = n;
   }
   memset(&list_by_id, 0xfe, sizeof(list_by_id));

   p = list_by_payload.first;
   n = NULL;
   while (p) {
      n = p->fwd;
      ld_free(&list_by_payload, &p);
      p = n;
   }
   memset(&list_by_payload, 0xfe, sizeof(list_by_payload));
}

/*
 * the doubly linked list tests:
 *
 * there's a lot of white box testing in here peeking at the structore of
 * list items and such from early testing. that may not be proper unit
 * testing practice, but i'm leaving the assertions in. they don't hurt
 * anything.
 */


/* build a item and check out its state, then free it. */

MU_TEST(test_list_create) {
   lditem_t *n = NULL;

   n = ld_new(&list_by_id, (void *)147);
   mu_assert_int_eq(true, n->fwd == NULL);
   mu_assert_int_eq(true, n->bwd == NULL);
   mu_assert_int_eq(147, n->id);
   ld_free(&list_by_id, &n);
   mu_assert_int_eq(true, n == NULL);

   char *s = strdup("this is a sentence");
   n = ld_new(&list_by_payload, s);
   mu_assert_int_eq(true, n->fwd == NULL);
   mu_assert_int_eq(true, n->bwd == NULL);
   mu_assert_string_eq("this is a sentence", n->payload);
   mu_assert_int_eq(strlen(s), strlen(n->payload));
   ld_free(&list_by_payload, &n);
   free(s);
   mu_assert_int_eq(true, n == NULL);

}


/* add one item and confirm that count_items finds it */

MU_TEST(test_list_count) {
   lditem_t *n = NULL;
   bool r;

   n = ld_new(&list_by_id, (void *)15);
   mu_assert_int_eq(0, list_by_id.count);
   mu_assert_int_eq(0, ld_count(&list_by_id));
   r = ld_add(&list_by_id, n);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(1, list_by_id.count);
   mu_assert_int_eq(1, ld_count(&list_by_id));

   n = ld_new(&list_by_payload, "qwerty");
   mu_assert_int_eq(0, list_by_payload.count);
   mu_assert_int_eq(0, ld_count(&list_by_payload));
   r = ld_add(&list_by_payload, n);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(1, list_by_payload.count);
   mu_assert_int_eq(1, ld_count(&list_by_payload));

}


/* in a non-empty list, test inserting items at the head and tail
   of the list */

MU_TEST(test_list_insert) {
   lditem_t *n = NULL;
   lditem_t *head = NULL;
   lditem_t *tail = NULL;
   bool r;

   n = ld_new(&list_by_id, (void *)15);
   r = ld_add(&list_by_id, n);
   head = ld_new(&list_by_id, (void *)10);
   r = ld_add(&list_by_id, head);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(2, list_by_id.count);
   mu_assert_int_eq(2, ld_count(&list_by_id));
   mu_assert_int_eq(true, n->bwd == head);
   mu_assert_int_eq(true, n->fwd == NULL);
   mu_assert_int_eq(true, head->bwd == NULL);
   mu_assert_int_eq(true, head->fwd == n);
   tail = ld_new(&list_by_id, (void *)20);
   r = ld_add(&list_by_id, tail);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_id.count);
   mu_assert_int_eq(3, ld_count(&list_by_id));
   mu_assert_int_eq(true, n->bwd == head);
   mu_assert_int_eq(true, n->fwd == tail);
   mu_assert_int_eq(true, tail->fwd == NULL);
   mu_assert_int_eq(true, tail->bwd == n);

   n = ld_new(&list_by_payload, "bbbb");
   r = ld_add(&list_by_payload, n);
   head = ld_new(&list_by_payload, "aaaa");
   r = ld_add(&list_by_payload, head);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(2, list_by_payload.count);
   mu_assert_int_eq(2, ld_count(&list_by_payload));
   mu_assert_int_eq(true, n->bwd == head);
   mu_assert_int_eq(true, n->fwd == NULL);
   mu_assert_int_eq(true, head->bwd == NULL);
   mu_assert_int_eq(true, head->fwd == n);
   tail = ld_new(&list_by_payload, "zzzz");
   r = ld_add(&list_by_payload, tail);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_payload.count);
   mu_assert_int_eq(3, ld_count(&list_by_payload));
   mu_assert_int_eq(true, n->bwd == head);
   mu_assert_int_eq(true, n->fwd == tail);
   mu_assert_int_eq(true, tail->fwd == NULL);
   mu_assert_int_eq(true, tail->bwd == n);

}


/* confirm that attempting to add an duplicate item id is not allowed */

MU_TEST(test_list_duplicates) {
   lditem_t *n = NULL;
   lditem_t *d = NULL;
   bool r;

   n = ld_new(&list_by_id, (void *)10);
   d = ld_new(&list_by_id, (void *)10);
   r = ld_add(&list_by_id, n);
   mu_assert_int_eq(true, r);
   r = ld_add(&list_by_id, d);
   mu_assert_int_eq(false, r);
   mu_assert_int_eq(1, list_by_id.count);
   mu_assert_int_eq(1, ld_count(&list_by_id));
   ld_free(&list_by_id, &d);

   n = ld_new(&list_by_payload, "asdf");
   d = ld_new(&list_by_payload, "asdf");
   r = ld_add(&list_by_payload, n);
   mu_assert_int_eq(true, r);
   r = ld_add(&list_by_payload, d);
   mu_assert_int_eq(false, r);
   mu_assert_int_eq(1, list_by_payload.count);
   mu_assert_int_eq(1, ld_count(&list_by_payload));
   ld_free(&list_by_payload, &d);

}


/* try to insert between existing items */

MU_TEST(test_list_chaining) {
   lditem_t *head = NULL;
   lditem_t *tail = NULL;
   lditem_t *inside = NULL;
   bool r;

   head = ld_new(&list_by_id, (void *)10);
   tail = ld_new(&list_by_id, (void *)90);
   r = ld_add(&list_by_id, head);
   mu_assert_int_eq(true, r);
   r = ld_add(&list_by_id, tail);
   mu_assert_int_eq(true, r);
   inside = ld_new(&list_by_id, (void *)20);
   r = ld_add(&list_by_id, inside);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_id.count);
   mu_assert_int_eq(3, ld_count(&list_by_id));

   head = ld_new(&list_by_payload, "head");
   tail = ld_new(&list_by_payload, "tail");
   r = ld_add(&list_by_payload, head);
   mu_assert_int_eq(true, r);
   r = ld_add(&list_by_payload, tail);
   mu_assert_int_eq(true, r);
   inside = ld_new(&list_by_payload, "middle");
   r = ld_add(&list_by_payload, inside);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_payload.count);
   mu_assert_int_eq(3, ld_count(&list_by_payload));
}


/* volume tests, sometimes you just gotta throw spaghetti at the wall */

MU_TEST(test_list_many_asc) {
   lditem_t *n = NULL;
   bool r;
   char *digits[100];

   for (long i = 0; i < 100; i++) {
      n = ld_new(&list_by_id, (void *)i);
      r = ld_add(&list_by_id, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_id.count);
   mu_assert_int_eq(100, ld_count(&list_by_id));

   for (long i = 0; i < 100; i++) {
      digits[i] = calloc(16, 1);
      snprintf(digits[i], 15, "%ld", i);
      n = ld_new(&list_by_payload, digits[i]);
      r = ld_add(&list_by_payload, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_payload.count);
   mu_assert_int_eq(100, ld_count(&list_by_payload));
   for (long i = 0; i < 100; i++) {
      free(digits[i]);
   }
}

MU_TEST(test_list_many_dsc) {
   for (long i = 0; i < 100; i++) {
      lditem_t *n = ld_new(&list_by_id, (void *)(100 - i));
      bool r = ld_add(&list_by_id, n);
      if (r == false) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_id.count);
   mu_assert_int_eq(100, ld_count(&list_by_id));
}

MU_TEST(test_list_many_random) {

   /* srand() is reseeded every test_setup */
   mu_assert_int_eq(0, ld_count(&list_by_id));

   int inserted = 0;

   lditem_t *n;
   while (ld_count(&list_by_id) < 1000) {
      n = ld_new(&list_by_id, (void *)(long)rand_between(0, 10000));
      if (ld_add(&list_by_id, n)) {
         inserted += 1;
      } else {
         ld_free(&list_by_id, &n); /* item was never linked */
      }
   }
   mu_assert_int_eq(inserted, ld_count(&list_by_id));
}

/* test iterations. */

MU_TEST(test_list_iteration) {

   lditem_t *first = NULL;
   lditem_t *last = NULL;
   lditem_t *middle = NULL;
   lditem_t *n = NULL;

   /* load some items */
   for (long i = 0; i < 10; i++) {
      n = ld_new(&list_by_id, (void *)i);
      if (i == 0) {
         first = n;
      }
      if (n->id == 5) {
         middle = n;
      }
      ld_add(&list_by_id, n);
      last = n;
   }

   lditem_t *curr = NULL;

   /* iterate from front */
   n = ld_next(&list_by_id, &curr);
   mu_assert_int_eq(true, n == first);
   int count = 0;
   mu_assert_int_eq(true, n == curr);
   mu_assert_int_eq(true, n->fwd != NULL);
   mu_assert_int_eq(true, n->bwd == NULL);
   first = n;
   while (n) {
      count += 1;
      if (n->fwd == NULL) {
         last = n;
      }
      n = ld_next(&list_by_id, &curr);
   }
   mu_assert_int_eq(count, ld_count(&list_by_id));

   /* iterate from back */
   curr = NULL;
   n = ld_prev(&list_by_id, &curr);
   mu_assert_int_eq(true, n == last);
   count = 0;
   while (n) {
      count += 1;
      n = ld_prev(&list_by_id, &curr);
   }
   mu_assert_int_eq(count, ld_count(&list_by_id));

   /* navigate from middle */
   curr = middle;
   n = ld_prev(&list_by_id, &curr);
   mu_assert_int_eq(n->id, middle->id - 1);
   curr = middle;
   n = ld_next(&list_by_id, &curr);
   mu_assert_int_eq(n->id, middle->id + 1);
}

/* test finding items. */

MU_TEST(test_list_find) {
   lditem_t *middle = NULL;
   lditem_t *n = NULL;

   /* load some items */
   for (long i = 0; i < 100; i += 10) {
      n = ld_new(&list_by_id, (void *)i);
      if (n->id == 50) {
         middle = n;
      }
      ld_add(&list_by_id, n);
   }

   /* can we find the middle? */
   n = ld_find(&list_by_id, (void *)middle->id);
   mu_assert_int_eq(true, middle == n);

   /* other items */
   mu_assert_int_eq(true, NULL != ld_find(&list_by_id, (void *)10));
   mu_assert_int_eq(true, NULL != ld_find(&list_by_id, (void *)90));
   mu_assert_int_eq(true, NULL == ld_find(&list_by_id, (void *)15));
   mu_assert_int_eq(true, NULL == ld_find(&list_by_id, (void *)-1));
   mu_assert_int_eq(true, NULL == ld_find(&list_by_id, (void *)101));
}

/* and of course removal */

MU_TEST(test_list_remove) {
   lditem_t *first = NULL;
   lditem_t *last = NULL;
   lditem_t *middle = NULL;
   lditem_t *n = NULL;
   lditem_t *removed = NULL;
   char *digits[100];
   long r;

   /* load up 100 items */

   for (long i = 0; i < 100; i++) {
      digits[i] = calloc(16, 1);
      snprintf(digits[i], 15, "%ld", i);
      n = ld_new(&list_by_payload, digits[i]);
      if (i == 0) {
         first = n;
      } else if (i == 50) {
         middle = n;
      } else if (i == 99) {
         last = n;
      }
      r = ld_add(&list_by_payload, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_payload.count);
   mu_assert_int_eq(100, ld_count(&list_by_payload));

   /* try to remove a item that isn't there */
   removed = ld_remove(&list_by_payload, "123456");
   mu_assert_int_eq((long)NULL, (long)removed);
   mu_assert_int_eq(100, ld_count(&list_by_payload));

   /* try to remove the first item */
   char *s = strdup(first->payload);
   removed = ld_remove(&list_by_payload, s);
   mu_assert_int_eq(false, removed == NULL);
   mu_assert_int_eq(true, removed == first);
   mu_assert_string_eq(s, removed->payload);
   mu_assert_int_eq(99, ld_count(&list_by_payload));
   ld_free(&list_by_payload, &removed);
   mu_assert_int_eq(true, removed == NULL);
   free(s);

   /* try to remove the last item */
   s = strdup(last->payload);
   removed = ld_remove(&list_by_payload, s);
   mu_assert_int_eq(false, removed == NULL);
   mu_assert_int_eq(true, removed == last);
   mu_assert_string_eq(s, removed->payload);
   mu_assert_int_eq(98, ld_count(&list_by_payload));
   ld_free(&list_by_payload, &removed);
   mu_assert_int_eq(true, removed == NULL);
   free(s);

   /* and now a item in the middle */
   s = strdup(middle->payload);
   removed = ld_remove(&list_by_payload, s);
   mu_assert_int_eq(false, removed == NULL);
   mu_assert_int_eq(true, removed == middle);
   mu_assert_string_eq(s, removed->payload);
   mu_assert_int_eq(97, ld_count(&list_by_payload));
   ld_free(&list_by_payload, &removed);
   mu_assert_int_eq(true, removed == NULL);

   /* and just to be sure it's really gone */
   removed = ld_remove(&list_by_payload, s);
   mu_assert_int_eq(true, removed == NULL);

   free(s);

   for (long i = 0; i < 100; i++) {
      free(digits[i]);
   }
}

/* test freeing all the items in a populated list. */

MU_TEST(test_list_free) {
   lditem_t *n = NULL;
   char *digits[100];
   long r;

   /* load up 100 items */

   for (long i = 0; i < 100; i++) {
      digits[i] = calloc(16, 1);
      snprintf(digits[i], 15, "%ld", i);
      n = ld_new(&list_by_payload, digits[i]);
      r = ld_add(&list_by_payload, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_payload.count);
   mu_assert_int_eq(100, ld_count(&list_by_payload));

   ld_free_all(&list_by_payload);

   mu_assert_int_eq(0, list_by_payload.count);
   mu_assert_int_eq(0, ld_count(&list_by_payload));

   for (long i = 0; i < 100; i++) {
      free(digits[i]);
   }
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
