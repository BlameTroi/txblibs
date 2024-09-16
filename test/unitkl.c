/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../inc/misc.h"
#include "../inc/kl.h"
#include "../inc/str.h"
#include "../inc/rand.h"

static int
fn_compare_key_string(void *s1, void *s2) {
	return strcmp(s1, s2);
}

static int
fn_compare_key_long(void *i, void *j) {
	return (long)i - (long)j;
}

/*
 * minunit setup and teardown of listd infratstructure.
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
 * 100 items to work with, ids or keys run from 10 to 1000 by 10s.
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
		kl_insert(kl, (void *)i, dup_string(buffer));
	}
	return kl;
}

static void
destroy_populated_key_long(klcb *kl) {
	void *key;
	void *value;
	while (kl_get_first(kl, &key, &value)) {
		if (kl_delete(kl, key))
			continue;
		printf("\nerror on delete during teardown %s\n", kl_get_error(kl));
		break;
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
		snprintf(buffer, 99, "%06d i'm a key", i);
		kl_insert(kl, dup_string(buffer), dup_string(buffer));
	}
	return kl;
}

static void
destroy_populated_key_string(klcb *kl) {
	void *key;
	void *value;
	while (kl_get_first(kl, &key, &value)) {
		if (kl_delete(kl, key))
			continue;
		printf("\nerror on delete during teardown %s\n", kl_get_error(kl));
		break;
	}
	kl_reset(kl); /* in case of error above */
	kl_destroy(kl);
}

MU_TEST(test_create) {
	klcb *kl = kl_create(fn_compare_key_string);
	mu_should(kl);
	mu_should(kl_empty(kl));
	mu_should(kl_count(kl) == 0);
	mu_should(kl_destroy(kl));
}

MU_TEST(test_insert_single) {
	klcb *kl = kl_create(fn_compare_key_string);
	mu_should(kl_insert(kl, "abcd", "1234"));
	mu_shouldnt(kl_empty(kl));
	mu_should(kl_count(kl) == 1);
	/* note: following doesn't leak because the refs are on the
	 * stack and go out of scope when the function ends */
	mu_should(kl_reset(kl) == 1);
	mu_should(kl_empty(kl));
	mu_should(kl_count(kl) == 0);
	kl_destroy(kl);
}

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
	/* it takes more than two entries to mess with linking. */
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
	/* add two unique entries, then remove them. */
	mu_should(kl_insert(kl, "1", "first"));
	mu_should(kl_insert(kl, "2", "second"));
	mu_should(kl_count(kl) == 2);
	mu_should(kl_reset(kl) == 2);
	mu_should(kl_empty(kl));
	mu_should(kl_count(kl) == 0);
	/* it takes more than two entries to mess with linking. */
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
 * get the first item on the list.
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
 * and the last.
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
 * read backward from a specific item.
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

MU_TEST(test_update) {
	klcb *kl = create_populated_key_string();
	void *key = NULL;
	void *value = NULL;
	const char *msg = NULL;




	destroy_populated_key_string(kl);
}

MU_TEST(test_delete) {
}

/*
 * here we define the whole test suite. sadly there's no runtime
 * introspection. there is probably an opportunity for an elisp helper
 * to create the suite in the editor, but for now it's just a matter
 * of doing it manually.
 */

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

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
	MU_RUN_TEST(test_update);
	MU_RUN_TEST(test_delete);
	return;

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
