/*  unitdl.c -- tests for my doubly linked list library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "txbmisc.h"
#include "txbdl.h"
#include "txbstr.h"
#include "txbrand.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803
static hdl *test_dl = NULL;

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
	dlid id = NULL_DLID;
	ppayload payload;
	while (id = dl_get_first(test_dl, &payload), payload) {
		free(payload);
		dl_delete(test_dl, id);
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
	hdl *dl = dl_create();
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

	hdl *dl = dl_create();
	dlid id = NULL_DLID;
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
	ppayload payload;
	while (ordering[i]) {
		id = dl_get_first(dl, &payload);
		mu_should(((char *)payload)[0] == ordering[i]);
		dl_delete(dl, id);
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
		id = dl_get_last(dl, &payload);
		mu_should(((char *)payload)[0] == ordering[i]);
		dl_delete(dl, id);
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
	hdl *dl = NULL;
	dlid id = NULL_DLID;
	ppayload payload = NULL;

	/* in an empty list do the inserts work as expected? after the
	 * inserts, traverse the list from first to last and then last
	 * to first, checking that the payloads are as expected.
	 *
	 * this pattern of validation will repeat throughout these
	 * unit tests. */

	/* insert after head of single item list */
	dl = dl_create();
	payload = strdup("first");
	id = dl_insert_first(dl, payload);
	mu_shouldnt(null_dlid(id));
	id = dl_get_first(dl, &payload);
	payload = strdup("inserted after first");
	id = dl_insert_after(dl, id, payload);
	mu_shouldnt(null_dlid(id));
	mu_should(dl_count(dl) == 2);

	/* order should be first, inserted after first */
	id = dl_get_first(dl, &payload);
	mu_should(equal_string(payload, "first"));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string(payload, "inserted after first"));

	/* while we're at it, check that an error is reported when we
	 * reach the end of the list. */
	id = dl_get_next(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_should(dl_get_error(dl));

	/* check from the tail */
	id = dl_get_last(dl, &payload);
	mu_should(equal_string(payload, "inserted after first"));
	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string(payload, "first"));

	/* again, should get an error trying to read past head */
	id = dl_get_previous(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_should(dl_get_error(dl));

	/* clean up before switching to the preloaded list */
	while (id = dl_get_first(dl, &payload), !null_dlid(id)) {
		free(payload);
		dl_delete(dl, id);
	}
	dl_destroy(dl);

	/* insert after the head of the list */
	dl = test_dl;
	id = dl_get_first(dl, &payload);
	payload = strdup("inserted after first");
	id = dl_insert_after(dl, id, payload);
	mu_shouldnt(null_dlid(id));

	/* first three should be 10, new, 20 */
	id = dl_get_first(dl, &payload);
	mu_should(equal_string("0010 bogus", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("inserted after first", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("0020 bogus", payload));

	/* and now after the end */
	id = dl_get_last(dl, &payload);
	payload = strdup("inserted after last");
	id = dl_insert_after(dl, id, payload);
	mu_shouldnt(null_dlid(id));

	/* last three should be new, 990, 980 */
	id = dl_get_last(dl, &payload);
	mu_should(equal_string("inserted after last", payload));
	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string("0990 bogus", payload));
	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string("0980 bogus", payload));

	/* now read forward to end, end should be detected. */
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("0990 bogus", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("inserted after last", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_should(dl_get_error(dl));

	/* count should be 99 + 2 = 101 */
	mu_should(dl_count(dl) == 99 + 2);

	/* find 0500 in the list */
	id = dl_get_first(dl, &payload);
	while (!null_dlid(id)) {
		if (equal_string("0500 bogus", payload))
			break;
		id = dl_get_next(dl, id, &payload);
	}
	mu_shouldnt(null_dlid(id));
	mu_should(equal_string("0500 bogus", payload));

	/* insert after */
	payload = strdup("inserted after 0500");
	id = dl_insert_after(dl, id, payload);

	/* find it forward */
	id = dl_get_first(dl, &payload);
	while (!null_dlid(id)) {
		if (equal_string("0500 bogus", payload))
			break;
		id = dl_get_next(dl, id, &payload);
	}
	mu_shouldnt(null_dlid(id));

	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("inserted after 0500", payload));

	/* and now find it backward */
	id = dl_get_last(dl, &payload);
	while (!null_dlid(id)) {
		if (equal_string("inserted after 0500", payload))
			break;
		id = dl_get_previous(dl, id, &payload);
	}
	mu_shouldnt(null_dlid(id));

	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string("0500 bogus", payload));

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
	hdl *dl = NULL;
	dlid id = NULL_DLID;
	ppayload payload = NULL;

	/* in an empty list do the inserts work as expected? after the
	 * inserts, traverse the list from first to last and then last
	 * to first, checking that the payloads are as expected.
	 *
	 * this pattern of validation will repeat throughout these
	 * unit tests. */

	/* insert after head of single item list */
	dl = dl_create();
	payload = strdup("first");
	id = dl_insert_first(dl, payload);
	mu_shouldnt(null_dlid(id));
	id = dl_get_first(dl, &payload);
	payload = strdup("inserted before first");
	id = dl_insert_before(dl, id, payload);
	mu_shouldnt(null_dlid(id));
	mu_should(dl_count(dl) == 2);

	/* order should be new, first */
	id = dl_get_first(dl, &payload);
	mu_should(equal_string(payload, "inserted before first"));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string(payload, "first"));

	/* while we're at it, check that an error is reported when we
	 * reach the end of the list. */
	id = dl_get_next(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_should(dl_get_error(dl));

	/* check from the tail */
	id = dl_get_last(dl, &payload);
	mu_should(equal_string(payload, "first"));
	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string(payload, "inserted before first"));

	/* again, should get an error trying to read past head */
	id = dl_get_previous(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_should(dl_get_error(dl));

	/* clean up before switching to the preloaded list */
	while (id = dl_get_first(dl, &payload), id) {
		free(payload);
		dl_delete(dl, id);
	}
	dl_destroy(dl);

	/* insert after the head of the list */
	dl = test_dl;
	id = dl_get_first(dl, &payload);
	payload = strdup("inserted before first");
	id = dl_insert_before(dl, id, payload);
	mu_shouldnt(null_dlid(id));

	/* first three should be new, 10, 20 */
	id = dl_get_first(dl, &payload);
	mu_should(equal_string("inserted before first", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("0010 bogus", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("0020 bogus", payload));

	/* and now after the end */
	id = dl_get_last(dl, &payload);
	payload = strdup("inserted before last");
	id = dl_insert_before(dl, id, payload);
	mu_shouldnt(null_dlid(id));

	/* last three should be 980, new, 990 */
	id = dl_get_last(dl, &payload);
	mu_should(equal_string("0990 bogus", payload));
	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string("inserted before last", payload));
	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string("0980 bogus", payload));

	/* now read forward to end, end should be detected. */
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("inserted before last", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("0990 bogus", payload));
	id = dl_get_next(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_should(dl_get_error(dl));

	/* count should be 99 + 2 = 101 */
	mu_should(dl_count(dl) == 99 + 2);

	/* find 0500 in the list */
	id = dl_get_first(dl, &payload);
	while (id) {
		if (equal_string("0500 bogus", payload))
			break;
		id = dl_get_next(dl, id, &payload);
	}
	mu_shouldnt(null_dlid(id));

	/* insert before */
	payload = strdup("inserted before 0500");
	id = dl_insert_before(dl, id, payload);

	/* quick check count */
	mu_should(dl_count(dl) == 99 + 2 + 1);

	/* find it forward */
	id = dl_get_first(dl, &payload);
	while (id) {
		if (equal_string("0500 bogus", payload))
			break;
		id = dl_get_next(dl, id, &payload);
	}
	mu_shouldnt(null_dlid(id));

	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string("inserted before 0500", payload));

	/* and now find it backward */
	id = dl_get_last(dl, &payload);
	while (id) {
		if (equal_string("inserted before 0500", payload))
			break;
		id = dl_get_previous(dl, id, &payload);
	}
	mu_shouldnt(null_dlid(id));

	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string("0500 bogus", payload));

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
	hdl *dl = test_dl;
	ppayload payload = NULL;
	int start_nodes = dl_count(dl);
	int added_nodes = 0;

	/* from the front, add an item after every 0x2x value. */
	dlid id = dl_get_first(dl, &payload);
	mu_should(payload && equal_string(payload, "0010 bogus"));
	while (id = dl_get_next(dl, id, &payload), id) {
		char *payload_char = payload;
		if (payload_char[2] != '2')
			continue;
		char *read_to = strdup(payload_char);
		read_to[3] = '5';
		id = dl_insert_after(dl, id, read_to);
		mu_shouldnt(null_dlid(id));
		added_nodes += 1;
	}

	/* now verify chaining reading forward. */
	int found_nodes = 0;
	id = dl_get_first(dl, &payload);
	found_nodes += 1;
	while (id = dl_get_next(dl, id, &payload), !null_dlid(id))
		found_nodes += 1;
	mu_should(found_nodes == start_nodes + added_nodes);

	/* and backward. */
	found_nodes = 0;
	id = dl_get_last(dl, &payload);
	found_nodes += 1;
	while (id = dl_get_previous(dl, id, &payload), id)
		found_nodes += 1;
	mu_should(found_nodes == start_nodes + added_nodes);

	/* now take it the other way */
	start_nodes = dl_count(dl);
	added_nodes = 0;
	id = dl_get_first(dl, &payload);
	mu_shouldnt(null_dlid(id));
	mu_should(equal_string(payload, "0010 bogus"));
	while (id = dl_get_next(dl, id, &payload), !null_dlid(id)) {
		char *payload_chars = payload;
		if (payload_chars[2] != '4')
			continue;
		char *read_to = strdup(payload_chars);
		read_to[2] = read_to[2] - 1;
		read_to[3] = read_to[3] + 5;
		id = dl_insert_before(dl, id, read_to);
		mu_shouldnt(null_dlid(id));
		added_nodes += 1;
		/* reposition to the node that triggered the insert before */
		/* DOH! */
		id = dl_get_next(dl, id, &payload);
	}

	/* now verify chaining reading forward. */
	found_nodes = 0;
	id = dl_get_first(dl, &payload);
	found_nodes += 1;
	while (id = dl_get_next(dl, id, &payload), !null_dlid(id))
		found_nodes += 1;
	mu_should(found_nodes == start_nodes + added_nodes);

	/* and backward. */
	found_nodes = 0;
	id = dl_get_last(dl, &payload);
	found_nodes += 1;
	while (id = dl_get_previous(dl, id, &payload), !null_dlid(id))
		found_nodes += 1;
	mu_should(found_nodes == start_nodes + added_nodes);
}

/*
 * test_get_first.
 *
 * get the first item on the list.
 */

MU_TEST(test_get_first) {
	hdl *dl = test_dl;
	ppayload payload = NULL;
	dlid id = dl_get_first(dl, &payload);
	mu_shouldnt(null_dlid(id));
	mu_should(payload);
	mu_should(equal_string(payload, "0010 bogus"));
}

/*
 * test_get_last.
 *
 * get the last item on the list.
 */

MU_TEST(test_get_last) {
	hdl *dl = test_dl;
	ppayload payload = NULL;
	dlid id = dl_get_last(dl, &payload);
	mu_shouldnt(null_dlid(id));
	mu_should(payload);
	mu_should(equal_string(payload, "0990 bogus"));
}

/*
 * test_get_next.
 *
 * read forward from a specific item. also make sure that dl_get_next
 * properly handles end of list.
 */

MU_TEST(test_get_next) {
	hdl *dl = test_dl;

	/* somewhere in the list */
	ppayload payload = NULL;
	dlid id = dl_get_first(dl, &payload);
	mu_shouldnt(null_dlid(id));
	while (!equal_string(payload, "0500 bogus"))
		id = dl_get_next(dl, id, &payload);

	/* read forwards a couple of times */
	id = dl_get_next(dl, id, &payload);
	id = dl_get_next(dl, id, &payload);
	mu_should(equal_string(payload, "0520 bogus"));

	/* end of list */
	id = dl_get_last(dl, &payload);
	mu_shouldnt(null_dlid(id));
	mu_should(equal_string(payload, "0990 bogus"));
	id = dl_get_next(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_should(dl_get_error(dl));
}

/*
 * test_get_previous.
 *
 * read backward from a specific item. also make sure that
 * dl_get_previous properly handles start of list.
 */

MU_TEST(test_get_previous) {
	hdl *dl = NULL;
	dlid id = NULL_DLID;
	ppayload payload = NULL;

	/* can't read previous from first, no wrap */
	dl = test_dl;
	id = dl_get_first(dl, &payload);
	mu_shouldnt(null_dlid(id));
	mu_should(equal_string(payload, "0010 bogus"));
	id = dl_get_previous(dl, id, &payload);
	mu_should(null_dlid(id));
	mu_shouldnt(payload);
	mu_should(dl_get_error(dl));

	/* but can read previous from last */
	id = dl_get_last(dl, &payload);
	mu_should(equal_string(payload, "0990 bogus"));
	while (!equal_string(payload, "0500 bogus"))
		id = dl_get_previous(dl, id, &payload);

	/* read backwards a couple of times */
	id = dl_get_previous(dl, id, &payload);
	id = dl_get_previous(dl, id, &payload);
	mu_should(equal_string(payload, "0480 bogus"));
}

/*
 * test_delete.
 *
 * as link maintenance is part of the delete process, we need to
 * delete from the ends and in the middle of the list.
 */

MU_TEST(test_delete) {
	hdl *dl = NULL;
	dlid id = NULL_DLID;
	ppayload payload = NULL;

	/* test deleting from the head */
	dl = test_dl;
	id = dl_get_first(dl, &payload);
	mu_shouldnt(null_dlid(id));
	mu_should(equal_string(payload, "0010 bogus"));
	free(payload);
	mu_should(dl_delete(dl, id));
	id = dl_get_first(dl, &payload);
	mu_shouldnt(null_dlid(id));
	mu_should(equal_string(payload, "0020 bogus"));
	mu_should(dl_count(dl) == 98);

	/* and now from the tail */
	id = dl_get_last(dl, &payload);
	mu_should(equal_string(payload, "0990 bogus"));
	free(payload);
	mu_should(dl_delete(dl, id));
	id = dl_get_last(dl, &payload);
	mu_should(equal_string(payload, "0980 bogus"));
	mu_should(dl_count(dl) == 97);

	/* now somewhere in the middle */
	while (!equal_string(payload, "0600 bogus"))
		id = dl_get_previous(dl, id, &payload);

	free(payload);
	mu_should(dl_delete(dl, id));
	mu_should(dl_count(dl) == 96);

	/* while i know dl_count chases all the links and does a
	   crosscheck against a global count, we'll do some
	   chasing of links here too. */

	/* read past the deleted node from both ends. */
	id = dl_get_first(dl, &payload);
	mu_should(equal_string(payload, "0020 bogus"));
	while (id = dl_get_next(dl, id, &payload), !null_dlid(id))
		if (equal_string(payload, "0600 bogus"))
			mu_shouldnt(true);

	id = dl_get_last(dl, &payload);
	mu_should(equal_string(payload, "0980 bogus"));
	while (id = dl_get_previous(dl, id, &payload), !null_dlid(id))
		if (equal_string(payload, "0600 bogus"))
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
	hdl *dl = NULL;
	dlid id = NULL_DLID;
	ppayload old_payload = NULL;
	ppayload new_payload = NULL;

	/* test updating end items */
	dl = test_dl;
	id = dl_get_first(dl, &old_payload);
	mu_should(equal_string(old_payload, "0010 bogus"));
	new_payload = strdup("0010 not bogus");
	mu_should(dl_update(dl, id, new_payload));
	free(old_payload);
	id = dl_get_next(dl, id, &old_payload);
	id = dl_get_next(dl, id, &old_payload);
	mu_shouldnt(null_dlid(id));
	mu_should(equal_string(old_payload, "0030 bogus"));
	id = dl_get_first(dl, &old_payload);
	mu_shouldnt(null_dlid(id));
	mu_shouldnt(equal_string(old_payload, "0010 bogus"));
	mu_should(equal_string(old_payload, "0010 not bogus"));
}

/*
 * test that an error is set when positioning is lost due to
 * client sequencing error.
 */

MU_TEST(test_bad_position) {
	hdl *dl = test_dl;
	ppayload first_payload = NULL;
	dlid first_id = dl_get_first(dl, &first_payload);
	ppayload last_payload = NULL;
	dlid last_id = dl_get_last(dl, &last_payload);
	ppayload replacement_payload = "this should fail";
	/* list is positioned on last, so try to update with the first node */
	bool result = dl_update(dl, first_id, replacement_payload);
	mu_should(dl_get_error(dl));
	mu_shouldnt(result);
	/* position is lost, so moving relative to the current position
	 * should error. */
	ppayload previous_payload = NULL;
	result = dl_get_previous(dl, last_id, &previous_payload);
	mu_should(dl_get_error(dl));
	mu_shouldnt(result);
}


MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

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

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitdl.c ends here */
