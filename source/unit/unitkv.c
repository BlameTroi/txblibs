/* unitkv.c -- tests for the key:value store library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "minunit.h"
#include "txbrand.h"
#include "txbkv.h"

/*
 * minunit setup and teardown.
 */

void
test_setup(void) {
	set_random_generator(RAND_RANDOM);
	/*      seed_random_generator(6803); */
}

void
test_teardown(void) {
}

/*
 * key comparators for integer and string keys.
 */

int
fn_key_compare_int(const void *a, const void *b) {
	return *(int *)a - *(int *)b;
}

int
fn_key_compare_string(const void *a, const void *b) {
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

hkv *
load_ints(void) {
	hkv *kv = kv_create(fn_key_compare_int);
	int i = 0;
	while (int_keyed[i][0] != -1) {
		kv_put(kv, &int_keyed[i][0], &int_keyed[i][1]);
		i += 1;
	}
	return kv;
}

hkv *
load_strs(void) {
	hkv *kv = kv_create(fn_key_compare_int);
	int i = 0;
	while (str_valued[i].value) {
		kv_put(kv, &str_valued[i].key, str_valued[i].value);
		i += 1;
	}
	return kv;
}

hkv *
load_str_keys(void) {
	hkv *kv = kv_create(fn_key_compare_string);
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
	hkv *kv = kv_create(fn_key_compare_int);
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
	int ia = 1;
	int ib = 2;
	char *sa = "1";
	char *sb = "2";
	int ca = fn_key_compare_string(sa, sb);
	int cb = fn_key_compare_int(&ia, &ib);
	mu_should(ca == cb);

	hkv *kv = kv_create(fn_key_compare_int);

	int i = 0;
	while (int_keyed[i][0] != -1) {
		kv_put(kv, &int_keyed[i][0], &int_keyed[i][1]);
		i += 1;
	}
	mu_shouldnt(kv_empty(kv));
	mu_should(kv_count(kv) == i);

	pkey *keys = kv_keys(kv);
	pvalue *values = kv_values(kv);

	mu_should(keys);
	mu_should(values);

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
	hkv *kv = load_ints();

	/* get a list of all the keys in the store */
	pkey *keys = kv_keys(kv);
	mu_should(keys);

	/* print them and their associated values */
	printf("\n");
	for (int i = 0; keys[i]; i++)
		printf("%d %d\n", *(int *)keys[i], *(int *)kv_get(kv, keys[i]));
	free(keys);

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
	hkv *kv = load_ints();

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
	hkv *kv = load_ints();
	mu_should(kv_count(kv) == 10);

	/* get a list of all the keys in the store */
	pkey *keys = kv_keys(kv);
	mu_should(keys);

	/* print them and their associated values */
	printf("\n");
	for (int i = 0; keys[i]; i++)
		printf("%d %d\n", *(int *)keys[i], *(int *)kv_get(kv, keys[i]));
	free(keys);

	kv_reset(kv);
	kv_destroy(kv);
}

MU_TEST(test_values) {
	hkv *kv = load_strs();
	mu_should(kv_count(kv) == 10);

	/* get the list of all the values in the store */
	pvalue *values = kv_values(kv);
	mu_should(values);

	/* all we can really do here is print and count them */
	printf("\n");
	int k = 0;
	for (int i = 0; values[i]; i++) {
		k += 1;
		printf("%s\n", (char *)values[i]);
	}
	mu_should(k == kv_count(kv));
	free(values);

	kv_reset(kv);
	kv_destroy(kv);
}

MU_TEST(test_volume_ascending) {
	hkv *kv = kv_create(fn_key_compare_int);
	/* grow a few times */
	clock_t b = clock();
	for (int i = 1; i <= 10000; i++) {
		int *j = malloc(sizeof(int));
		*j = i;
		kv_put(kv, j, j);
	}
	clock_t e = clock();
	printf("\ntime ascending %lu\n", b-e);
	/* add after end (which is 10000) */
	int *after = malloc(sizeof(int));
	*after = 10010;
	kv_put(kv, after, after);
	/* and in front of */
	int *before = malloc(sizeof(int));
	*before = -10;
	kv_put(kv, before, before);
	/* between first two and between last two */
	int *penultimate = malloc(sizeof(int));
	*penultimate = 10005;
	kv_put(kv, penultimate, penultimate);
	mu_should(kv_count(kv) == 10003);
	int *anteoriginal = malloc(sizeof(int));
	*anteoriginal = 0;
	kv_put(kv, anteoriginal, anteoriginal);
	mu_should(kv_count(kv) == 10004);
	pkey *keys = kv_keys(kv);
	mu_should(*(int *)keys[0] == -10);
	mu_should(*(int *)keys[1] == 0);
	mu_should(*(int *)keys[2] == 1);
	mu_should(*(int *)keys[3] == 2);
	mu_should(*(int *)keys[10000] == 9999);
	mu_should(*(int *)keys[10001] == 10000);
	mu_should(*(int *)keys[10002] == 10005);
	mu_should(*(int *)keys[10003] == 10010);
	kv_delete(kv, after);
	kv_delete(kv, before);
	kv_delete(kv, penultimate);
	kv_delete(kv, anteoriginal);
	free(keys);

	keys = kv_keys(kv);
	mu_should(*(int *)keys[0] == 1);
	mu_should(*(int *)keys[1] == 2);
	mu_should(*(int *)keys[9998] == 9999);
	mu_should(*(int *)keys[9999] == 10000);

	int i = 0;
	while (keys[i]) {
		free(keys[i]);
		i += 1;
	}
	free(keys);
	mu_should(kv_reset(kv));
	mu_should(kv_destroy(kv));
}

MU_TEST(test_volume_descending) {
	hkv *kv = kv_create(fn_key_compare_int);
	/* grow a few times */
	clock_t b = clock();
	for (int i = 10000; i >= 1; i--) {
		int *j = malloc(sizeof(int));
		*j = i;
		kv_put(kv, j, j);
	}
	clock_t e = clock();
	printf("\ntime descending %lu\n", b-e);
	/* add after end (which is 10000) */
	int *after = malloc(sizeof(int));
	*after = 10010;
	kv_put(kv, after, after);
	/* and in front of */
	int *before = malloc(sizeof(int));
	*before = -10;
	kv_put(kv, before, before);
	/* between first two and between last two */
	int *penultimate = malloc(sizeof(int));
	*penultimate = 10005;
	kv_put(kv, penultimate, penultimate);
	mu_should(kv_count(kv) == 10003);
	int *anteoriginal = malloc(sizeof(int));
	*anteoriginal = 0;
	kv_put(kv, anteoriginal, anteoriginal);
	mu_should(kv_count(kv) == 10004);
	pkey *keys = kv_keys(kv);
	mu_should(*(int *)keys[0] == -10);
	mu_should(*(int *)keys[1] == 0);
	mu_should(*(int *)keys[2] == 1);
	mu_should(*(int *)keys[3] == 2);
	mu_should(*(int *)keys[10000] == 9999);
	mu_should(*(int *)keys[10001] == 10000);
	mu_should(*(int *)keys[10002] == 10005);
	mu_should(*(int *)keys[10003] == 10010);
	int i = 0;
	while (keys[i]) {
		free(keys[i]);
		i += 1;
	}
	free(keys);
	mu_should(10004 == kv_reset(kv));
	mu_should(kv_destroy(kv));
}

MU_TEST(test_volume_random) {
	hkv *kv = kv_create(fn_key_compare_int);
	/* grow a few times */
	clock_t b = clock();
	int i = 0;
	while (i < 10000) {
		int *j = malloc(sizeof(int));
		*j = random_between(1, 100000);
		if (!kv_get(kv, j)) {
			kv_put(kv, j, j);
			i += 1;
		}
	}
	clock_t e = clock();
	printf("\ntime random %lu\n", b-e);
	mu_should(kv_count(kv) == 10000);
	pkey *keys = kv_keys(kv);
	printf("\n");
	for (int i = 0; i < 10; i++)
		printf("%d %d\n", i, *(int *)keys[i]);
	printf("\n");
	for (int i = 5000; i < 5010; i++)
		printf("%d %d\n", i, *(int *)keys[i]);
	printf("\n");
	for (int i = 9990; i < 10000; i++)
		printf("%d %d\n", i, *(int *)keys[i]);
	printf("\n");
	/* add after end (which is 10000) */
	i = 0;
	while (keys[i]) {
		free(keys[i]);
		i += 1;
	}
	free(keys);
	mu_should(10000 == kv_reset(kv));
	mu_should(kv_destroy(kv));

}

MU_TEST(test_string_keys) {
	hkv *kv = load_str_keys();
	mu_should(kv_count(kv) == 6);

	/* key and value lists */
	pkey *keys = kv_keys(kv);
	mu_should(keys);
	pvalue *values = kv_values(kv);
	mu_should(values);

	/* are the keys that loaded strings? the printf will tell */
	printf("\n");
	for (int i = 0; keys[i]; i++)
		printf("%s %d\n", (char *)keys[i], *(int *)values[i]);
	free(values);
	free(keys);

	/* check known key:value pairs */
	pvalue value = kv_get(kv, "alpha");
	mu_should(*(int *)value == 0);
	value = kv_get(kv, "charlie");
	mu_should(*(int *)value == 17);

	mu_should(kv_reset(kv) == 6);
	kv_destroy(kv);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_load);
	MU_RUN_TEST(test_put);
	MU_RUN_TEST(test_delete);
	MU_RUN_TEST(test_keys);
	MU_RUN_TEST(test_values);
	MU_RUN_TEST(test_string_keys);
	MU_RUN_TEST(test_volume_ascending);
	MU_RUN_TEST(test_volume_descending);
	MU_RUN_TEST(test_volume_random);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitkv.c ends here */
