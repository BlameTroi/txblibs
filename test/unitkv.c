/* unitkv.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */


#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "../inc/kv.h"

/*
 * minunit setup and teardown.
 */

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * key comparators for integer and string keys.
 */

int
fn_key_compare_int(void *a, void *b) {
	return *(int *)a - *(int *)b;
}

int
fn_key_compare_string(void *a, void *b) {
	return strcmp(a, b);
}

/*
 * test data.
 */

int int_keyed[][2] = {
	{ 0, 0 },
	{ 1, 1 },
	{ 2, 2 },
	{ 3, 3 },
	{ 4, 4 },
	{ 5, 5 },
	{ 6, 6 },
	{ 7, 7 },
	{ 8, 8 },
	{ 9, 9 },
	{ -1, -1 }
};

typedef struct str_int str_int;
struct str_int {
	char *key;
	int value;
};

str_int str_keyed[] = {
	{ "alpha", 0 },
	{ "bravo", 1 },
	{ "charlie", 17 },
	{ "delta", -7 },
	{ "echo", 15 },
	{ "foxtrot", 69 },
	{ NULL, 0 }
};

typedef struct int_str int_str;
struct int_str {
	int key;
	char *value;
};

int_str str_valued[] = {
	{ 0, "zero" },
	{ 1, "one" },
	{ 2, "two" },
	{ 3, "three" },
	{ 4, "four" },
	{ 5, "five" },
	{ 6, "six" },
	{ 7, "seven" },
	{ 8, "eight" },
	{ 9, "nine" },
	{ -1, NULL }
};

/*
 * loaders for test data test data
 */

kvcb *
load_ints(void) {
	kvcb *kv = kv_create(fn_key_compare_int);
	int i = 0;
	while (int_keyed[i][0] != -1) {
		kv_put(kv, &int_keyed[i][0], &int_keyed[i][1]);
		i += 1;
	}
	return kv;
}

kvcb *
load_strs(void) {
	kvcb *kv = kv_create(fn_key_compare_int);
	int i = 0;
	while (str_valued[i].value) {
		kv_put(kv, &str_valued[i].key, str_valued[i].value);
		i += 1;
	}
	return kv;
}

kvcb *
load_str_keys(void) {
	kvcb *kv = kv_create(fn_key_compare_string);
	int i = 0;
	while (str_keyed[i].key) {
		kv_put(kv, str_keyed[i].key, &str_keyed[i].value);
		i += 1;
	}
	return kv;
}

/*
 * test_create
 *
 * create a new rational empty instance.
 */

MU_TEST(test_create) {
	kvcb *kv = kv_create(fn_key_compare_int);
	mu_should(kv);
	mu_should(kv_count(kv) == 0);
	mu_should(kv_empty(kv));
	kv_destroy(kv);
}

/*
 * test_load
 *
 * load from the int[][] dataset and verify.
 */

MU_TEST(test_load) {
	kvcb *kv = kv_create(fn_key_compare_int);

	int i = 0;
	while (int_keyed[i][0] != -1) {
		kv_put(kv, &int_keyed[i][0], &int_keyed[i][1]);
		i += 1;
	}
	mu_shouldnt(kv_empty(kv));
	mu_should(kv_count(kv) == i);

	/* do we read back a known loaded row? */
	int k = 1;
	mu_should(kv_get(kv, &k));

	/* do we read back a known not to be loaded row? */
	k = 100;
	mu_shouldnt(kv_get(kv, &k));

	kv_reset(kv);
	kv_destroy(kv);
}

/*
 * test_put
 *
 * update an exixsting key's value.
 */

MU_TEST(test_put) {
	kvcb *kv = load_ints();

	/* we expect 10 pairs, and keys 4 and 5
	 * have values of 4 and 5 */
	mu_should(kv_count(kv) == 10);
	int k = 4;
	mu_should(*(int *)kv_get(kv, &k) == 4);
	k = 5;
	mu_should(*(int *)kv_get(kv, &k) == 5);

	/* change 5:5 to 5:8 */
	int v = 8;
	int *pv = kv_put(kv, &k, &v);
	mu_should(*pv);
	mu_should(*pv == 8);

	/* but 4:4 should still be 4:4 */
	k = 4;
	mu_should(*(int *)kv_get(kv, &k) == 4);

	kv_reset(kv);
	kv_destroy(kv);
}

/*
 * test_delete
 *
 * delete specific pairs.
 */

MU_TEST(test_delete) {
	kvcb *kv = load_ints();

	/* establish that pairs 2, 8, and 9 exist */
	mu_should(kv_count(kv) == 10);
	int k = 8;
	mu_should(kv_get(kv, &k));
	k = 2;
	mu_should(kv_get(kv, &k));
	k = 9;
	mu_should(kv_get(kv, &k));

	/* and that key 101 does not */
	k = 101;
	mu_shouldnt(kv_get(kv, &k));

	/* delete 8 */
	k = 8;
	mu_should(kv_delete(kv, &k));
	mu_should(kv_count(kv) == 9);

	/* are 2 and 9 still there */
	k = 2;
	mu_should(kv_get(kv, &k));
	k = 9;
	mu_should(kv_get(kv, &k));

	/* and 8 is gone */
	k = 8;
	mu_shouldnt(kv_get(kv, &k));

	/* while here, test that destroy checks for pairs */
	mu_shouldnt(kv_destroy(kv));
	mu_should(kv_reset(kv) == 9);
	kv_destroy(kv);
}

MU_TEST(test_keys) {
	kvcb *kv = load_ints();
	mu_should(kv_count(kv) == 10);

	/* get a list of all the keys in the store */
	int **keys = kv_keys(kv);
	mu_should(keys);

	/* print them and their associated values */
	printf("\n");
	for (int i = 0; keys[i]; i++)
		printf("%d %d\n", *keys[i], *(int *)kv_get(kv, keys[i]));
	free(keys);

	kv_reset(kv);
	kv_destroy(kv);
}

MU_TEST(test_values) {
	kvcb *kv = load_strs();
	mu_should(kv_count(kv) == 10);

	/* get the list of all the values in the store */
	char **values = kv_values(kv);
	mu_should(values);

	/* all we can really do here is print and count them */
	printf("\n");
	int k = 0;
	for (int i = 0; values[i]; i++) {
		k += 1;
		printf("%s\n", values[i]);
	}
	mu_should(k == kv_count(kv));
	free(values);

	kv_reset(kv);
	kv_destroy(kv);
}

MU_TEST(test_string_keys) {
	kvcb *kv = load_str_keys();
	mu_should(kv_count(kv) == 6);

	/* key and value lists */
	char **keys = kv_keys(kv);
	mu_should(keys);
	int **values = kv_values(kv);
	mu_should(values);

	/* are the keys that loaded strings? the printf will tell */
	printf("\n");
	for (int i = 0; keys[i]; i++)
		printf("%s %d\n", keys[i], *values[i]);
	free(values);
	free(keys);

	/* check known key:value pairs */
	int *value = kv_get(kv, "alpha");
	mu_should(*value == 0);
	value = kv_get(kv, "charlie");
	mu_should(*value == 17);

	mu_should(kv_reset(kv) == 6);
	kv_destroy(kv);
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

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	/* run your tests here */

	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_load);
	MU_RUN_TEST(test_put);
	MU_RUN_TEST(test_delete);
	MU_RUN_TEST(test_keys);
	MU_RUN_TEST(test_values);
	MU_RUN_TEST(test_string_keys);
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

/* unitkv.c ends here */
