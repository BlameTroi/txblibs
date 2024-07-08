/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <assert.h>
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
 * tests for  txbdl -- doubly linked list
 *
 */

/*
 * change the following to control if using pthread calls
 */

#define USE_THREADING true

/*
 * for testing the payload is a string, so payload compare is just a
 * wrapper around strcmp.
 */

int
payload_compare(void *s1, void *s2) {
   return strcmp(s1, s2);
}

int
payload_compare_char4(void *s1, void *s2) {
   assert(strlen(s1) > 3 &&
          strlen(s2) > 3);
   return strncmp(s1, s2, 4);
}

int
payload_compare_long(void *i, void *j) {
   long r = (long)i - (long)j;
   if (r < 0) {
      return -1;
   }
   if (r == 0) {
      return 0;
   }
   return +1;
}

/*
 * for testing free any single block of allocated memory.
 */

void
payload_free(void *p) {
   free(p);
}

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

void
test_setup(void) {
   /* let's use a different seed than 1, but not time() because i want
    * repeatable tests. */
   srand(RAND_SEED);
}

void
test_teardown(void) {
}

/*
 * utility functions to create and destroy lists for testing. just a list of
 * 100 items to work with, ids or keys run from 10 to 1000 by 10s.
 */

dlcb *
create_populated_id_list(void) {
   char buffer[100];
   memset(buffer, 0, 100 * sizeof(char));

   dlcb *dl = dl_create_by_id(
                 USE_THREADING,
                 payload_free
              );
   assert(dl &&
          "error creating test data linked list");

   for (int i = 10; i < 1000; i += 10) {
      snprintf(buffer, 99, "%04d bogus", i);
      dl_insert(dl, i, strdup(buffer));
   }

   return dl;
}

void
destroy_populated_id_list(dlcb *dl) {
   dl_delete_all(dl);
   dl_destroy(dl);
   dl = NULL;
}

dlcb *
create_populated_key_list(
   int(*optional_payload_compare)(void *, void *)
) {

   char buffer[100];
   memset(buffer, 0, 100 * sizeof(char));

   if (optional_payload_compare == NULL) {
      optional_payload_compare = payload_compare;
   }
   dlcb *dl = dl_create_by_key(
                 USE_THREADING,
                 optional_payload_compare,
                 payload_free
              );
   assert(dl &&
          "error creating test data linked list");

   for (int i = 10; i < 1000; i += 10) {
      snprintf(buffer, 99, "%04d i'm a key", i);
      dl_insert(dl, i, strdup(buffer));
   }

   return dl;
}

void
destroy_populated_key_list(dlcb *dl) {
   dl_delete_all(dl);
   dl_destroy(dl);
   dl = NULL;
}

/*
 * the doubly linked list tests for identity keyed lists.
 */

MU_TEST(test_dl_id_create) {
   dlcb *dl = dl_create_by_id(
                 USE_THREADING,
                 NULL
              );
   mu_should(dl);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);
   mu_should(dl_destroy(dl));
}

/*
 * test adding a single entry to an identity list.
 */

MU_TEST(test_dl_id_add) {
   dlcb *dl = dl_create_by_id(
                 USE_THREADING,
                 NULL
              );
   mu_should(dl_insert(dl, 1, "1234"));
   mu_shouldnt(dl_empty(dl));
   mu_should(dl_count(dl) == 1);
   mu_should(dl_delete_all(dl) == 1);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);
   dl_destroy(dl);
}

/*
 * add multiples in various orders.
 */

MU_TEST(test_dl_id_add_multiple) {
   dlcb *dl = dl_create_by_id(
                 USE_THREADING,
                 NULL
              );
   /* add two unique entries, then remove them. */
   mu_should(dl_insert(dl, 1, "first"));
   mu_should(dl_insert(dl, 2, "second"));
   mu_should(dl_count(dl) == 2);
   mu_should(dl_delete_all(dl) == 2);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);

   /* it takes more than two entries to mess with linking. */
   mu_should(dl_insert(dl, 1, "first"));
   mu_should(dl_insert(dl, 4, "fourth, added second"));
   mu_should(dl_insert(dl, 2, "second, added third"));
   mu_should(dl_insert(dl, 3, "third, added fourth"));
   mu_should(dl_count(dl) == 4);

   /* now insert at front and then at back, knowing the ordering as we did above. */
   mu_should(dl_insert(dl, 0, "zeroeth, added fifth"));
   mu_should(dl_insert(dl, 5, "sixth, added sixth"));

   /* we'll confirm ordering in another set of tests. */
   mu_should(dl_count(dl) == 6);

   /* and now empty the list and we're done. */
   mu_should(dl_delete_all(dl) == 6);
   dl_destroy(dl);
}

MU_TEST(test_dl_id_add_duplicate) {
   dlcb *dl = dl_create_by_id(
                 USE_THREADING,
                 NULL
              );
   for (int i = 1; i < 10; i++) {
      dl_insert(dl, i, NULL);
   }
   mu_should(dl_count(dl) == 9);
   mu_should(dl_insert(dl, 20, NULL));           /* +1 */
   mu_shouldnt(dl_insert(dl, 5, NULL));          /* -- */
   mu_shouldnt(dl_insert(dl, 1, NULL));          /* -- */
   mu_should(dl_insert(dl, 19, NULL));           /* +1 */
   mu_shouldnt(dl_insert(dl, 19, NULL));         /* -- */
   mu_should(dl_delete_all(dl) == 11);
   dl_destroy(dl);
}

/*
 * create a list of randomly keyed entries. there should
 * be duplicates and we'll check counts.
 */

MU_TEST(test_dl_id_add_random) {
   dlcb *dl = dl_create_by_id(
                 USE_THREADING,
                 payload_free
              );
   int generated = 0;
   int added = 0;
   int duplicated = 0;
   for (int i = 0; i < 10000; i++) {
      int *p = malloc(sizeof(int));
      *p = rand_between(1, 5000);
      generated += 1;
      if (dl_insert(dl, *p, p)) {
         added += 1;
      } else {
         duplicated += 1;
      }
   }
   mu_should(generated == added + duplicated);
   mu_should(dl_count(dl) == added);
   dl_delete_all(dl);
   dl_destroy(dl);
}

/*
 * get the first item on the list.
 */

MU_TEST(test_dl_id_get_first) {
   dlcb *dl = create_populated_id_list();
   long id;
   void *payload;
   mu_should(dl_get_first(dl, &id, &payload));
   mu_should(id == 10);
   mu_should(strcmp(payload, "0010 bogus") == 0);
   destroy_populated_id_list(dl);
}

/*
 * and the last.
 */

MU_TEST(test_dl_id_get_last) {
   dlcb *dl = create_populated_id_list();
   long id;
   void *payload;
   mu_should(dl_get_last(dl, &id, &payload));
   mu_should(id == 990);
   mu_should(strcmp(payload, "0990 bogus") == 0);
   destroy_populated_id_list(dl);
}

/*
 * get by specific id keys.
 */

MU_TEST(test_dl_id_get) {
   dlcb *dl = create_populated_id_list();

   long id;
   void *payload;

   /* first in list */
   id = 10;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 10);
   mu_should(strcmp(payload, "0010 bogus") == 0);

   /* somewhere in the list */
   id = 100;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 100);
   mu_should(strcmp(payload, "0100 bogus") == 0);

   /* does not exist */
   id = 5;
   mu_shouldnt(dl_get(dl, &id, &payload));
   id = 11;
   mu_shouldnt(dl_get(dl, &id, &payload));
   id = 602;
   mu_shouldnt(dl_get(dl, &id, &payload));
   id = 989;
   mu_shouldnt(dl_get(dl, &id, &payload));
   id = 10000;
   mu_shouldnt(dl_get(dl, &id, &payload));

   /* last in list */
   id = 990;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 990);
   mu_should(strcmp(payload, "0990 bogus") == 0);

   destroy_populated_id_list(dl);
}

/*
 * read backward from a specific item.
 */

MU_TEST(test_dl_id_get_previous) {
   dlcb *dl = create_populated_id_list();
   long id;
   void *payload;

   /* somewhere in the list */
   id = 500;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 500);
   mu_should(strcmp(payload, "0500 bogus") == 0);

   /* read backwards a couple of times */
   mu_should(dl_get_previous(dl, &id, &payload));
   mu_should(id == 490);
   mu_should(dl_get_previous(dl, &id, &payload));
   mu_should(id == 480);
   mu_should(strcmp(payload, "0480 bogus") == 0);

   /* head of list */
   mu_should(dl_get_first(dl, &id, &payload));
   mu_should(id == 10);
   mu_shouldnt(dl_get_previous(dl, &id, &payload));

   /* but list access isn't broken */
   id = 370;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 370);
   mu_should(strcmp(payload, "0370 bogus") == 0);

   /* can't move from a failed get */
   id = 512;
   mu_shouldnt(dl_get(dl, &id, &payload));
   mu_shouldnt(dl_get_previous(dl, &id, &payload));

   destroy_populated_id_list(dl);
}

/*
 * and now read forward from a specific item.
 */

MU_TEST(test_dl_id_get_next) {
   dlcb *dl = create_populated_id_list();
   long id;
   void *payload;

   /* somewhere in the list */
   id = 500;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 500);
   mu_should(strcmp(payload, "0500 bogus") == 0);

   /* read forwards a couple of times */
   mu_should(dl_get_next(dl, &id, &payload));
   mu_should(id == 510);
   mu_should(dl_get_next(dl, &id, &payload));
   mu_should(id == 520);
   mu_should(strcmp(payload, "0520 bogus") == 0);

   /* end of list */
   mu_should(dl_get_last(dl, &id, &payload));
   mu_should(id == 990);
   mu_shouldnt(dl_get_next(dl, &id, &payload));

   /* but list access isn't broken */
   id = 370;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 370);
   mu_should(strcmp(payload, "0370 bogus") == 0);

   /* can't move from a failed get */
   id = 512;
   mu_shouldnt(dl_get(dl, &id, &payload));
   mu_shouldnt(dl_get_next(dl, &id, &payload));

   destroy_populated_id_list(dl);
}

/*
 * delete items.
 */

MU_TEST(test_dl_id_delete) {
   dlcb *dl = create_populated_id_list();
   long id;
   void *payload;

   mu_should(dl_count(dl) == 99);

   /* position to head */
   dl_get_first(dl, &id, &payload);
   mu_should(id == 10);
   mu_should(strcmp(payload, "0010 bogus") == 0);

   /* delete 10, alternating from that starting position */
   int deleted = 0;
   bool toggle = true;
   while (deleted < 10) {
      if (toggle) {
         mu_should(dl_delete(dl, id, payload));
         deleted += 1;
      }
      id += 10;
      toggle = !toggle;
   }

   /* we should now have 89 rows, 20, 40, 60, 80 ... */
   mu_should(dl_count(dl) == 89);
   dl_get_first(dl, &id, &payload);
   mu_should(id == 20);
   mu_should(dl_get_next(dl, &id, &payload));
   mu_should(id == 40);
   mu_should(strcmp(payload, "0040 bogus") == 0);

   /* delete a couple more rows */
   id = 500;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 500);
   mu_should(strcmp(payload, "0500 bogus") == 0);
   mu_should(dl_delete(dl, id, payload));
   id = 600;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 600);
   mu_should(strcmp(payload, "0600 bogus") == 0);
   mu_should(dl_delete(dl, id, payload));

   /* and so 490 and 510 are there, but 500 is not ... */
   id = 490;
   mu_should(dl_get(dl, &id, &payload));
   id = 500;
   mu_shouldnt(dl_get(dl, &id, &payload));
   id = 510;
   mu_should(dl_get(dl, &id, &payload));

   /* and check this way */
   id = 490;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(dl_get_next(dl, &id, &payload));
   mu_should(id == 510);

   destroy_populated_id_list(dl);
}

/*
 * update items. as the payload is dynamic, once it added to the
 * list a delete or update will call the payload_free function.
 */

MU_TEST(test_dl_id_update) {
   dlcb *dl = create_populated_id_list();

   long id;
   void *payload;

   mu_should(dl_count(dl) == 99);

   /* position to head */
   dl_get_first(dl, &id, &payload);
   mu_should(id == 10);
   mu_should(strcmp(payload, "0010 bogus") == 0);

   /* change data of 510 */
   id = 510;
   payload = NULL;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 510);
   mu_should(strcmp(payload, "0510 bogus") == 0);

   payload = strdup("0510 not bogus");
   mu_should(dl_update(dl, id, payload));

   id = 200;
   payload = NULL;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 200);
   mu_should(strcmp(payload, "0200 bogus") == 0);

   id = 510;
   payload = NULL;
   mu_should(dl_get(dl, &id, &payload));
   mu_should(id == 510);
   mu_should(strcmp(payload, "0510 not bogus") == 0);

   destroy_populated_id_list(dl);
}

/*
 * these are copy and paste and edited versions of the id tests
 * for a payload key list. same basic tests, just a few detals
 * need tweaking.
 */

MU_TEST(test_dl_key_create) {
   dlcb *dl = dl_create_by_key(
                 USE_THREADING,
                 payload_compare,
                 payload_free
              );
   mu_should(dl);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);
   mu_should(dl_destroy(dl));
}

MU_TEST(test_dl_key_add) {
   dlcb *dl = dl_create_by_key(
                 USE_THREADING,
                 payload_compare,
                 NULL
              );
   mu_should(dl);
   mu_should(dl);
   mu_should(dl_insert(dl, 1, "1234"));
   mu_shouldnt(dl_empty(dl));
   mu_should(dl_count(dl) == 1);
   mu_should(dl_delete_all(dl) == 1);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);
   mu_should(dl_destroy(dl));
}

MU_TEST(test_dl_key_add_random) {
   dlcb *dl = dl_create_by_key(
                 USE_THREADING,
                 payload_compare_long,
                 payload_free
              );
   int generated = 0;
   int added = 0;
   int duplicated = 0;
   for (long i = 0; i < 10000; i++) {
      long *p = malloc(sizeof(long));
      *p = rand_between(1, 5000);
      generated += 1;
      if (dl_insert(dl, *p, p)) {
         added += 1;
      } else {
         duplicated += 1;
      }
   }
   mu_should(generated == added + duplicated);
   mu_should(dl_count(dl) == added);
   dl_delete_all(dl);
   dl_destroy(dl);
}

MU_TEST(test_dl_key_add_multiple) {
   dlcb *dl = dl_create_by_id(
                 USE_THREADING,
                 NULL
              );

   /* add two unique entries, then remove them. */
   mu_should(dl_insert(dl, 1, "first"));
   mu_should(dl_insert(dl, 2, "second"));
   mu_should(dl_count(dl) == 2);
   mu_should(dl_delete_all(dl) == 2);
   mu_should(dl_empty(dl));
   mu_should(dl_count(dl) == 0);

   /* it takes more than two entries to mess with linking. */
   mu_should(dl_insert(dl, 1, "first"));
   mu_should(dl_insert(dl, 4, "fourth, added second"));
   mu_should(dl_insert(dl, 2, "second, added third"));
   mu_should(dl_insert(dl, 3, "third, added fourth"));
   mu_should(dl_count(dl) == 4);

   /* now insert at front and then at back, knowing the ordering as we did above. */
   mu_should(dl_insert(dl, 0, "zeroeth, added fifth"));
   mu_should(dl_insert(dl, 5, "sixth, added sixth"));

   /* we'll confirm ordering in another set of tests. */
   mu_should(dl_count(dl) == 6);

   /* and now empty the list and we're done. */
   mu_should(dl_delete_all(dl) == 6);
   dl_destroy(dl);
}

MU_TEST(test_dl_key_add_duplicate) {
   dlcb *dl = dl_create_by_key(USE_THREADING, payload_compare_long, NULL);
   for (int i = 1; i < 10; i++) {
      dl_insert(dl, i, (void *)(long)i);
   }
   mu_should(dl_count(dl) == 9);
   mu_should(dl_insert(dl, 20, (void *)(long)20));           /* +1 */
   mu_shouldnt(dl_insert(dl, 5, (void *)(long)5));           /* -- */
   mu_shouldnt(dl_insert(dl, 1, (void *)(long)1));           /* -- */
   mu_should(dl_insert(dl, 19, (void *)(long)19));           /* +1 */
   mu_shouldnt(dl_insert(dl, 19, (void *)(long)19));         /* -- */
   mu_should(dl_delete_all(dl) == 11);
   dl_destroy(dl);
}

MU_TEST(test_dl_key_get_first) {
   dlcb *dl = create_populated_key_list(NULL);
   long id;
   void *payload;
   mu_should(dl_get_first(dl, &id, &payload));
   mu_should(strcmp(payload, "0010 i'm a key") == 0);
   destroy_populated_key_list(dl);
}

MU_TEST(test_dl_key_get_last) {
   dlcb *dl = create_populated_key_list(NULL);
   long id;
   void *payload;

   mu_should(dl_get_last(dl, &id, &payload));
   mu_should(strcmp(payload, "0990 i'm a key") == 0);

   destroy_populated_key_list(dl);
}

MU_TEST(test_dl_key_get) {
   dlcb *dl = create_populated_key_list(NULL);
   long id;
   void *payload = NULL;

   mu_shouldnt(dl_get(dl, &id, &payload));
   payload = "0010 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strcmp(payload, "0010 i'm a key") == 0);

   /* somewhere in the list */
   payload = "0100 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strcmp(payload, "0100 i'm a key") == 0);

   /* does not exist */
   payload = "0005 blah";
   mu_shouldnt(dl_get(dl, &id, &payload));
   payload = "0011 meh";
   mu_shouldnt(dl_get(dl, &id, &payload));
   payload = "0602 there's a man on that page";
   mu_shouldnt(dl_get(dl, &id, &payload));
   payload = "0989";
   mu_shouldnt(dl_get(dl, &id, &payload));
   payload = "10000 miles";
   mu_shouldnt(dl_get(dl, &id, &payload));

   /* last in list */
   payload = "0990 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strcmp(payload, "0990 i'm a key") == 0);

   destroy_populated_key_list(dl);
}

MU_TEST(test_dl_key_get_previous) {
   dlcb *dl = create_populated_key_list(NULL);

   long id;
   void *payload;

   /* somewhere in the list */
   payload = "0500 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strncmp(payload, "0500", 4) == 0);

   /* read backwards a couple of times */
   mu_should(dl_get_previous(dl, &id, &payload));
   mu_should(strncmp(payload, "0490", 4) == 0);
   mu_should(dl_get_previous(dl, &id, &payload));
   mu_should(strncmp(payload, "0480", 4) == 0);

   /* head of list */
   mu_should(dl_get_first(dl, &id, &payload));
   mu_should(strncmp(payload, "0010", 4) == 0);
   mu_shouldnt(dl_get_previous(dl, &id, &payload));

   /* but list access isn't broken */
   payload = "0370 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strncmp(payload, "0370", 4) == 0);

   /* can't move from a failed get */
   payload = "0512 i don't exist";
   mu_shouldnt(dl_get(dl, &id, &payload));
   mu_shouldnt(dl_get_previous(dl, &id, &payload));

   destroy_populated_key_list(dl);
}

MU_TEST(test_dl_key_get_next) {
   dlcb *dl = create_populated_key_list(NULL);
   long id;
   void *payload;
   /* somewhere in the list */
   payload = "0500 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strncmp(payload, "0500", 4) == 0);

   /* read forwards a couple of times */
   mu_should(dl_get_next(dl, &id, &payload));
   mu_should(strncmp(payload, "0510", 4) == 0);
   mu_should(dl_get_next(dl, &id, &payload));
   mu_should(strncmp(payload, "0520", 4) == 0);

   /* end of list */
   mu_should(dl_get_last(dl, &id, &payload));
   mu_should(strncmp(payload, "0990", 4) == 0);
   mu_shouldnt(dl_get_next(dl, &id, &payload));


   /* but list access isn't broken */
   payload = "0370 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strncmp(payload, "0370", 4) == 0);

   /* can't move from a failed get */
   payload = "0512 i don't exist";
   mu_shouldnt(dl_get(dl, &id, &payload));
   mu_shouldnt(dl_get_next(dl, &id, &payload));

   destroy_populated_key_list(dl);
}

MU_TEST(test_dl_key_delete) {
   dlcb *dl = create_populated_key_list(NULL);

   long id;
   void *payload;

   mu_should(dl_count(dl) == 99);

   /* position to head */
   dl_get_first(dl, &id, &payload);
   mu_should(strncmp(payload, "0010", 4) == 0);

   /* delete 10, alternating from that starting position */
   int deleted = 0;
   bool toggle = true;
   char buffer[256];
   memset(buffer, 0, 256);
   int i = 10;
   while (deleted < 10) {
      if (toggle) {
         snprintf(buffer, 255, "%04d i'm a key", i);
         payload = &buffer;
         mu_should(dl_delete(dl, id, payload));
         deleted += 1;
      }
      i += 10;
      toggle = !toggle;
   }

   /* we should now have 89 rows, 10, 30, 40, 60 ... */
   mu_should(dl_count(dl) == 89);
   dl_get_first(dl, &id, &payload);
   mu_should(strncmp(payload, "0020", 4) == 0);
   mu_should(dl_get_next(dl, &id, &payload));
   mu_should(strncmp(payload, "0040", 4) == 0);

   /* delete a couple more rows */
   payload = "0500 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strncmp(payload, "0500", 4) == 0);
   mu_should(dl_delete(dl, id, payload));
   payload = "0600 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strncmp(payload, "0600", 4) == 0);
   mu_should(dl_delete(dl, id, payload));

   /* and so 490 and 510 are there, but 500 is not ... */
   payload = "0490 i'm a key";
   mu_should(dl_get(dl, &id, &payload));
   payload = "0500 i'm a key";
   mu_shouldnt(dl_get(dl, &id, &payload));
   payload = "0510 i'm a key";
   mu_should(dl_get(dl, &id, &payload));

   destroy_populated_key_list(dl);
}

MU_TEST(test_dl_key_update) {
   dlcb *dl = create_populated_key_list(payload_compare_char4);

   long id;
   void *payload;

   mu_should(dl_count(dl) == 99);

   /* position to head */
   dl_get_first(dl, &id, &payload);
   mu_should(strncmp(payload, "0010", 4) == 0);

   /* change data of 510 */
   payload = "0510";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strcmp(payload, "0510 i'm a key") == 0);

   payload = strdup("0510 not bogus");
   mu_should(dl_update(dl, id, payload));

   payload = "0200";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strcmp(payload, "0200 i'm a key") == 0);

   payload = "0510";
   mu_should(dl_get(dl, &id, &payload));
   mu_should(strcmp(payload, "0510 not bogus") == 0);

   destroy_populated_key_list(dl);
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

   printf("\n\ndoubly linked list tests -- id key\n\n");

   MU_RUN_TEST(test_dl_id_create);
   MU_RUN_TEST(test_dl_id_add);
   MU_RUN_TEST(test_dl_id_add_multiple);
   MU_RUN_TEST(test_dl_id_add_duplicate);
   MU_RUN_TEST(test_dl_id_add_random);
   MU_RUN_TEST(test_dl_id_get_first);
   MU_RUN_TEST(test_dl_id_get_last);
   MU_RUN_TEST(test_dl_id_get);
   MU_RUN_TEST(test_dl_id_get_previous);
   MU_RUN_TEST(test_dl_id_get_next);
   MU_RUN_TEST(test_dl_id_delete);
   MU_RUN_TEST(test_dl_id_update);


   printf("\n\ndoubly linked list tests -- payload key\n\n");
   MU_RUN_TEST(test_dl_key_create);
   MU_RUN_TEST(test_dl_key_add);
   MU_RUN_TEST(test_dl_key_add_multiple);
   MU_RUN_TEST(test_dl_key_add_duplicate);
   MU_RUN_TEST(test_dl_key_add_random);
   MU_RUN_TEST(test_dl_key_get_first);
   MU_RUN_TEST(test_dl_key_get_last);
   MU_RUN_TEST(test_dl_key_get);
   MU_RUN_TEST(test_dl_key_get_previous);
   MU_RUN_TEST(test_dl_key_get_next);
   MU_RUN_TEST(test_dl_key_delete);
   MU_RUN_TEST(test_dl_key_update);


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
