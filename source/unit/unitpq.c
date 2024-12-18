/*  unitpq.c -- tests for the priority queue header library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <stdbool.h>
#include <stdio.h>
#include "minunit.h"
#include "txbmisc.h"
#include "txbrand.h"
#include "txbstr.h"
#include "txballoc.h"
#include "txbone.h"

/*
 * minunit setup and teardown of listd infratstructure.
 */

#define RAND_SEED 6803

void
test_setup(void) {
	set_random_generator(RAND_DEFAULT);
	seed_random_generator(RAND_SEED);
	tsinitialize(25000, txballoc_f_errors, stderr);
}

void
test_teardown(void) {
	tsterminate();
}

/*
 * the priority queue tests:
 */

MU_TEST(test_create) {
	one_block *pq = make_one(pqueue);
	mu_should(pq);
	free_one(pq);
}

MU_TEST(test_is_empty) {
	one_block *pq = make_one(pqueue);
	mu_should(is_empty(pq));
	mu_should(count(pq) == 0);
	free_one(pq);
}

MU_TEST(test_access_is_empty) {
	one_block *pq = make_one(pqueue);
	mu_shouldnt(peek_max(pq));
	mu_shouldnt(peek_min(pq));
	mu_shouldnt(get_max(pq));
	mu_shouldnt(get_min(pq));
	free_one(pq);
}

MU_TEST(test_add_with_priority) {
	one_block *pq = make_one(pqueue);
	add_with_priority(pq, 100, "100");
	mu_shouldnt(is_empty(pq));
	mu_should(count(pq) == 1);
	void *payload = get_max(pq);
	mu_should(equal_string((char *)payload, "100"));
	mu_should(is_empty(pq));
	mu_should(count(pq) == 0);
	free_one(pq);
}

MU_TEST(test_read_loop) {
	one_block *pq = make_one(pqueue);
	add_with_priority(pq, 100, "100");
	add_with_priority(pq, 99, "99");
	add_with_priority(pq, 101, "101");
	int i = 0;
	void *payload;
	while (payload = get_max(pq), payload)
		i += 1;
	mu_should(i == 3);
	mu_should(is_empty(pq));
	free_one(pq);
}

MU_TEST(test_peek_high_low) {
	one_block *pq = make_one(pqueue);
	add_with_priority(pq, 100, "100");
	add_with_priority(pq, 99, "99");
	add_with_priority(pq, 101, "101");
	mu_should(count(pq) == 3);

	long priority;
	void *payload;

	payload = peek_max(pq);
	priority = max_priority(pq);
	mu_should(priority == 101);
	mu_should(equal_string((char *)payload, "101"));
	mu_should(count(pq) == 3);

	payload = peek_min(pq);
	priority = min_priority(pq);
	mu_should(priority == 99);
	mu_should(equal_string((char *)payload, "99"));

	int i = 0;
	while (get_max(pq))
		i += 1;
	mu_should(i == 3);
	mu_should(is_empty(pq));
	free_one(pq);
}

MU_TEST(test_random_volume) {
	one_block *pq = make_one(pqueue);
	add_with_priority(pq, 1024, (void *)1024);
	add_with_priority(pq, 8888, (void *)8888);
	add_with_priority(pq, -3, (void *)-3);
	mu_shouldnt(is_empty(pq));
	mu_should(count(pq) == 3);
	for (int i = 0; i < 10000; i++) {
		long j = random_between(0, 99999);
		add_with_priority(pq, j, (void *)j);
	}
	mu_shouldnt(is_empty(pq));
	mu_should(count(pq) == 10003);
	long last_pri;
	long priority;
	mu_should(peek_max(pq));
	last_pri = max_priority(pq);
	while (!is_empty(pq)) {
		priority = max_priority(pq);
		get_max(pq);
		if (priority > last_pri)
			mu_should((priority <= last_pri));
		last_pri = priority;
	}
	printf("\n");
	mu_should(is_empty(pq));
	mu_should(count(pq) == 0);
	free_one(pq);
}

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	printf("\n\npriority queue\n\n");
	MU_RUN_TEST(test_create);
	MU_RUN_TEST(test_is_empty);
	MU_RUN_TEST(test_access_is_empty);
	MU_RUN_TEST(test_add_with_priority);
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
