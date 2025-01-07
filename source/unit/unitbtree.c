/* unitbtree.c -- consolidated unit tests for scapegoat tree */

/* system includes here */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* the test framework */
#include "minunit.h"

/* other library functions */
#include "../inc/alloc.h"
#include "../inc/rand.h"
#include "../inc/one.h"

/* these are not exposed in txbone.h */

one_tree *
btree_rebalance(
	one_tree *self
);

int
btree_height_for_key(
	one_tree *self,
	void *key
);

/**
 * global variables go here if they are needed.
 */

/*
 * main() drops these here for anyone who wants them.
 */

static
int argc;

static
char **argv;

/**
 * utility code and common data
 */

/*
 * intention revealing helper for wrapping.
 */

#define as_key(n) (void *)(long)(n)

/*
 * small keysets for minimal but traceable testing.
 */

int ascending_keys[] = {
	10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110,
	120, 130, 140, 150, 160, 170, 180, 190, 200,
	210, 220, 230, 240, 250, 260, 270, 280, 290,
	-1
};

int descending_keys[] = {
	190, 180, 170, 160, 150, 140, 130, 120, 110, 100,
	90, 80, 70, 60, 50, 40, 30, 20, 10, -1
};

/* 10->200 random shuffle */
int randomized_keys[] = {
	170, 200, 190, 30, 20, 50, 140, 80, 120, 100, 150,
	70, 40, 10, 90, 110, 160, 60, 180, 130, -1
};

/**
 * insert an array of integer keys into a tree.
 */

int
load_keys(one_block *t, int *keys) {
	int i = 0;
	while (keys[i] > 0) {
		insert(t, as_key(keys[i]), &keys[i]);
		i += 1;
	}
	return i;
}

/**
 * create a random string of lower case letters of length n.
 *
 * this is useful for recreatable volume testing if you seed_random()
 * during test_setup() and run with the default random number
 * generator.
 *
 * the strings are written into n+1 sized malloced blocks, you should
 * free them!
 */

char *
random_strings(int n) {
	char *s = malloc(n+1);
	memset(s, 0, n+1);;
	for (int i = 0; i < n; i++)
		s[i] = random_lower();
	return s;
}

/**
 * integer key test tree. it has a right lean before balancing.
 */

one_block *
small_right_leaning_integral(void) {
	one_block *t = make_one_keyed(keyval, integral, NULL);
	insert(t, as_key(5), "5");
	insert(t, as_key(3), "3");
	insert(t, as_key(7), "7");
	insert(t, as_key(4), "4");
	insert(t, as_key(6), "6");
	insert(t, as_key(8), "8");
	insert(t, as_key(9), "9");
	insert(t, as_key(10), "10");
	insert(t, as_key(11), "11");
	insert(t, as_key(12), "12");
	insert(t, as_key(13), "13");
	insert(t, as_key(14), "14");
	insert(t, as_key(15), "15");
	insert(t, as_key(1), "1");
	insert(t, as_key(2), "2");
	return t;
}

/**
 * string key test tree. it zig zags a bit.
 */

one_block *
small_zag_left_string(void) {
	one_block *t = make_one_keyed(keyval, string, NULL);
	insert(t, "5", "5");
	insert(t, "3", "3");
	insert(t, "7", "7");
	insert(t, "4", "4");
	insert(t, "6", "6");
	insert(t, "8", "8");
	insert(t, "9", "9");
	insert(t, "15", "15");
	insert(t, "14", "14");
	insert(t, "13", "13");
	insert(t, "12", "12");
	insert(t, "11", "11");
	insert(t, "10", "10");
	insert(t, "1", "1");
	insert(t, "2", "2");
	return t;
}

/**
 * custom key test tree. kind of random.
 */

int
custom_cmp(
	const void *left,
	const void *right
) {
	/* just reverse integer compares */
	int res = (long)left - (long)right;
	return -res;
}

one_block *
small_custom_tree(void) {
	one_block *t = make_one_keyed(keyval, custom, custom_cmp);

	insert(t, as_key(50), "root");
	insert(t, as_key(40), "actually right");
	insert(t, as_key(60), "actually left");
	for (int i = 0; i < 40; i += 5) {
		insert(t, as_key(i), "right");
		insert(t, as_key(100-i), "left");
	}
	return t;
}

/**
 * common analysis functions.
 */

int depths[50] = {0};

void
clear_depths(void) {
	for (int i = 0; i < 50; i++)
		depths[i] = 0;
}

void
print_depths(void) {
	fprintf(stderr, "\ndepth:count\n");
	int total = 0;
	for (int i = 0; i < 50; i++) {
		if (depths[i] == 0) continue;
		total += depths[i];
		fprintf(stderr, "%d:%d\n", i+1, depths[i]);
	}
	fprintf(stderr, "nodes:%d\n", total);
}

bool
in_order_cb_list(void *key, void *value, void *context, one_tree *self) {
	fprintf(stderr, "key: %6ld   height: %6d\n", (long)key,
		1+btree_height_for_key(self, key));
	return true;
}

bool
in_order_cb_depth(void *key, void *value, void *context, one_tree *self) {
	int d = btree_height_for_key(self, key);
	if (d > 49) d = 49;
	depths[d] += 1;
	return true;
}

/**
 * the bulk of the unit tests. i do both white and black box testing.
 *
 * each test contains one or more assertions. while minunit provides
 * many assertion macros, i tend to use mu_should() and mu_shouldnt().
 *
 * i probably 'should' and 'shouldnt' too often, but using them
 * reminds me to make sure functions that should return a value do so.
 * they also establish that the setup before a failing assertion is as
 * expected.
 *
 * these are the main tests. controlling which ones are run is done
 * in function test_suite. i end up reordering the list of tests
 * and inserting an early return after the last test i'm interested
 * in.
 */

/**
 * use this test for breakpointing. copy a failure here and dive
 * right in.
 */

MU_TEST(test_wip) {
	one_block *t = NULL;
	/* one_block *xs = NULL; */

	/* are deleted terminal nodes really removed? */

	t = make_one_keyed(keyval, integral, NULL);
	insert(t, as_key(50), "root");
	insert(t, as_key(40), "left");
	insert(t, as_key(45), "not as left");
	insert(t, as_key(60), "right");
	insert(t, as_key(30), "lefter");
	insert(t, as_key(70), "righter");

	delete (t, as_key(30));
	mu_shouldnt(exists(t, as_key(30)));
	mu_should(exists(t, as_key(50)));
	mu_should(exists(t, as_key(40)));

	free_one(t);

}

/**
 * rebalance after deletes
 */

MU_TEST(test_rebalance_deleted_root) {
	one_block *t = NULL;

	t = make_one_keyed(keyval, integral, NULL);
	insert(t, as_key(50), "root");
	insert(t, as_key(40), "left");
	insert(t, as_key(60), "right");

	delete (t, as_key(50));
	mu_shouldnt(exists(t, as_key(50)));

	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_list);
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();
	free_one(t);

	t = make_one_keyed(keyval, integral, NULL);
	insert(t, as_key(50), "root");
	insert(t, as_key(40), "left");
	insert(t, as_key(60), "right");
	insert(t, as_key(30), "lefter");
	insert(t, as_key(70), "righter");

	/* remove root, rebalance of two nodes */
	delete (t, as_key(50));
	mu_shouldnt(exists(t, as_key(50)));
	mu_should(get(t, as_key(40)));
	mu_shouldnt(get(t, as_key(50)));

	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_list);
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();

	free_one(t);

}

MU_TEST(test_simple_rebalance) {
	one_block *t = NULL;

	t = small_right_leaning_integral();
	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_list);
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();
	btree_rebalance(&t->u.kvl);
	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_list);
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();
	free_one(t);
}

/**
 * traversals after delete
 */

MU_TEST(test_traversal_deletes) {
	one_block *t = NULL;

	/* keys 1-15, leaning right */
	t = small_right_leaning_integral();
	mu_should(count(t) == 15);
	delete (t, as_key(5));
	mu_should(count(t) == 14);

	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();

	delete (t, as_key(10));
	mu_should(count(t) == 13);

	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();

	free_one(t);
}

/**
 * the various delete cases
 */

MU_TEST(test_delete_cases) {
	one_block *t = NULL;

	/* case 1, delete root with no children */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "root", "root");
	mu_should(exists(t, "root") && count(t) == 1);
	delete (t, "root");
	mu_should(is_empty(t) && !exists(t, "root") && count(t) == 0);
	free_one(t);

	/* case 2a, delete root right child only */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "c", "groot");
	insert(t, "d", "d > c");
	delete (t, "c");
	mu_should(count(t) == 1 && !exists(t, "c") && exists(t, "d"));
	free_one(t);

	/* case 2b, delete root left child only */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "c", "groot");
	insert(t, "b", "b < c");
	delete (t, "c");
	mu_should(count(t) == 1 &&
		!exists(t, "c")
		&& exists(t, "b"));
	free_one(t);

	/* case 3, root with 2 children */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "c", "groot");
	insert(t, "b", "b < c");
	insert(t, "d", "d > c");
	delete (t, "c");
	mu_should(!is_empty(t) && !exists(t, "c") && count(t) == 2 &&
		exists(t, "b") && exists(t, "d"));
	free_one(t);

	/* case 4, delete node is an end leaf (neither right or left
	 * children) */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "c", "groot");
	insert(t, "b", "b < c");
	insert(t, "a", "a < c < b"); /* terminal no children */
	insert(t, "d", "d > c");
	delete (t, "a");
	mu_should(!is_empty(t) && !exists(t, "a") && count(t) == 3 &&
		exists(t, "b") && exists(t, "d") && exists(t, "c"));
	free_one(t);

	/* case 5a, leaf with only right child */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "c", "groot");
	insert(t, "b", "b < c");
	insert(t, "a", "a < c < b");
	insert(t, "d", "d > c"); /* leafy with only a right child */
	insert(t, "e", "e > d > c");
	delete (t, "d");
	mu_should(!is_empty(t) && !exists(t, "d") && count(t) == 4 &&
		exists(t, "b") && exists(t, "e") && exists(t, "c") && exists(t, "a"));
	free_one(t);

	/* case 5b, leaf with only left child */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "c", "groot");
	insert(t, "b", "b < c"); /* leafy with only a right child */
	insert(t, "a", "a < c < b");
	insert(t, "d", "d > c");
	insert(t, "e", "e > d > c");
	delete (t, "b");
	mu_should(!is_empty(t) && !exists(t, "b") && count(t) == 4 &&
		exists(t, "a") && exists(t, "c") && exists(t, "d") && exists(t, "e"));
	free_one(t);

	/* case 6, in the middle with two children */
	t = make_one_keyed(keyval, string, NULL);
	insert(t, "n", "groot");
	insert(t, "g", "g < n");
	insert(t, "a", "a < g < n"); /* left child of g */
	insert(t, "k", "g < k");
	delete (t, "g");
	mu_should(!is_empty(t) && !exists(t, "g") && count(t) == 3 &&
		exists(t, "n") && exists(t, "a") && exists(t, "k"));
	free_one(t);

}

/**
 * we wrap most everything in here in should or shouldnt since we're
 * testing the full api. while it shouldn't matter, we run the tests
 * for all three key types.
 */

MU_TEST(test_api_integral) {

	one_block *t = NULL;

	t = small_right_leaning_integral();
	mu_should(t);

	/* check that load worked pretty much as expected, keys 1-15 */
	mu_shouldnt(is_empty(t));
	mu_should(count(t) == 15);
	mu_should(exists(t, as_key(1)));
	mu_should(exists(t, as_key(15)));

	/* reading returns expected value */
	mu_should(strcmp(get(t, as_key(5)), "5") == 0);

	/* insert of dupicate will fail */
	mu_shouldnt(insert(t, as_key(9), "nine"));
	mu_shouldnt(strcmp(get(t, as_key(9)), "nine") == 0);

	/* update of existing will work */
	mu_should(update(t, as_key(9), "NINE"));
	mu_should(strcmp(get(t, as_key(9)), "NINE") == 0);

	/* update of non existing will fail */
	mu_shouldnt(exists(t, as_key(20)));
	mu_shouldnt(update(t, as_key(20), "20"));
	mu_shouldnt(exists(t, as_key(20)));

	/* and that update doesn't seem to have damaged the tree */
	mu_shouldnt(is_empty(t));
	mu_should(count(t) == 15);
	mu_should(exists(t, as_key(1)));
	mu_should(exists(t, as_key(15)));

	/* and now insert a new high value key */
	mu_should(insert(t, as_key(20), "20"));
	mu_should(exists(t, as_key(20)));

	/* delete from middle and ends */
	mu_should(count(t) == 16);
	mu_should(delete (t, as_key(8)));
	mu_should(count(t) == 15);
	mu_should(exists(t, as_key(1)));
	mu_should(delete (t, as_key(1)));
	mu_should(count(t) == 14);
	fprintf(stderr, "\n 3 %s\n", (char *)get(t, as_key(3)));
	fprintf(stderr, "\n 2 %s\n", (char *)get(t, as_key(2)));
	fprintf(stderr, "\n 1 %s\n", (char *)get(t, as_key(1)));
	mu_should(exists(t, as_key(2)));
	mu_should(delete (t, as_key(15)));
	mu_should(count(t) == 13);
	mu_should(exists(t, as_key(14)));

	/* and done */
	free_one(t);
}

MU_TEST(test_api_string) {
	one_block *t = NULL;

	t = small_zag_left_string();
	mu_should(t);

	/* check that load worked pretty much as expected, keys 1-15 */
	mu_shouldnt(is_empty(t));
	mu_should(count(t) == 15);
	mu_should(exists(t, "1"));
	mu_should(exists(t, "9"));

	/* reading returns expected value */
	mu_should(strcmp(get(t, "5"), "5") == 0);

	/* insert of dupicate will fail */
	mu_shouldnt(insert(t, "9", "nine"));
	mu_shouldnt(strcmp(get(t, "9"), "nine") == 0);

	/* update of existing will work */
	mu_should(update(t, "9", "NINE"));
	mu_should(strcmp(get(t, "9"), "NINE") == 0);

	/* update of non existing will fail */
	mu_shouldnt(exists(t, "20"));
	mu_shouldnt(update(t, "20", "20"));
	mu_shouldnt(exists(t, "20"));

	/* and that update doesn't seem to have damaged the tree */
	mu_shouldnt(is_empty(t));
	mu_should(count(t) == 15);
	mu_should(exists(t, "1"));
	mu_should(exists(t, "9"));

	/* and now insert a new high value key */
	mu_should(insert(t, "99", "99"));
	mu_should(exists(t, "99"));

	/* and done */
	free_one(t);

}

/**
 * test_volume
 *
 * hit it with 50000 random items, check depth distribution and
 * counts.
 */

MU_TEST(test_volume) {
	one_block *t = make_one_keyed(keyval, integral, NULL);
	fprintf(stderr, "loading up to 50000 items\n");
	int added = 0;
	for (int i = 0; i < 50000; i++) {
		if (insert(t, as_key(random_between(1, 999999)), "random"))
			added += 1;
	}
	/* mu_shouldnt(true); */
	fprintf(stderr, "added %d items\n", added);
	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();
	fprintf(stderr, "rebalancing\n");
	btree_rebalance(&t->u.kvl);
	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();

	int target = added * 2 / 10;
	fprintf(stderr, "deleting ~20%% of the rows...%d\n", target);
	int deleted = 0;
	while (deleted < target) {
		long k = random_between(1, 999999);
		if (exists(t, (void *)k)) {
			delete (t, (void *)k);
			deleted += 1;
		}
	}

	fprintf(stderr, "rebalancing\n");
	btree_rebalance(&t->u.kvl);
	clear_depths();
	in_order_keyed(t, NULL, in_order_cb_depth);
	print_depths();

	free_one(t);
}

/**
 * these are run before and after every test function above.
 */

static
void
test_setup(void) {
	seed_random_generator(6803);
	//tsinitialize(125000, txballoc_f_errors, stderr);
}

static
void
test_teardown(void) {
	//tsterminate();
}

/**
 * a test suite invokes one or more tests. while it is possible to
 * have multiple suites, i don't.
 */

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	MU_RUN_TEST(test_api_integral);
	MU_RUN_TEST(test_api_string);

	MU_RUN_TEST(test_delete_cases);

	MU_RUN_TEST(test_simple_rebalance);
	MU_RUN_TEST(test_rebalance_deleted_root);

	MU_RUN_TEST(test_traversal_deletes);

	MU_RUN_TEST(test_volume);

	MU_RUN_TEST(test_wip);

}

/********************************************************************
 * start me up -- save command line information for access elsewhere
 * and run the test suite.
 ********************************************************************/

int
main(int aargc, char *aargv[]) {

	argc = aargc;
	argv = aargv;

	MU_RUN_SUITE(test_suite);

	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitbal.c ends here */
