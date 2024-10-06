/*  unitpq.c -- units for my header libraries -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>

#include "minunit.h"

#include "../inc/misc.h"
#include "../inc/rand.h"
#include "../inc/str.h"

#include "../inc/pq.h"

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

void
test_setup(void) {
	/* let's use a different seed than 1, but not time() because i want
	   repeatable tests. */
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);

}

void
test_teardown(void) {
}

/*
 * the priority queue tests:
 */

MU_TEST(test_create) {
	pqcb *pq = pq_create();
	mu_should(pq);
	mu_should(pq_destroy(pq));
}

MU_TEST(test_empty) {
	pqcb *pq = pq_create();
	mu_should(pq_empty(pq));
	mu_should(pq_count(pq) == 0);
	mu_should(pq_destroy(pq));
}

MU_TEST(test_access_empty) {
	pqcb *pq = pq_create();
	long priority;
	void *payload;
	mu_shouldnt(pq_peek_highest(pq, &priority, &payload));
	mu_shouldnt(pq_peek_lowest(pq, &priority, &payload));
	mu_shouldnt(pq_get_highest(pq, &priority, &payload));
	mu_shouldnt(pq_get_lowest(pq, &priority, &payload));
	mu_should(pq_destroy(pq));
}

MU_TEST(test_insert) {
	pqcb *pq = pq_create();
	pq_insert(pq, 100, "100");
	mu_shouldnt(pq_empty(pq));
	mu_should(pq_count(pq) == 1);
	mu_shouldnt(pq_destroy(pq));
	long priority;
	void *payload;
	mu_should(pq_get_highest(pq, &priority, &payload));
	mu_should(equal_string(payload, "100"));
	mu_should(pq_empty(pq));
	mu_should(pq_count(pq) == 0);
	mu_should(pq_destroy(pq));
}

MU_TEST(test_read_loop) {
	pqcb *pq = pq_create();
	pq_insert(pq, 100, "100");
	pq_insert(pq, 99, "99");
	pq_insert(pq, 101, "101");
	int i = 0;
	long priority;
	void *payload;
	while (pq_get_highest(pq, &priority, &payload))
		i += 1;
	mu_should(i == 3);
	mu_should(pq_empty(pq));
	mu_should(pq_destroy(pq));
}

MU_TEST(test_peek_high_low) {
	pqcb *pq = pq_create();
	pq_insert(pq, 100, "100");
	pq_insert(pq, 99, "99");
	pq_insert(pq, 101, "101");

	long priority;
	void *payload;

	pq_peek_highest(pq, &priority, &payload);
	mu_should(priority = 101);
	mu_should(equal_string(payload, "101"));

	pq_peek_lowest(pq, &priority, &payload);
	mu_should(priority = 99);
	mu_should(equal_string(payload, "99"));

	int i = 0;
	while (pq_get_highest(pq, &priority, &payload))
		i += 1;
	mu_should(i == 3);
	mu_should(pq_empty(pq));
	mu_should(pq_destroy(pq));
}

MU_TEST(test_random_volume) {
	pqcb *pq = pq_create();
	pq_insert(pq, 1024, (void *)1024);
	pq_insert(pq, 8888, (void *)8888);
	pq_insert(pq, -3, (void *)-3);
	mu_shouldnt(pq_empty(pq));
	mu_should(pq_count(pq) == 3);
	for (int i = 0; i < 10000; i++) {
		long j = random_between(0, 99999);
		pq_insert(pq, j, (void *)j);
	}
	mu_shouldnt(pq_empty(pq));
	mu_should(pq_count(pq) == 10003);
	long last_pri;
	long priority;
	void *payload;
	mu_should(pq_peek_highest(pq, &priority, &payload));
	last_pri = priority;
	while (!pq_empty(pq)) {
		pq_get_highest(pq, &priority, &payload);
		if (priority > last_pri)
			mu_should((priority <= last_pri));
		last_pri = priority;
	}
	printf("\n");
	mu_should(pq_empty(pq));
	mu_should(pq_count(pq) == 0);
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

	printf("\n\npriority queue\n\n");
	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_empty);
	MU_RUN_TEST(test_access_empty);
	MU_RUN_TEST(test_insert);
	MU_RUN_TEST(test_read_loop);
	MU_RUN_TEST(test_random_volume);
	MU_RUN_TEST(test_peek_high_low);
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
/* unitpq.c ends here */
