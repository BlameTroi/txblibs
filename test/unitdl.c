/*  unittest.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#undef NDEBUG
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "../inc/misc.h"
#include "../inc/dl.h"
#include "../inc/str.h"
#include "../inc/rand.h"

/*
 * tests for  txbdl -- doubly linked list
 */

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

static dlcb * test_dl = NULL;

/*
 * test_setup.
 *
 * after seeding rand(), create a list for testing. list payload items
 * are malloced strings "9999 bogus", where the digits run from 10 to
 * 990 by 10s to get 99 items.
 *
 * note that the payloads are malloced via strdup after the
 * snprintf. the buffer is on the local stack. even if the payload
 * could be accessed reliably from later code, they would all point to
 * single buffer with whatever is in memory at that point.
 *
 * payloads are references, remember this.
 */

static
void
test_setup(void) {

	/* reproducible results if doing something 'random' */
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);

	/* load up a list with 100 items */
	char buffer[100];
	memset(buffer, 0, 100);
	test_dl = dl_create();
	assert(test_dl &&
		"error creating test data linked list");
	for (int i = 10; i < 1000; i += 10) {
		snprintf(buffer, 99, "%04d bogus", i);
		dl_insert_last(test_dl, strdup(buffer));
	}
}

/*
 * test_teardown.
 *
 * delete and free payloads of any items still on the list after a
 * test has run.
 *
 * with sanatize-address on, if you drop a payload as the address of a
 * literal, the free will fail.
 */

static
void
test_teardown(void) {
	if (!test_dl)
		return;
	dlnode *dn = NULL;
	while (dn = dl_get_first(test_dl), dn) {
		free(dn->payload);
		dl_delete(test_dl, dn);
	}
	dl_destroy(test_dl);
	test_dl = NULL;
}

/*
 * test_create.
 *
 * create a new list instance and confirm that it arrives in the
 * proper starting state.
 */

MU_TEST(test_create) {
	dlcb *dl = dl_create();
	mu_should(dl);
	mu_should(dl_empty(dl));
	mu_should(dl_count(dl) == 0);
	mu_should(dl_destroy(dl));
}

/*
 * test_insert_ends.
 *
 * test adding a single item in an empty list using first and last
 * positions. like a queue or stack would.
 *
 * note that this doesn't leak payload storage since the payloads are
 * constants.
 */

MU_TEST(test_insert_ends) {

	/* insert these strings last if index is even (0, 2, ...) or
	 * first if index is odd (1, 3, ...). checking the order is
	 * a way to validate that the data made it into the list and
	 * that the links were updated correctly in each direction. */

	char *items[] = { "1", "2", "3", "4", "5", "6", "7", NULL };
	char ordering[] = { '6', '4', '2', '1', '3', '5', '7', '\0' };

	dlcb *dl = dl_create();
	dlnode *dn = NULL;
	int i = 0;

	/* add the test items alternating from add last to add first */
	i = 0;
	while (items[i]) {
		if (is_even(i))
			mu_should(dl_insert_last(dl, items[i]));
		else
			mu_should(dl_insert_first(dl, items[i]));
		i += 1;
		mu_should(dl_count(dl) == i);
	}

	/* the expected ordering forward is 6421357 */
	i = 0;
	while (ordering[i]) {
		dn = dl_get_first(dl);
		mu_should(((char *)dn->payload)[0] == ordering[i]);
		dl_delete(dl, dn);
		i += 1;
	}

	mu_should(dl_count(dl) == 0);
	mu_should(dl_empty(dl));

	/* the expected ordering forward is 6421357 but we're going to
	 * check reading backwards through the list. re-add the items
	 * to the list as above. */

	i = 0;
	while (items[i]) {
		if (is_even(i))
			mu_should(dl_insert_last(dl, items[i]));
		else
			mu_should(dl_insert_first(dl, items[i]));
		i += 1;
		mu_should(dl_count(dl) == i);
	}

	/* the expected ordering forward is 6421357
	 * but we're going to check backwards. */

	/* find end of ordering check */
	i = 0;
	while (ordering[i])
		i += 1;

	/* recheck from the last node and backing through the
	 * ordering array. */
	while (i > 0) {
		i -= 1;
		dn = dl_get_last(dl);
		mu_should(((char *)dn->payload)[0] == ordering[i]);
		dl_delete(dl, dn);
	}

	mu_should(dl_count(dl) == 0);
	mu_should(dl_empty(dl));

	/* all good */

	dl_destroy(dl);
}

/*
 * test_insert_after.
 *
 * check that links are updated correctly when inserting after the
 * first and last items in the list. then insert some items in the
 * middle of the list and verify that they are correctly linked.
 */

MU_TEST(test_insert_after) {
	dlcb *dl = NULL;
	dlnode *dn = NULL;
	void *payload = NULL;

	/* in an empty list do the inserts work as expected? after the
	 * inserts, traverse the list from first to last and then last
	 * to first, checking that the payloads are as expected.
	 *
	 * this pattern of validation will repeat throughout these
	 * unit tests. */

	/* insert after head of single item list */
	dl = dl_create();
	payload = strdup("first");
	dn = dl_insert_first(dl, payload);
	mu_should(dn);
	dn = dl_get_first(dl);
	payload = strdup("inserted after first");
	dn = dl_insert_after(dl, dn, payload);
	mu_should(dn);
	mu_should(dl_count(dl) == 2);

	/* order should be first, inserted after first */
	dn = dl_get_first(dl);
	mu_should(equal_string(dn->payload, "first"));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string(dn->payload, "inserted after first"));

	/* while we're at it, check that an error is reported when we
	 * reach the end of the list. */
	dn = dl_get_next(dl, dn);
	mu_shouldnt(dn);
	mu_should(dl_get_error(dl));

	/* check from the tail */
	dn = dl_get_last(dl);
	mu_should(equal_string(dn->payload, "inserted after first"));
	dn = dl_get_previous(dl, dn);
	mu_should(equal_string(dn->payload, "first"));

	/* again, should get an error trying to read past head */
	dn = dl_get_previous(dl, dn);
	mu_shouldnt(dn);
	mu_should(dl_get_error(dl));

	/* clean up before switching to the preloaded list */
	while (dn = dl_get_first(dl), dn) {
		free(dn->payload);
		dl_delete(dl, dn);
	}
	dl_destroy(dl);

	/* insert after the head of the list */
	dl = test_dl;
	dn = dl_get_first(dl);
	payload = strdup("inserted after first");
	dn = dl_insert_after(dl, dn, payload);
	mu_should(dn);

	/* first three should be 10, new, 20 */
	dn = dl_get_first(dl);
	mu_should(equal_string("0010 bogus", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("inserted after first", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("0020 bogus", dn->payload));

	/* and now after the end */
	dn = dl_get_last(dl);
	payload = strdup("inserted after last");
	dn = dl_insert_after(dl, dn, payload);
	mu_should(dn);

	/* last three should be new, 990, 980 */
	dn = dl_get_last(dl);
	mu_should(equal_string("inserted after last", dn->payload));
	dn = dl_get_previous(dl, dn);
	mu_should(equal_string("0990 bogus", dn->payload));
	dn = dl_get_previous(dl, dn);
	mu_should(equal_string("0980 bogus", dn->payload));

	/* now read forward to end, end should be detected. */
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("0990 bogus", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("inserted after last", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_shouldnt(dn);
	mu_should(dl_get_error(dl));

	/* count should be 99 + 2 = 101 */
	mu_should(dl_count(dl) == 99 + 2);

	/* find 0500 in the list */
	dn = dl_get_first(dl);
	while (dn) {
		if (equal_string("0500 bogus", dn->payload))
			break;
		dn = dl_get_next(dl, dn);
	}
	mu_should(dn);

	/* insert after */
	payload = strdup("inserted after 0500");
	dn = dl_insert_after(dl, dn, payload);

	/* find it forward */
	dn = dl_get_first(dl);
	while (dn) {
		if (equal_string("0500 bogus", dn->payload))
			break;
		dn = dl_get_next(dl, dn);
	}
	mu_should(dn);

	dn = dl_get_next(dl, dn);
	mu_should(equal_string("inserted after 0500", dn->payload));

	/* and now find it backward */
	dn = dl_get_last(dl);
	while (dn) {
		if (equal_string("inserted after 0500", dn->payload))
			break;
		dn = dl_get_previous(dl, dn);
	}
	mu_should(dn);

	dn = dl_get_previous(dl, dn);
	mu_should(equal_string("0500 bogus", dn->payload));

	/* lastly check the count */
	mu_should(dl_count(dl) == 99 + 2 + 1);
}

/*
 * test_insert_before.
 *
 * check that links are updated correctly when inserting before the
 * first and last items in the list. then insert some items in the
 * middle of the list and verify that they are correctly linked.
 *
 * these tests parallel test_insert_after.
 */

MU_TEST(test_insert_before) {
	dlcb *dl = NULL;
	dlnode *dn = NULL;
	void *payload = NULL;

	/* in an empty list do the inserts work as expected? after the
	 * inserts, traverse the list from first to last and then last
	 * to first, checking that the payloads are as expected.
	 *
	 * this pattern of validation will repeat throughout these
	 * unit tests. */

	/* insert after head of single item list */
	dl = dl_create();
	payload = strdup("first");
	dn = dl_insert_first(dl, payload);
	mu_should(dn);
	dn = dl_get_first(dl);
	payload = strdup("inserted before first");
	dn = dl_insert_before(dl, dn, payload);
	mu_should(dn);
	mu_should(dl_count(dl) == 2);

	/* order should be new, first */
	dn = dl_get_first(dl);
	mu_should(equal_string(dn->payload, "inserted before first"));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string(dn->payload, "first"));

	/* while we're at it, check that an error is reported when we
	 * reach the end of the list. */
	dn = dl_get_next(dl, dn);
	mu_shouldnt(dn);
	mu_should(dl_get_error(dl));

	/* check from the tail */
	dn = dl_get_last(dl);
	mu_should(equal_string(dn->payload, "first"));
	dn = dl_get_previous(dl, dn);
	mu_should(equal_string(dn->payload, "inserted before first"));

	/* again, should get an error trying to read past head */
	dn = dl_get_previous(dl, dn);
	mu_shouldnt(dn);
	mu_should(dl_get_error(dl));

	/* clean up before switching to the preloaded list */
	while (dn = dl_get_first(dl), dn) {
		free(dn->payload);
		dl_delete(dl, dn);
	}
	dl_destroy(dl);

	/* insert after the head of the list */
	dl = test_dl;
	dn = dl_get_first(dl);
	payload = strdup("inserted before first");
	dn = dl_insert_before(dl, dn, payload);
	mu_should(dn);

	/* first three should be new, 10, 20 */
	dn = dl_get_first(dl);
	mu_should(equal_string("inserted before first", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("0010 bogus", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("0020 bogus", dn->payload));

	/* and now after the end */
	dn = dl_get_last(dl);
	payload = strdup("inserted before last");
	dn = dl_insert_before(dl, dn, payload);
	mu_should(dn);

	/* last three should be 980, new, 990 */
	dn = dl_get_last(dl);
	mu_should(equal_string("0990 bogus", dn->payload));
	dn = dl_get_previous(dl, dn);
	mu_should(equal_string("inserted before last", dn->payload));
	dn = dl_get_previous(dl, dn);
	mu_should(equal_string("0980 bogus", dn->payload));

	/* now read forward to end, end should be detected. */
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("inserted before last", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_should(equal_string("0990 bogus", dn->payload));
	dn = dl_get_next(dl, dn);
	mu_shouldnt(dn);
	mu_should(dl_get_error(dl));

	/* count should be 99 + 2 = 101 */
	mu_should(dl_count(dl) == 99 + 2);

	/* find 0500 in the list */
	dn = dl_get_first(dl);
	while (dn) {
		if (equal_string("0500 bogus", dn->payload))
			break;
		dn = dl_get_next(dl, dn);
	}
	mu_should(dn);

	/* insert before */
	payload = strdup("inserted before 0500");
	dn = dl_insert_before(dl, dn, payload);

	/* quick check count */
	mu_should(dl_count(dl) == 99 + 2 + 1);

	/* find it forward */
	dn = dl_get_first(dl);
	while (dn) {
		if (equal_string("0500 bogus", dn->payload))
			break;
		dn = dl_get_next(dl, dn);
	}
	mu_should(dn);

	dn = dl_get_previous(dl, dn);
	mu_should(equal_string("inserted before 0500", dn->payload));

	/* and now find it backward */
	dn = dl_get_last(dl);
	while (dn) {
		if (equal_string("inserted before 0500", dn->payload))
			break;
		dn = dl_get_previous(dl, dn);
	}
	mu_should(dn);

	dn = dl_get_next(dl, dn);
	mu_should(equal_string("0500 bogus", dn->payload));

	/* lastly check the count */
	mu_should(dl_count(dl) == 99 + 2 + 1);
}

/*
 * test_insert_many.
 *
 * add a few items off the ends of the list and then confirm that they
 * are where they should be.
 */

MU_TEST(test_insert_many) {
	dlcb *dl = test_dl;
	int start_nodes = dl_count(dl);
	int added_nodes = 0;
	/* from the front, add an item after every 0x2x value. */
	dlnode *dn = dl_get_first(dl);
	mu_should(dn && dn->payload);
	mu_should(equal_string(dn->payload, "0010 bogus"));
	while (dn = dl_get_next(dl, dn), dn) {
		char *payload = dn->payload;
		if (payload[2] != '2')
			continue;
		char *read_to = strdup(payload);
		read_to[3] = '5';
		dn = dl_insert_after(dl, dn, read_to);
		mu_should(dn && dn->payload == read_to);
		added_nodes += 1;
	}
	/* now verify chaining reading forward. */
	int found_nodes = 0;
	dn = dl_get_first(dl);
	found_nodes += 1;
	while (dn = dl_get_next(dl, dn), dn) {
		found_nodes += 1;
	}
	mu_should(found_nodes == start_nodes + added_nodes);
	/* and backward. */
	found_nodes = 0;
	dn = dl_get_last(dl);
	found_nodes += 1;
	while (dn = dl_get_previous(dl, dn), dn)
		found_nodes += 1;
	mu_should(found_nodes == start_nodes + added_nodes);
	/* now take it the other way */
	start_nodes = dl_count(dl);
	added_nodes = 0;
	dn = dl_get_first(dl);
	mu_should(dn && dn->payload);
	mu_should(equal_string(dn->payload, "0010 bogus"));
	while (dn = dl_get_next(dl, dn), dn) {
		char *payload = dn->payload;
		if (payload[2] != '4')
			continue;
		char *read_to = strdup(payload);
		read_to[2] = read_to[2] - 1;
		read_to[3] = read_to[3] + 5;
		dn = dl_insert_before(dl, dn, read_to);
		mu_should(dn && dn->payload == read_to);
		added_nodes += 1;
		/* reposition to the node that triggered the insert before */
		/* DOH! */
		dn = dl_get_next(dl, dn);
	}
	/* now verify chaining reading forward. */
	found_nodes = 0;
	dn = dl_get_first(dl);
	found_nodes += 1;
	while (dn = dl_get_next(dl, dn), dn) {
		found_nodes += 1;
	}
	mu_should(found_nodes == start_nodes + added_nodes);
	/* and backward. */
	found_nodes = 0;
	dn = dl_get_last(dl);
	found_nodes += 1;
	while (dn = dl_get_previous(dl, dn), dn)
		found_nodes += 1;
	mu_should(found_nodes == start_nodes + added_nodes);
}

/*
 * test_get_first.
 *
 * get the first item on the list.
 */

MU_TEST(test_get_first) {
	dlcb *dl = test_dl;
	dlnode *dn = dl_get_first(dl);
	mu_should(dn && dn->payload);
	mu_should(equal_string(dn->payload, "0010 bogus"));
}

/*
 * test_get_list.
 *
 * get the last item on the list.
 */

MU_TEST(test_get_last) {
	dlcb *dl = test_dl;
	dlnode *dn = dl_get_last(dl);
	mu_should(dn && dn->payload);
	mu_should(equal_string(dn->payload, "0990 bogus"));
}

/*
 * test_get_next.
 *
 * read forward from a specific item. also make sure that dl_get_next
 * properly handles end of list.
 */

MU_TEST(test_get_next) {
	dlcb *dl = test_dl;

	/* somewhere in the list */
	dlnode *dn = dl_get_first(dl);
	mu_should(dn);
	while (!equal_string(dn->payload, "0500 bogus"))
		dn = dl_get_next(dl, dn);

	/* read forwards a couple of times */
	dn = dl_get_next(dl, dn);
	dn = dl_get_next(dl, dn);
	mu_should(equal_string(dn->payload, "0520 bogus"));

	/* end of list */
	dn = dl_get_last(dl);
	mu_should(dn);
	mu_should(equal_string(dn->payload, "0990 bogus"));
	dn = dl_get_next(dl, dn);
	mu_shouldnt(dn);
}

/*
 * test_get_previous.
 *
 * read backward from a specific item. also make sure that
 * dl_get_previous properly handles start of list.
 */

MU_TEST(test_get_previous) {
	dlcb *dl = NULL;
	dlnode *dn = NULL;

	/* can't read previous from first, no wrap */
	dl = test_dl;
	dn = dl_get_first(dl);
	mu_should(dn && equal_string(dn->payload, "0010 bogus"));
	dn = dl_get_previous(dl, dn);

	/* but can read previous from last */
	dn = dl_get_last(dl);
	mu_should(dn && equal_string(dn->payload, "0990 bogus"));
	while (!equal_string(dn->payload, "0500 bogus"))
		dn = dl_get_previous(dl, dn);

	/* read forwards a couple of times */
	dn = dl_get_previous(dl, dn);
	dn = dl_get_previous(dl, dn);
	mu_should(equal_string(dn->payload, "0480 bogus"));
}

/*
 * test_delete.
 *
 * as link maintenance is part of the delete process, we need to
 * delete from the ends and in the middle of the list.
 */

MU_TEST(test_delete) {
	dlcb *dl = NULL;
	dlnode *dn = NULL;

	/* test deleting from the head */
	dl = test_dl;
	dn = dl_get_first(dl);
	mu_should(dn && equal_string(dn->payload, "0010 bogus"));
	free(dn->payload);
	mu_should(dl_delete(dl, dn));
	dn = dl_get_first(dl);
	mu_should(dn && equal_string(dn->payload, "0020 bogus"));
	mu_should(dl_count(dl) == 98);

	/* and now from the tail */
	dn = dl_get_last(dl);
	mu_should(dn && equal_string(dn->payload, "0990 bogus"));
	free(dn->payload);
	mu_should(dl_delete(dl, dn));
	dn = dl_get_last(dl);
	mu_should(dl_count(dl) == 97);

	/* now somewhere in the middle */
	while (!equal_string(dn->payload, "0600 bogus"))
		dn = dl_get_previous(dl, dn);

	free(dn->payload);
	mu_should(dl_delete(dl, dn));
	mu_should(dl_count(dl) == 96);

	/* while i know dl_count chases all the links and does a
	   crosscheck against a global count, we'll do some
	   chasing of links here too. */

	/* read past the deleted node from both ends. */
	dn = dl_get_first(dl);
	mu_should(dn && equal_string(dn->payload, "0020 bogus"));
	while (dn = dl_get_next(dl, dn), dn)
		if (equal_string(dn->payload, "0600 bogus"))
			mu_shouldnt(true);

	dn = dl_get_last(dl);
	mu_should(dn && equal_string(dn->payload, "0980 bogus"));
	while (dn = dl_get_previous(dl, dn), dn)
		if (equal_string(dn->payload, "0600 bogus"))
			mu_shouldnt(true);
}

/*
 * test_update.
 *
 * the update function is only needed if you wish to change the
 * contents of the payload field. as no link maintenance is
 * done, there's not a lot to test here.
 */

MU_TEST(test_update) {
	dlcb *dl = NULL;
	dlnode *dn = NULL;
	void *old_payload = NULL;
	void *new_payload = NULL;

	/* test updating end items */
	dl = test_dl;
	dn = dl_get_first(dl);
	mu_should(dn && equal_string(dn->payload, "0010 bogus"));
	old_payload = dn->payload;
	new_payload = strdup("0010 not bogus");
	mu_should(dl_update(dl, dn, new_payload));
	free(old_payload);
	dn = dl_get_next(dl, dn);
	dn = dl_get_next(dl, dn);
	mu_should(dn && equal_string(dn->payload, "0030 bogus"));
	dn = dl_get_first(dl);
	mu_shouldnt(dn && equal_string(dn->payload, "0010 bogus"));
	mu_should(dn && equal_string(dn->payload, "0010 not bogus"));
}

/*
 * test that an error is set when positioning is lost due to
 * client sequencing error.
 */

MU_TEST(test_bad_position) {
	dlcb *dl = test_dl;
	dlnode *first = dl_get_first(dl);
	dlnode *last = dl_get_last(dl);
	void *payload = "this should fail";
	/* list is positioned on last, so try to update with the first node */
	dlnode *result = dl_update(dl, first, payload);
	mu_should(dl_get_error(dl));
	mu_shouldnt(result);
	/* position is lost, so moving relative to the current position
	 * should error. */
	result = dl_get_previous(dl, last);
	mu_should(dl_get_error(dl));
	mu_shouldnt(result);
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
	MU_RUN_TEST(test_insert_ends);
	MU_RUN_TEST(test_insert_after);
	MU_RUN_TEST(test_insert_before);
	MU_RUN_TEST(test_insert_many);
	MU_RUN_TEST(test_get_first);
	MU_RUN_TEST(test_get_last);
	MU_RUN_TEST(test_get_next);
	MU_RUN_TEST(test_get_previous);
	MU_RUN_TEST(test_delete);
	MU_RUN_TEST(test_update);
	MU_RUN_TEST(test_bad_position);
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
