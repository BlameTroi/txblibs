/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "misc.h"
#include "kl.h"
#include "str.h"
#include "rand.h"

/*
 * fn_compare_key for strings and longs
 *
 * these follow the same api as cmpst, as expected by
 * qsort.
 */

static int
fn_compare_key_string(void *s1, void *s2) {
	return strcmp(s1, s2);
}

static int
fn_compare_key_long(void *i, void *j) {
	return (long)i - (long)j;
}

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

static void
test_setup(void) {
	/* let's use a different seed than 1, but not time() because i want
	   repeatable tests. */
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
}

static void
test_teardown(void) {
}

/*
 * utility functions to create and destroy lists for testing. just a list of
 * 100 items to work with, keys run from 10 to 990 by 10s.
 */

static klcb *
create_populated_key_long(void) {
	char buffer[100];
	memset(buffer, 0, 100 * sizeof(char));
	klcb *kl = kl_create(fn_compare_key_long);
	assert(kl &&
		"error creating test data linked list");
	for (long i = 10; i < 1000; i += 10) {
		snprintf(buffer, 99, "%06ld bogus", i);
		kl_insert(kl, (void *)i, strdup(buffer));
	}
	return kl;
}

static void
destroy_populated_key_long(klcb *kl) {
	void *key;
	void *value;
	while (kl_get_first(kl, &key, &value)) {
		if (!kl_delete(kl, key)) {
			printf("\nerror on delete during teardown %s\n", kl_get_error(kl));
			break;
		}
		free(value);
	}
	kl_reset(kl); /* in case of error above */
	kl_destroy(kl);
}

static klcb *
create_populated_key_string(void) {
	char buffer[100];
	memset(buffer, 0, 100 * sizeof(char));
	klcb *kl = kl_create(fn_compare_key_string);
	assert(kl &&
		"error creating test data linked list");
	for (int i = 10; i < 1000; i += 10) {
		snprintf(buffer, 99, "%06d", i);
		kl_insert(kl, strdup(buffer), strdup(buffer));
	}
	return kl;
}

static void
destroy_populated_key_string(klcb *kl) {
	void *key;
	void *value;
	while (kl_get_first(kl, &key, &value)) {
		if (!kl_delete(kl, key)) {
			printf("\nerror on delete during teardown %s\n", kl_get_error(kl));
			break;
		}
		free(key);
		free(value);
	}
	kl_reset(kl); /* in case of error above */
	kl_destroy(kl);
}

/*
 * test_create
 *
 * an empty kl.
 */

MU_TEST(test_create) {
	klcb *kl = kl_create(fn_compare_key_string);
	mu_should(kl);
	mu_should(kl_empty(kl));
	mu_should(kl_count(kl) == 0);
	mu_should(kl_destroy(kl));
}

/*
 * test_insert_single
 *
 * insert one item into the kl.
 */

MU_TEST(test_insert_single) {
	klcb *kl = kl_create(fn_compare_key_string);
	mu_should(kl_insert(kl, "abcd", "1234"));
	mu_shouldnt(kl_empty(kl));
	mu_should(kl_count(kl) == 1);
	/* note: following doesn't leak because the refs to
	 * constants. */
	mu_should(kl_reset(kl) == 1);
	mu_should(kl_empty(kl));
	mu_should(kl_count(kl) == 0);
	kl_destroy(kl);
}

/*
 * tet_insert_multiple
 *
 * insert several items into a list that uses longs as keys. the
 * items are added out of key sequence order but they should be
 * correctly stored in order.
 */

MU_TEST(test_insert_multiple) {
	klcb *kl = NULL;
	/* using longs as keys, quick tests */
	kl = kl_create(fn_compare_key_long);
	mu_should(kl_insert(kl, (void *)1L, "first"));
	mu_should(kl_insert(kl, (void *)2L, "second"));
	mu_should(kl_count(kl) == 2);
	mu_should(kl_reset(kl) == 2);
	mu_should(kl_empty(kl));
	mu_should(kl_count(kl) == 0);
	/* it takes more than two items to mess with linking. */
	mu_should(kl_insert(kl, (void *)1L, "first"));
	mu_should(kl_insert(kl, (void *)4L, "fourth, added second"));
	mu_should(kl_insert(kl, (void *)2L, "second, added third"));
	mu_should(kl_insert(kl, (void *)3L, "third, added fourth"));
	mu_should(kl_count(kl) == 4);
	/* now insert at front and then at back, knowing the ordering as we did above. */
	mu_should(kl_insert(kl, (void *)0L, "zeroeth, added fifth"));
	mu_should(kl_insert(kl, (void *)5L, "sixth, added sixth"));
	/* we'll confirm ordering in another set of tests. */
	mu_should(kl_count(kl) == 6);
	/* and now empty the list and we're done. */
	mu_should(kl_reset(kl) == 6);
	kl_destroy(kl);
	/* do the same tests with string keys and then confirm
	   ordering, we'll do some get next stuff here but the
	   real tests those functions are elsewhere. */
	kl = kl_create(fn_compare_key_string);
	/* add two unique items, then remove them. */
	mu_should(kl_insert(kl, "1", "first"));
	mu_should(kl_insert(kl, "2", "second"));
	mu_should(kl_count(kl) == 2);
	mu_should(kl_reset(kl) == 2);
	mu_should(kl_empty(kl));
	mu_should(kl_count(kl) == 0);
	/* it takes more than two items to mess with linking. */
	mu_should(kl_insert(kl, "1", "first, added first"));
	mu_should(kl_insert(kl, "4", "fourth, added second"));
	mu_should(kl_insert(kl, "2", "second, added third"));
	mu_should(kl_insert(kl, "3", "third, added fourth"));
	mu_should(kl_count(kl) == 4);
	/* now insert at front and then at back, knowing the ordering as we did above. */
	mu_should(kl_insert(kl, "0", "zero, added fifth"));
	mu_should(kl_insert(kl, "5", "five, added sixth"));
	/* check ordering */
	mu_should(kl_count(kl) == 6);
	void *key;
	void *value;
	bool got = false;
	char *expected[] = { "0", "1", "2", "3", "4", "5", NULL };
	int i = 0;
	got = kl_get_first(kl, &key, &value);
	while (got) {
		printf("\n%s %s ", (char *)key, (char *)value);
		mu_should(equal_string(key, expected[i]));
		i += 1;
		got = kl_get_next(kl, &key, &value);
	}
	/* and now empty the list and we're done. */
	mu_should(kl_reset(kl) == 6);
	kl_destroy(kl);
}

/*
 * test_insert_duplicate
 *
 * build a list keyed 1->9 and then try to insert new
 * items with unique and duplicate keys.
 */

MU_TEST(test_insert_duplicate) {
	klcb *kl = kl_create(fn_compare_key_long);
	for (long i = 1; i < 10; i++)
		kl_insert(kl, (void *)i, NULL);
	mu_should(kl_count(kl) == 9);
	mu_should(kl_insert(kl, (void *)20L, NULL));          /* +1 */
	mu_shouldnt(kl_insert(kl, (void *)5, NULL));          /* -- */
	mu_shouldnt(kl_insert(kl, (void *)1, NULL));          /* -- */
	mu_should(kl_insert(kl, (void *)19, NULL));           /* +1 */
	mu_shouldnt(kl_insert(kl, (void *)19, NULL));         /* -- */
	mu_should(kl_reset(kl) == 11);
	kl_destroy(kl);
}

/*
 * test_insert_random
 *
 * insert up to 10,000 random numbers into the list. this
 * is a bit like throwing spaghetti against the wall but
 * it works well enough.
 */

MU_TEST(test_insert_random) {
	klcb *kl = kl_create(fn_compare_key_long);
	int generated = 0;
	int inserted = 0;
	int duplicates = 0;
	for (long i = 0; i < 10000; i++) {
		long p = random_between(1, 5000);
		generated += 1;
		if (kl_insert(kl, (void *)p, (void *)p))
			inserted += 1;
		else
			duplicates += 1;
	}
	printf("\ngenerated %d\ninserted %d\nduplicates %d\n", generated, inserted,
		duplicates);
	mu_should(generated == inserted + duplicates);
	mu_should(kl_count(kl) == inserted);
	kl_reset(kl);
	kl_destroy(kl);
}

/*
 * test_get_first
 *
 * get the first item, the one with the smallest key, on the list.
 */

MU_TEST(test_get_first) {
	printf("\n");
	klcb *kl = create_populated_key_long();
	void *key = NULL;
	void *value = NULL;
	const char *msg = NULL;
	mu_should(kl_get_first(kl, &key, &value));
	printf("%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 10L);
	mu_should(equal_string(value, "000010 bogus"));
	msg = kl_get_error(kl);
	mu_shouldnt(msg);
	destroy_populated_key_long(kl);
}

/*
 * test_get_last
 *
 * get the last item, the one with the largest key, on the list.
 */

MU_TEST(test_get_last) {
	printf("\n");
	klcb *kl = create_populated_key_long();
	void *key = NULL;
	void *value = NULL;
	const char *msg = NULL;
	mu_should(kl_get_last(kl, &key, &value));
	printf("%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 990L);
	mu_should(equal_string(value, "000990 bogus"));
	msg = kl_get_error(kl);
	mu_shouldnt(msg);
	destroy_populated_key_long(kl);
}

/*
 * test_get_specific
 *
 * get items by known keys at various points in the list.
 */

MU_TEST(test_get_specific) {
	printf("\n");
	klcb *kl = create_populated_key_long();
	void *key = NULL;
	void *value = NULL;
	const char *msg = NULL;
	/* somewhere in the list */
	key = (void *)30L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 30L);
	mu_should(equal_string(value, "000030 bogus"));
	msg = kl_get_error(kl);
	mu_shouldnt(msg);
	/* does not exist */
	key = (void *)35L;
	mu_shouldnt(kl_get(kl, &key, &value));
	mu_should((long)key == 35L);
	mu_should(value == NULL);
	mu_should(kl_get_error(kl));
	/* somewhere else in the list */
	key = (void *)500L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 500L);
	mu_should(equal_string(value, "000500 bogus"));
	/* last in list by key */
	key = (void *)990L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 990L);
	mu_should(equal_string(value, "000990 bogus"));
	/* first in list by key */
	key = (void *)10L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 10L);
	mu_should(equal_string(value, "000010 bogus"));
	destroy_populated_key_long(kl);
}

/*
 * test_get_previous
 *
 * read backward from various locations. make sure reading backward
 * from the head of the list reports that the end was reached.
 */

MU_TEST(test_get_previous) {
	printf("\n");
	klcb *kl = create_populated_key_long();
	void *key = NULL;
	void *value = NULL;
	const char *msg = NULL;
	/* somewhere in the list */
	key = (void *)500L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 500L);
	mu_should(equal_string(value, "000500 bogus"));
	/* read backwards a couple of times */
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should(kl_get_previous(kl, &key, &value));
	mu_should((long)key == 490L);
	mu_should(kl_get_previous(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 480L);
	mu_should(equal_string(value, "000480 bogus"));
	/* head of list */
	mu_should(kl_get_first(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 10);
	mu_shouldnt(kl_get_previous(kl, &key, &value));
	mu_should(kl_get_error(kl));
	/* but list access isn't broken */
	key = (void *)370L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 370);
	mu_should(equal_string(value, "000370 bogus"));
	/* can't move from a failed get */
	key = (void *)512L;
	mu_shouldnt(kl_get(kl, &key, &value));
	msg = kl_get_error(kl);
	mu_should(msg);
	printf("\n%s\n", msg);
	mu_shouldnt(kl_get_previous(kl, &key, &value));
	msg = kl_get_error(kl);
	mu_should(msg);
	printf("\n%s\n", msg);
	destroy_populated_key_long(kl);
}

/*
 * test_get_next
 *
 * read forward from various locations. make sure reading forward
 * from the tail of the list reports that the end was reached.
 */

MU_TEST(test_get_next) {
	printf("\n");
	klcb *kl = create_populated_key_long();
	void *key = NULL;
	void *value = NULL;
	const char *msg = NULL;
	/* somewhere in the list */
	key = (void *)500L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 500L);
	mu_should(equal_string(value, "000500 bogus"));
	/* read forwards a couple of times */
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should(kl_get_next(kl, &key, &value));
	mu_should((long)key == 510L);
	mu_should(kl_get_next(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 520L);
	mu_should(equal_string(value, "000520 bogus"));
	/* tail of list */
	mu_should(kl_get_last(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 990L);
	mu_shouldnt(kl_get_next(kl, &key, &value));
	mu_should(kl_get_error(kl));
	/* but list access isn't broken */
	key = (void *)370L;
	mu_should(kl_get(kl, &key, &value));
	printf("\n%ld %s\n", (long)key, (char *)value);
	mu_should((long)key == 370);
	mu_should(equal_string(value, "000370 bogus"));
	/* can't move from a failed get */
	key = (void *)512L;
	mu_shouldnt(kl_get(kl, &key, &value));
	msg = kl_get_error(kl);
	mu_should(msg);
	printf("\n%s\n", msg);
	mu_shouldnt(kl_get_next(kl, &key, &value));
	msg = kl_get_error(kl);
	mu_should(msg);
	printf("\n%s\n", msg);
	destroy_populated_key_long(kl);
}

/*
 * test_clone
 *
 * create a shallow copy of a list, possibly useful for iteration.
 */

MU_TEST(test_clone
) {
	klcb *kl = create_populated_key_long();
	klcb *clone = kl_clone(kl);
	mu_shouldnt(kl_empty(clone));
	mu_should(kl_count(kl) == kl_count(clone));
	destroy_populated_key_long(kl);
	/* kl_clone has to be a shallow copy since kl knows nothing
	 * about the actual keys and values. destroy_populated_key*
	 * frees keys and values as items are deleted, so
	 * destroy_populated_key* on the clone will abort when
	 * attempting to do a duplicate free. */
	kl_reset(clone);
	kl_destroy(clone);
}

/*
 * test_update
 *
 * read and modify items in a list.
 */

MU_TEST(test_update) {
	klcb *kl = create_populated_key_long();
	void *key = NULL;
	void *value = NULL;
	char *keep = NULL;
	char *hand = NULL;

	/* update head of list */
	mu_should(kl_get_first(kl, &key, &value));
	mu_should((long)key == 10L);
	keep = strdup(value);
	hand = value;
	value = strdup("i used to be 10");
	mu_should(kl_update(kl, key, value));
	free(hand);
	free(keep);

	/* move off the head and the return */
	mu_should(kl_get_last(kl, &key, &value));
	mu_should((long)key == 990L);
	mu_should(kl_get_first(kl, &key, &value));
	mu_should((long)key == 10L);
	mu_should(equal_string("i used to be 10", value));

	/* update tail of list */
	mu_should(kl_get_last(kl, &key, &value));
	mu_should((long)key == 990L);
	keep = strdup(value);
	hand = value;
	value = strdup("i used to be 990");
	mu_should(kl_update(kl, key, value));
	free(hand);
	free(keep);

	/* move off the tail and the return */
	mu_should(kl_get_first(kl, &key, &value));
	mu_should((long)key == 10L);
	mu_should(kl_get_last(kl, &key, &value));
	mu_should((long)key == 990L);
	mu_should(equal_string("i used to be 990", value));

	/* this should scan all the links forward */
	mu_should(kl_count(kl) == 99);

	/* in the middle of the list */
	key = (void *)500L;;
	mu_should(kl_get(kl, &key, &value));
	mu_should((long)key == 500L);
	keep = strdup(value);
	hand = value;
	value = strdup("i used to be 500");
	mu_should(kl_update(kl, key, value));
	free(hand);
	free(keep);

	/* move off this node and the return */
	key = (void *)750L;
	mu_should(kl_get(kl, &key, &value));
	mu_should((long)key == 750L);
	key = (void *)500L;
	mu_should(kl_get(kl, &key, &value));
	mu_should((long)key == 500L);
	mu_should(equal_string("i used to be 500", value));

	/* this should scan all the links forward */
	mu_should(kl_count(kl) == 99);

	destroy_populated_key_long(kl);
}

/*
 * test_delete
 *
 * delete items at the ends and in the middle of the list.
 */

MU_TEST(test_delete) {
	klcb *kl = create_populated_key_string();
	void *key = NULL;
	void *value = NULL;

	/* delete somewhere in the middle */
	key = "000500";
	mu_should(kl_get(kl, &key, &value));
	mu_should(equal_string("000500", key));
	mu_should(kl_delete(kl, key));
	free(value);
	free(key);
	mu_should(kl_count(kl) == 98);
	key = "000500";
	mu_shouldnt(kl_get(kl, &key, &value));
	mu_should(kl_get_error(kl));

	/* delete head */
	mu_should(kl_get_first(kl, &key, &value));
	mu_should(equal_string("000010", key));
	mu_should(kl_delete(kl, key));
	free(value);
	free(key);
	mu_should(kl_get_first(kl, &key, &value));
	mu_should(equal_string("000020", key));
	mu_should(kl_count(kl) == 97);
	key = "000010";
	mu_shouldnt(kl_get(kl, &key, &value));
	mu_should(kl_get_error(kl));

	/* delete tail */
	mu_should(kl_get_last(kl, &key, &value));
	mu_should(equal_string("000990", key));
	mu_should(kl_delete(kl, key));
	free(value);
	free(key);
	mu_should(kl_get_last(kl, &key, &value));
	mu_should(equal_string("000980", key));
	mu_should(kl_count(kl) == 96);
	key = "000990";
	mu_shouldnt(kl_get(kl, &key, &value));
	mu_should(kl_get_error(kl));

	/* and done */
	destroy_populated_key_string(kl);
}

/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_insert_single);
	MU_RUN_TEST(test_insert_multiple);
	MU_RUN_TEST(test_insert_duplicate);
	MU_RUN_TEST(test_insert_random);
	MU_RUN_TEST(test_get_first);
	MU_RUN_TEST(test_get_last);
	MU_RUN_TEST(test_get_specific);
	MU_RUN_TEST(test_get_previous);
	MU_RUN_TEST(test_get_next);
	MU_RUN_TEST(test_clone);
	MU_RUN_TEST(test_update);
	MU_RUN_TEST(test_delete);

	return; /* move me to skip working tests */

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
