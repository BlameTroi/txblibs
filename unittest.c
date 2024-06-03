/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#define TXBMISC_H_IMPLEMENTATION
#include "txbmisc.h"

#define TXBLISTD_H_IMPLEMENTATION
#include "txblistd.h"

#define TXBSTR_H_IMPLEMENTATION
#include "txbstr.h"


/*
 * this just keeps growing and growing, but i resist splitting it out
 * into separate files just yet.
 *
 * tests for:
 *
 * txblistd -- doubly linked list
 *
 * txbstr -- string and character in string functions
 *
 * txbmisc -- factor
 *            min/max
 *
 * txbpmute -- permutation and combination
 *
 * txbpat -- pattern matching
 */

/*
 * global state and constants. i want a better way to create an
 * instance of the list, but for now this is sufficient.
 */

/* two lists, one by id one by payload */

listd_control_t list_by_id;
listd_control_t list_by_payload;

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
   list_by_id.initialized = true;

   /* set up for using static string payloads (literals) */
   memset(&list_by_payload, 0, sizeof(list_by_payload));
   list_by_payload.dynamic_payload = false;
   list_by_payload.has_payload = true;
   list_by_payload.use_id = false;
   list_by_payload.compare_payload = payload_compare;
   list_by_payload.initialized = true;

   /* let's use a different seed than 1, but not time() because i want
      repeatable tests. */
   srand(RAND_SEED);
}


/* after each test, release the items and lists properly. */

void
test_teardown(void) {

   listd_item_t *p = list_by_id.head;
   listd_item_t *n = NULL;
   while (p) {
      n = p->next;
      free_item(&list_by_id, &p);
      p = n;
   }
   memset(&list_by_id, 0xfe, sizeof(list_by_id));
   list_by_id.initialized = false;

   p = list_by_payload.head;
   n = NULL;
   while (p) {
      n = p->next;
      free_item(&list_by_payload, &p);
      p = n;
   }
   memset(&list_by_payload, 0xfe, sizeof(list_by_payload));
   list_by_payload.initialized = false;
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
   listd_item_t *n = NULL;

   n = make_item(&list_by_id, (void *)147);
   mu_assert_int_eq(true, n->next == NULL);
   mu_assert_int_eq(true, n->prev == NULL);
   mu_assert_int_eq(147, n->id);
   free_item(&list_by_id, &n);
   mu_assert_int_eq(true, n == NULL);

   char *s = strdup("this is a sentence");
   n = make_item(&list_by_payload, s);
   mu_assert_int_eq(true, n->next == NULL);
   mu_assert_int_eq(true, n->prev == NULL);
   mu_assert_string_eq("this is a sentence", n->payload);
   mu_assert_int_eq(strlen(s), strlen(n->payload));
   free_item(&list_by_payload, &n);
   free(s);
   mu_assert_int_eq(true, n == NULL);

}


/* add one item and confirm that count_items finds it */

MU_TEST(test_list_count) {
   listd_item_t *n = NULL;
   bool r;

   n = make_item(&list_by_id, (void *)15);
   mu_assert_int_eq(0, list_by_id.count);
   mu_assert_int_eq(0, count_items(&list_by_id));
   r = add_item(&list_by_id, n);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(1, list_by_id.count);
   mu_assert_int_eq(1, count_items(&list_by_id));

   n = make_item(&list_by_payload, "qwerty");
   mu_assert_int_eq(0, list_by_payload.count);
   mu_assert_int_eq(0, count_items(&list_by_payload));
   r = add_item(&list_by_payload, n);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(1, list_by_payload.count);
   mu_assert_int_eq(1, count_items(&list_by_payload));

}


/* in a non-empty list, test inserting items at the head and tail
   of the list */

MU_TEST(test_list_insert) {
   listd_item_t *n = NULL;
   listd_item_t *head = NULL;
   listd_item_t *tail = NULL;
   bool r;

   n = make_item(&list_by_id, (void *)15);
   r = add_item(&list_by_id, n);
   head = make_item(&list_by_id, (void *)10);
   r = add_item(&list_by_id, head);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(2, list_by_id.count);
   mu_assert_int_eq(2, count_items(&list_by_id));
   mu_assert_int_eq(true, n->prev == head);
   mu_assert_int_eq(true, n->next == NULL);
   mu_assert_int_eq(true, head->prev == NULL);
   mu_assert_int_eq(true, head->next == n);
   tail = make_item(&list_by_id, (void *)20);
   r = add_item(&list_by_id, tail);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_id.count);
   mu_assert_int_eq(3, count_items(&list_by_id));
   mu_assert_int_eq(true, n->prev == head);
   mu_assert_int_eq(true, n->next == tail);
   mu_assert_int_eq(true, tail->next == NULL);
   mu_assert_int_eq(true, tail->prev == n);

   n = make_item(&list_by_payload, "bbbb");
   r = add_item(&list_by_payload, n);
   head = make_item(&list_by_payload, "aaaa");
   r = add_item(&list_by_payload, head);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(2, list_by_payload.count);
   mu_assert_int_eq(2, count_items(&list_by_payload));
   mu_assert_int_eq(true, n->prev == head);
   mu_assert_int_eq(true, n->next == NULL);
   mu_assert_int_eq(true, head->prev == NULL);
   mu_assert_int_eq(true, head->next == n);
   tail = make_item(&list_by_payload, "zzzz");
   r = add_item(&list_by_payload, tail);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_payload.count);
   mu_assert_int_eq(3, count_items(&list_by_payload));
   mu_assert_int_eq(true, n->prev == head);
   mu_assert_int_eq(true, n->next == tail);
   mu_assert_int_eq(true, tail->next == NULL);
   mu_assert_int_eq(true, tail->prev == n);

}


/* confirm that attempting to add an duplicate item id is not allowed */

MU_TEST(test_list_duplicates) {
   listd_item_t *n = NULL;
   listd_item_t *d = NULL;
   bool r;

   n = make_item(&list_by_id, (void *)10);
   d = make_item(&list_by_id, (void *)10);
   r = add_item(&list_by_id, n);
   mu_assert_int_eq(true, r);
   r = add_item(&list_by_id, d);
   mu_assert_int_eq(false, r);
   mu_assert_int_eq(1, list_by_id.count);
   mu_assert_int_eq(1, count_items(&list_by_id));
   free_item(&list_by_id, &d);

   n = make_item(&list_by_payload, "asdf");
   d = make_item(&list_by_payload, "asdf");
   r = add_item(&list_by_payload, n);
   mu_assert_int_eq(true, r);
   r = add_item(&list_by_payload, d);
   mu_assert_int_eq(false, r);
   mu_assert_int_eq(1, list_by_payload.count);
   mu_assert_int_eq(1, count_items(&list_by_payload));
   free_item(&list_by_payload, &d);

}


/* try to insert between existing items */

MU_TEST(test_list_chaining) {
   listd_item_t *head = NULL;
   listd_item_t *tail = NULL;
   listd_item_t *inside = NULL;
   bool r;

   head = make_item(&list_by_id, (void *)10);
   tail = make_item(&list_by_id, (void *)90);
   r = add_item(&list_by_id, head);
   mu_assert_int_eq(true, r);
   r = add_item(&list_by_id, tail);
   mu_assert_int_eq(true, r);
   inside = make_item(&list_by_id, (void *)20);
   r = add_item(&list_by_id, inside);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_id.count);
   mu_assert_int_eq(3, count_items(&list_by_id));

   head = make_item(&list_by_payload, "head");
   tail = make_item(&list_by_payload, "tail");
   r = add_item(&list_by_payload, head);
   mu_assert_int_eq(true, r);
   r = add_item(&list_by_payload, tail);
   mu_assert_int_eq(true, r);
   inside = make_item(&list_by_payload, "middle");
   r = add_item(&list_by_payload, inside);
   mu_assert_int_eq(true, r);
   mu_assert_int_eq(3, list_by_payload.count);
   mu_assert_int_eq(3, count_items(&list_by_payload));
}


/* volume tests, sometimes you just gotta throw spaghetti at the wall */

MU_TEST(test_list_many_asc) {
   listd_item_t *n = NULL;
   bool r;
   char *digits[100];

   for (long i = 0; i < 100; i++) {
      n = make_item(&list_by_id, (void *)i);
      r = add_item(&list_by_id, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_id.count);
   mu_assert_int_eq(100, count_items(&list_by_id));

   for (long i = 0; i < 100; i++) {
      digits[i] = calloc(16, 1);
      snprintf(digits[i], 15, "%ld", i);
      n = make_item(&list_by_payload, digits[i]);
      r = add_item(&list_by_payload, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_payload.count);
   mu_assert_int_eq(100, count_items(&list_by_payload));
   for (long i = 0; i < 100; i++) {
      free(digits[i]);
   }
}

MU_TEST(test_list_many_dsc) {
   for (long i = 0; i < 100; i++) {
      listd_item_t *n = make_item(&list_by_id, (void *)(100 - i));
      bool r = add_item(&list_by_id, n);
      if (r == false) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_id.count);
   mu_assert_int_eq(100, count_items(&list_by_id));
}

MU_TEST(test_list_many_random) {

   /* srand() is reseeded every test_setup */
   mu_assert_int_eq(0, count_items(&list_by_id));

   int inserted = 0;

   listd_item_t *n;
   while (count_items(&list_by_id) < 1000) {
      n = make_item(&list_by_id, (void *)(long)rand_between(0, 10000));
      if (add_item(&list_by_id, n)) {
         inserted += 1;
      } else {
         free_item(&list_by_id, &n); /* item was never linked */
      }
   }
   mu_assert_int_eq(inserted, count_items(&list_by_id));
}

/* test iterations. */

MU_TEST(test_list_iteration) {

   listd_item_t *first = NULL;
   listd_item_t *last = NULL;
   listd_item_t *middle = NULL;
   listd_item_t *n = NULL;

   /* load some items */
   for (long i = 0; i < 10; i++) {
      n = make_item(&list_by_id, (void *)i);
      if (i == 0) {
         first = n;
      }
      if (n->id == 5) {
         middle = n;
      }
      add_item(&list_by_id, n);
      last = n;
   }

   listd_item_t *curr = NULL;

   /* iterate from front */
   n = next_item(&list_by_id, &curr);
   mu_assert_int_eq(true, n == first);
   int count = 0;
   mu_assert_int_eq(true, n == curr);
   mu_assert_int_eq(true, n->next != NULL);
   mu_assert_int_eq(true, n->prev == NULL);
   first = n;
   while (n) {
      count += 1;
      if (n->next == NULL) {
         last = n;
      }
      n = next_item(&list_by_id, &curr);
   }
   mu_assert_int_eq(count, count_items(&list_by_id));

   /* iterate from back */
   curr = NULL;
   n = prev_item(&list_by_id, &curr);
   mu_assert_int_eq(true, n == last);
   count = 0;
   while (n) {
      count += 1;
      n = prev_item(&list_by_id, &curr);
   }
   mu_assert_int_eq(count, count_items(&list_by_id));

   /* navigate from middle */
   curr = middle;
   n = prev_item(&list_by_id, &curr);
   mu_assert_int_eq(n->id, middle->id - 1);
   curr = middle;
   n = next_item(&list_by_id, &curr);
   mu_assert_int_eq(n->id, middle->id + 1);
}

/* test finding items. */

MU_TEST(test_list_find) {
   listd_item_t *middle = NULL;
   listd_item_t *n = NULL;

   /* load some items */
   for (long i = 0; i < 100; i += 10) {
      n = make_item(&list_by_id, (void *)i);
      if (n->id == 50) {
         middle = n;
      }
      add_item(&list_by_id, n);
   }

   /* can we find the middle? */
   n = find_item(&list_by_id, (void *)middle->id);
   mu_assert_int_eq(true, middle == n);

   /* other items */
   mu_assert_int_eq(true, NULL != find_item(&list_by_id, (void *)10));
   mu_assert_int_eq(true, NULL != find_item(&list_by_id, (void *)90));
   mu_assert_int_eq(true, NULL == find_item(&list_by_id, (void *)15));
   mu_assert_int_eq(true, NULL == find_item(&list_by_id, (void *)-1));
   mu_assert_int_eq(true, NULL == find_item(&list_by_id, (void *)101));
}

/* and of course removal */

MU_TEST(test_list_remove) {
   listd_item_t *first = NULL;
   listd_item_t *last = NULL;
   listd_item_t *middle = NULL;
   listd_item_t *n = NULL;
   listd_item_t *removed = NULL;
   char *digits[100];
   long r;

   /* load up 100 items */

   for (long i = 0; i < 100; i++) {
      digits[i] = calloc(16, 1);
      snprintf(digits[i], 15, "%ld", i);
      n = make_item(&list_by_payload, digits[i]);
      if (i == 0) {
         first = n;
      } else if (i == 50) {
         middle = n;
      } else if (i == 99) {
         last = n;
      }
      r = add_item(&list_by_payload, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_payload.count);
   mu_assert_int_eq(100, count_items(&list_by_payload));

   /* try to remove a item that isn't there */
   removed = remove_item(&list_by_payload, "123456");
   mu_assert_int_eq((long)NULL, (long)removed);
   mu_assert_int_eq(100, count_items(&list_by_payload));

   /* try to remove the first item */
   char *s = strdup(first->payload);
   removed = remove_item(&list_by_payload, s);
   mu_assert_int_eq(false, removed == NULL);
   mu_assert_int_eq(true, removed == first);
   mu_assert_string_eq(s, removed->payload);
   mu_assert_int_eq(99, count_items(&list_by_payload));
   free_item(&list_by_payload, &removed);
   mu_assert_int_eq(true, removed == NULL);
   free(s);

   /* try to remove the last item */
   s = strdup(last->payload);
   removed = remove_item(&list_by_payload, s);
   mu_assert_int_eq(false, removed == NULL);
   mu_assert_int_eq(true, removed == last);
   mu_assert_string_eq(s, removed->payload);
   mu_assert_int_eq(98, count_items(&list_by_payload));
   free_item(&list_by_payload, &removed);
   mu_assert_int_eq(true, removed == NULL);
   free(s);

   /* and now a item in the middle */
   s = strdup(middle->payload);
   removed = remove_item(&list_by_payload, s);
   mu_assert_int_eq(false, removed == NULL);
   mu_assert_int_eq(true, removed == middle);
   mu_assert_string_eq(s, removed->payload);
   mu_assert_int_eq(97, count_items(&list_by_payload));
   free_item(&list_by_payload, &removed);
   mu_assert_int_eq(true, removed == NULL);

   /* and just to be sure it's really gone */
   removed = remove_item(&list_by_payload, s);
   mu_assert_int_eq(true, removed == NULL);

   free(s);

   for (long i = 0; i < 100; i++) {
      free(digits[i]);
   }
}

/* test freeing all the items in a populated list. */

MU_TEST(test_list_free) {
   listd_item_t *n = NULL;
   char *digits[100];
   long r;

   /* load up 100 items */

   for (long i = 0; i < 100; i++) {
      digits[i] = calloc(16, 1);
      snprintf(digits[i], 15, "%ld", i);
      n = make_item(&list_by_payload, digits[i]);
      r = add_item(&list_by_payload, n);
      if (!r) {
         mu_assert_int_eq(true, r);
      }
   }
   mu_assert_int_eq(100, list_by_payload.count);
   mu_assert_int_eq(100, count_items(&list_by_payload));

   free_all_items(&list_by_payload);

   mu_assert_int_eq(0, list_by_payload.count);
   mu_assert_int_eq(0, count_items(&list_by_payload));

   for (long i = 0; i < 100; i++) {
      free(digits[i]);
   }
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
   char **splits = split_string(s, " ");
   char *ver[] = {NULL, "this", "is", "a", "test", "string", NULL};
   long i = 1;
   while (splits[i]) {
      mu_assert_string_eq(ver[i], splits[i]);
      i += 1;
   }
   free(splits[0]);
   free(splits);

   s = "and, now, for, something! else?";
   splits = split_string(s, " ,?");
   char *ver2[] = {NULL, "and", "now", "for", "something!", "else", NULL};
   i = 1;
   while (splits[i]) {
      mu_assert_string_eq(ver2[i], splits[i]);
      i += 1;
   }
   free(splits[0]);
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
