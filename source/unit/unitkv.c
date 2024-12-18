/* unitkv.c -- tests for the key:value store library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "minunit.h"
#include "txballoc.h"
#include "txbrand.h"
#include "txbone.h"


/*
 * intention revealing helper for wrapping.
 */

#define as_key(n) (void *)(long)(n)

/*
 * minunit setup and teardown.
 */

void
test_setup(void) {
	seed_random_generator(6803);
	/*      set_random_generator(RAND_RANDOM); */
	set_random_generator(RAND_DEFAULT);
	tsinitialize(50000, txballoc_f_errors, stderr);
}

void
test_teardown(void) {
	tsterminate();
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

one_block *
load_ints(void) {
	one_block *kv = make_one_keyed(keyval, integral, NULL);
	int i = 0;
	while (int_keyed[i][0] != -1) {
		insert(kv, as_key(int_keyed[i][0]), &int_keyed[i][1]);
		i += 1;
	}
	return kv;
}

one_block *
load_strs(void) {
	one_block *kv = make_one_keyed(keyval, integral, NULL);
	int i = 0;
	while (str_valued[i].value) {
		insert(kv, as_key(str_valued[i].key), str_valued[i].value);
		i += 1;
	}
	return kv;
}

one_block *
load_str_keys(void) {
	one_block *kv = make_one_keyed(keyval, integral, NULL);
	int i = 0;
	while (str_keyed[i].key) {
		insert(kv, str_keyed[i].key, &str_keyed[i].value);
		i += 1;
	}
	return kv;
}

/*
 * test_create
 *
 * create a new rational is_empty instance.
 */

MU_TEST(test_create) {
	one_block *kv = make_one_keyed(keyval, integral, NULL);
	mu_should(kv);
	mu_should(count(kv) == 0);
	mu_should(is_empty(kv));
	free_one(kv);
}

/*
 * test_load
 *
 * load from the int[][] dataset and verify.
 */

MU_TEST(test_load) {

	one_block *kv = load_ints();

	int i = 0;
	while (int_keyed[i][0] != -1) {
		bool r = update(kv, (void *)(long)int_keyed[i][0], &int_keyed[i][1]);
		fprintf(stderr, "load ints %d %s\n", int_keyed[i][0], r ? "updated" : "failed");
		i += 1;
	}
	mu_shouldnt(is_empty(kv));
	mu_should(count(kv) == i);

	one_block *kxs = keys(kv);
	one_block *vxs = values(kv);
	mu_should(kxs);
	mu_should(vxs);
	mu_should(count(kv) == count(kxs));
	mu_should(count(kxs)== count(vxs));
	fprintf(stderr, "keys\n");
	for (int i = 0; i < count(kxs); i++) {
		fprintf(stderr, "%d\n", (int)nth(kxs, i));
	}
	fprintf(stderr, "values\n");
	for (int i = 0; i < count(vxs); i++) {
		fprintf(stderr, "%d\n", *(int *)nth(vxs, i));
	}
	free_one(kxs);
	free_one(vxs);

	/* do we read back a known loaded row? */
	fprintf(stderr, "seeking key 1\n");
	int k = 1;
	mu_should(get(kv, (void *)(long)k));

	/* do we read back a known not to be loaded row? */
	k = 100;
	mu_shouldnt(get(kv, (void *)(long)k));

	/* fprintf(stderr, "purge\n"); */
	/* purge(kv); */
	/* fprintf(stderr, "purged\n"); */
	free_one(kv);
}

/*
 * test_update
 *
 * update an exixsting key's value.
 */

MU_TEST(test_update) {
	one_block *kv = load_ints();

	/* get a list of all the keys in the store */
	one_block *kxs = keys(kv);
	mu_should(kxs);
	one_block *vxs = values(kv);
	mu_should(vxs);
	/* print them and their associated values */
	printf("\n");
	int i = 0;
	for (i = 0; i < count(kxs); i++) {
		printf("%d %d %d\n", i, (int)nth(kxs, i), *(int *)get(kv, (void *)nth(kxs, i)));
		mu_should(*(int *)get(kv, (void *)nth(kxs, i)) == *(int *)nth(vxs, i));
		iterate(kxs, &i);
	}
	free_one(kxs);
	free_one(vxs);

	/* we expect 10 pairs, and keys 4 and 5
	 * have values of 4 and 5 */
	mu_should(count(kv) == 10);
	int j = 4;
	mu_should(*(int *)get(kv, as_key(j)) == 4);
	j = 5;
	mu_should(*(int *)get(kv, as_key(j)) == 5);

	/* change 5:5 to 5:8 */
	int v = 8;
	update(kv, as_key(j), &v);
	mu_should(*(int *)get(kv, as_key(j)) == 8);

	/* but 4:4 should still be 4:4 */
	j = 4;
	mu_should(*(int *)get(kv, as_key(j)) == 4);

	//      purge(kv);
	free_one(kv);
}

/*
 * test_delete
 *
 * delete specific pairs.
 */

MU_TEST(test_delete) {
	one_block *kv = load_ints();

	/* establish that pairs 2, 8, and 9 exist */
	mu_should(count(kv) == 10);
	mu_should(get(kv, as_key(8)));
	mu_should(get(kv, as_key(2)));
	mu_should(get(kv, as_key(9)));

	/* and that key 101 does not */
	mu_shouldnt(get(kv, as_key(101)));

	/* delete 8 */
	mu_should(delete (kv, as_key(8)));
	mu_should(count(kv) == 9);

	/* are 2 and 9 still there */
	mu_should(get(kv, as_key(2)));
	mu_should(get(kv, as_key(9)));

	/* and 8 is gone */
	mu_shouldnt(get(kv, as_key(8)));

	free_one(kv);
}

MU_TEST(test_volume_ascending) {
	one_block *kv = make_one_keyed(keyval, integral, NULL);

	/* load entries in ascending sequence
	 * key is int, value is *int */
	clock_t b = clock();
	for (int i = 1; i <= 10000; i++) {
		int *j = malloc(sizeof(int));
		*j = i;
		insert(kv, as_key(*j), j);
	}
	clock_t e = clock();
	printf("\ntime ascending %lu\n", b-e);
	mu_should(count(kv) == 10000);

	/* add after end (which is 10000) */
	int *after = malloc(sizeof(int));
	*after = 10010;
	insert(kv, as_key(*after), after);

	/* and in front of start (which is 0) */
	int *before = malloc(sizeof(int));
	*before = -10;
	insert(kv, as_key(*before), before);

	mu_should(count(kv) == 10002);

	/* between first two and between last two */
	int *penultimate = malloc(sizeof(int));
	*penultimate = 10005;
	insert(kv, as_key(*penultimate), penultimate);

	mu_should(count(kv) == 10003);

	int *anteoriginal = malloc(sizeof(int));
	*anteoriginal = 0;
	insert(kv, as_key(*anteoriginal), anteoriginal);

	mu_should(count(kv) == 10004);

	one_block *vl = values(kv);
	mu_should(*(int *)nth(vl, 0) == -10);
	mu_should(*(int *)nth(vl, 1) == 0);
	mu_should(*(int *)nth(vl, 2) == 1);
	mu_should(*(int *)nth(vl, 3) == 2);
	mu_should(*(int *)nth(vl, 10000) == 9999);
	mu_should(*(int *)nth(vl, 10001) == 10000);
	mu_should(*(int *)nth(vl, 10002) == 10005);
	mu_should(*(int *)nth(vl, 10003) == 10010);

	delete (kv, as_key(*after));
	delete (kv, as_key(*before));
	delete (kv, as_key(*penultimate));
	delete (kv, as_key(*anteoriginal));

	mu_should(count(kv) == 10000);

	free_one(vl);
	vl = values(kv);

	mu_should(*(int *)nth(vl, 0) == 1);
	mu_should(*(int *)nth(vl, 1) == 2);
	mu_should(*(int *)nth(vl, 9998) == 9999);
	mu_should(*(int *)nth(vl, 9999) == 10000);

	int i = 0;
	while (nth(vl, i)) {
		free((int *)nth(vl, i));
		i += 1;
	}
	free_one(vl);

	free_one(kv);
}

MU_TEST(test_volume_descending) {
	one_block *kv = make_one_keyed(keyval, integral, NULL);
	/* grow a few times */
	clock_t b = clock();
	for (int i = 10000; i >= 1; i--) {
		int *j = malloc(sizeof(int));
		*j = i;
		insert(kv, as_key(*j), j);
	}
	mu_should(count(kv) == 10000);
	clock_t e = clock();
	printf("\ntime descending %lu\n", b-e);
	/* add after end (which is 10000) */
	int *after = malloc(sizeof(int));
	*after = 10010;
	insert(kv, as_key(*after), after);
	/* and in front of */
	int *before = malloc(sizeof(int));
	*before = -10;
	insert(kv, as_key(*before), before);
	/* between first two and between last two */
	int *penultimate = malloc(sizeof(int));
	*penultimate = 10005;
	insert(kv, as_key(*penultimate), penultimate);
	mu_should(count(kv) == 10003);
	int *anteoriginal = malloc(sizeof(int));
	*anteoriginal = 0;
	insert(kv, as_key(*anteoriginal), anteoriginal);
	mu_should(count(kv) == 10004);
	one_block *vl = values(kv);
	mu_should(*(int *)nth(vl, 0) == -10);
	mu_should(*(int *)nth(vl, 1) == 0);
	mu_should(*(int *)nth(vl, 2) == 1);
	mu_should(*(int *)nth(vl, 3) == 2);
	mu_should(*(int *)nth(vl, 10000) == 9999);
	mu_should(*(int *)nth(vl, 10001) == 10000);
	mu_should(*(int *)nth(vl, 10002) == 10005);
	mu_should(*(int *)nth(vl, 10003) == 10010);
	int i = 0;
	while (nth(vl, i)) {
		free((int *)nth(vl, i));
		i += 1;
	}
	free_one(vl);
	//      mu_should(10004 == purge(kv));
	free_one(kv);
}

MU_TEST(test_volume_random) {
	one_block *kv = make_one_keyed(keyval, integral, NULL);
	/* grow a few times */
	clock_t b = clock();
	int i = 0;
	while (i < 10000) {
		int *j = malloc(sizeof(int));
		*j = random_between(1, 100000);
		if (!get(kv, as_key(*j))) {
			insert(kv, as_key(*j), j);
			i += 1;
		}
	}
	clock_t e = clock();
	printf("\ntime random %lu\n", b-e);
	mu_should(count(kv) == 10000);
	one_block *vl = values(kv);
	printf("\n");
	for (int i = 0; i < 10; i++)
		printf("%d %d\n", i, *(int *)nth(vl, i));
	printf("\n");
	for (int i = 5000; i < 5010; i++)
		printf("%d %d\n", i, *(int *)nth(vl, i));
	printf("\n");
	for (int i = 9990; i < 10000; i++)
		printf("%d %d\n", i, *(int *)nth(vl, i));
	printf("\n");
	/* add after end (which is 10000) */
	i = 0;
	while (nth(vl, i)) {
		free((int *)nth(vl, i));
		i += 1;
	}
	free_one(vl);
	//      mu_should(10000 == purge(kv));
	free_one(kv);

}

MU_TEST(test_string_keys) {
	one_block *kv = load_str_keys();
	mu_should(count(kv) == 6);

	/* key and value lists */
	one_block *kl = keys(kv);
	mu_should(kl);
	one_block *vl = values(kv);
	mu_should(vl);

	/* check known key:vl pairs */
	void *v = get(kv, "alpha");
	mu_should(*(int *)v == 0);
	v = get(kv, "charlie");
	mu_should(*(int *)v == 17);
	free_one(kl);
	free_one(vl);
	free_one(kv);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_load);
	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_update);
	MU_RUN_TEST(test_delete);
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
