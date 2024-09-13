/* unitkv.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "minunit.h"

#include "../inc/rand.h"
#include "../inc/kv.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

void
test_setup(void) {
}

void
test_teardown(void) {
}

/*
 * basic functionality tests.
 */

int
fn_key_compare_int(void *a, void *b) {
	return *(int *)a - *(int *)b;
}

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

MU_TEST(test_create) {
	kvcb *kv = kv_create(fn_key_compare_int);
	mu_should(kv);
	mu_should(kv_count(kv) == 0);
	mu_should(kv_empty(kv));
	kv_destroy(kv);
}

MU_TEST(test_load) {
	kvcb *kv = kv_create(fn_key_compare_int);
	int i = 0;
	while (int_keyed[i][0] != -1) {
		kv_put(kv, &int_keyed[i][0], &int_keyed[i][1]);
		i += 1;
	}
	mu_should(!kv_empty(kv));
	mu_should(kv_count(kv) == 10);
	int k = 1;
	mu_should(kv_exists(kv, &k));
	k = 100;
	mu_shouldnt(kv_exists(kv, &k));
	kv_destroy(kv);
}

kvcb *
load_ints(int init, int grow) {
	kvcb *kv = kv_create(fn_key_compare_int);
	int i = 0;
	while (int_keyed[i][0] != -1) {
		kv_put(kv, &int_keyed[i][0], &int_keyed[i][1]);
		i += 1;
	}
	return kv;
}

kvcb *
load_strs(int init, int grow) {
	kvcb *kv = kv_create(fn_key_compare_int);
	int i = 0;
	while (str_valued[i].value) {
		kv_put(kv, &str_valued[i].key, str_valued[i].value);
		i += 1;
	}
	return kv;
}

MU_TEST(test_put) {
	int k = 5;
	int v = 8;
	kvcb *kv = load_ints(10, 5);
	mu_should(kv_count(kv) == 10);
	mu_should(kv_exists(kv, &k));
	int *pv = kv_get(kv, &k);
	mu_should(*pv);
	mu_should(*pv == 5);
	kv_put(kv, &k, &v);
	k = 2;
	pv = kv_get(kv, &k);
	mu_should(*pv == 2);
	k = 1073;
	pv = kv_get(kv, &k);
	mu_shouldnt(pv);
	kv_destroy(kv);
}

MU_TEST(test_delete) {
	kvcb *kv = load_ints(10, 5);
	mu_should(kv_count(kv) == 10);
	int k = 8;
	mu_should(kv_exists(kv, &k));
	k = 2;
	mu_should(kv_exists(kv, &k));
	k = 8;
	bool f = kv_delete(kv, &k);
	mu_should(f);
	k = 2;
	mu_should(kv_exists(kv, &k));
	k = 8;
	mu_shouldnt(kv_exists(kv, &k));
	mu_should(kv_count(kv) == 9);
	kv_destroy(kv);
}

MU_TEST(test_keys) {
	kvcb *kv = load_ints(10, 5);
	mu_should(kv_count(kv) == 10);
	int **keys = kv_keys(kv);
	mu_should(keys);
	printf("\n");
	for (int i = 0; keys[i]; i++)
		printf("%d\n", *keys[i]);
	free(keys);
	kv_destroy(kv);
}

MU_TEST(test_values) {
	kvcb *kv = load_strs(10, 5);
	mu_should(kv_count(kv) == 10);
	char **values = kv_values(kv);
	mu_should(values);
	printf("\n");
	for (int i = 0; values[i]; i++)
		printf("%s\n", values[i]);
	free(values);
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

	MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

	/* run your tests here */

	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_load);
	MU_RUN_TEST(test_put);
	MU_RUN_TEST(test_delete);
	MU_RUN_TEST(test_keys);
	MU_RUN_TEST(test_values);

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
