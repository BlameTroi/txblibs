/*  unitpq.c -- tests for the priority queue header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include "minunit.h"
#include "txbmisc.h"
#include "txbrand.h"
#include "txbstr.h"
#include "txbpq.h"

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

void
test_setup(void) {
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
	hpq *pq = pq_create();
	mu_should(pq);
	mu_should(pq_destroy(pq));
}

MU_TEST(test_empty) {
	hpq *pq = pq_create();
	mu_should(pq_empty(pq));
	mu_should(pq_count(pq) == 0);
	mu_should(pq_destroy(pq));
}

MU_TEST(test_access_empty) {
	hpq *pq = pq_create();
	long priority;
	ppayload payload;
	mu_shouldnt(pq_peek_highest(pq, &priority, &payload));
	mu_shouldnt(pq_peek_lowest(pq, &priority, &payload));
	mu_shouldnt(pq_get_highest(pq, &priority, &payload));
	mu_shouldnt(pq_get_lowest(pq, &priority, &payload));
	mu_should(pq_destroy(pq));
}

MU_TEST(test_insert) {
	hpq *pq = pq_create();
	pq_insert(pq, 100, "100");
	mu_shouldnt(pq_empty(pq));
	mu_should(pq_count(pq) == 1);
	mu_shouldnt(pq_destroy(pq));
	long priority;
	ppayload payload;
	mu_should(pq_get_highest(pq, &priority, &payload));
	mu_should(equal_string(payload, "100"));
	mu_should(pq_empty(pq));
	mu_should(pq_count(pq) == 0);
	mu_should(pq_destroy(pq));
}

MU_TEST(test_read_loop) {
	hpq *pq = pq_create();
	pq_insert(pq, 100, "100");
	pq_insert(pq, 99, "99");
	pq_insert(pq, 101, "101");
	int i = 0;
	long priority;
	ppayload payload;
	while (pq_get_highest(pq, &priority, &payload))
		i += 1;
	mu_should(i == 3);
	mu_should(pq_empty(pq));
	mu_should(pq_destroy(pq));
}

MU_TEST(test_peek_high_low) {
	hpq *pq = pq_create();
	pq_insert(pq, 100, "100");
	pq_insert(pq, 99, "99");
	pq_insert(pq, 101, "101");

	long priority;
	ppayload payload;

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
	hpq *pq = pq_create();
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
	ppayload payload;
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

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	printf("\n\npriority queue\n\n");
	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_empty);
	MU_RUN_TEST(test_access_empty);
	MU_RUN_TEST(test_insert);
	MU_RUN_TEST(test_read_loop);
	MU_RUN_TEST(test_random_volume);
	MU_RUN_TEST(test_peek_high_low);
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitpq.c ends here */
