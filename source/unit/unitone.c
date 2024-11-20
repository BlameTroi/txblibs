/* unitlist.c -- tests for the my list library -- troy brumley */

/* released to the public domain, troy brumley, may 2024 */

#include <string.h>
#include "minunit.h"
#include "txballoc.h"
#include "txbstr.h"
#include "txbone.h"

/*
 * minunit setup and teardown.
 */

#define RAND_SEED 6803

void
test_setup(void) {
	/*
	 * #define txballoc_f_allocs    (1 << 0)
	 * #define txballoc_f_frees     (1 << 1)
	 * #define txballoc_f_dup_frees (1 << 2)
	 * #define txballoc_f_leaks     (1 << 3)
	 * #define txballoc_f_silent    (0)
	 * #define txballoc_f_trace     (txballoc_f_allocs + txballoc_f_frees)
	 * #define txballoc_f_errors    (txballoc_f_dup_frees + txballoc_f_leaks)
	 * #define txballoc_f_full      (txballoc_f_trace + txballoc_f_errors)
	 */

	/* log to stderr to better segregate output */
	tsinitialize(4000, txballoc_f_errors, stderr);
}

void
test_teardown(void) {
	tsterminate();
}

/*
 * test api wiring for each structured data type. are the expected
 * entry points available? do the wrong entry points fail gracefully?
 *
 * functions that do not otherwise return a value instead return
 * their first parameter, as an aid to chaining. while NULL is used
 * to represent no value found on get and peek, functions called for
 * the wrong type of sdt will also return NULL and log to stderr. this
 * allows for some limited wiring tests here.
 */

MU_TEST(test_api_singly) {
	/* make_one, add_first, add_last, peek_, get_, count, empty, purge, free_ */
	one_block *ob = make_one(singly);
	mu_should(empty(ob));
	mu_shouldnt(count(ob));
	mu_should(ob == add_first(ob, "first"));
	mu_should(ob == add_last(ob, "last"));
	mu_should(ob == free_one(ob));
	ob = NULL;
	ob = make_one(singly);
	char *p = "this is a test ...";
	for (int i = 0; i < strlen(p); i++) {
		add_first(ob, p);
		mu_should(equal_string(peek_first(ob), p));
		p += 1;
	}
	int c = count(ob);
	mu_shouldnt(c == depth(ob));
	mu_shouldnt(peek(ob));
	mu_shouldnt(pop(ob));
	printf("\n%d\n", count(ob));
	mu_should(peek_first(ob));
	mu_should(peek_last(ob));
	mu_should(get_first(ob));
	mu_should(get_last(ob));
	mu_shouldnt(enqueue(ob, "fred"));
	mu_shouldnt(dequeue(ob));
	mu_shouldnt(pop_front(ob));
	mu_shouldnt(push_front(ob, "wilma"));
	mu_should(purge(ob) > 0);
	free_one(ob);
}

MU_TEST(test_api_doubly) {
	/* make_one, add_first, add_last, peek_, get_, count, empty, purge, free_ */
	one_block *ob = make_one(doubly);
	mu_should(empty(ob));
	mu_shouldnt(count(ob));
	mu_should(ob == add_first(ob, "first"));
	mu_should(ob == add_last(ob, "last"));
	mu_should(ob == free_one(ob));
	ob = NULL;
	ob = make_one(singly);
	char *p = "this is a test ...";
	for (int i = 0; i < strlen(p); i++) {
		add_first(ob, p);
		mu_should(equal_string(peek_first(ob), p));
		p += 1;
	}
	int c = count(ob);
	mu_shouldnt(c == depth(ob));
	mu_shouldnt(peek(ob));
	mu_shouldnt(pop(ob));
	printf("\n%d\n", count(ob));
	mu_should(peek_first(ob));
	mu_should(peek_last(ob));
	mu_should(get_first(ob));
	mu_should(get_last(ob));
	mu_shouldnt(enqueue(ob, "fred"));
	mu_shouldnt(dequeue(ob));
	mu_shouldnt(pop_front(ob));
	mu_shouldnt(push_front(ob, "wilma"));
	mu_should(purge(ob) > 0);
	free_one(ob);
}

MU_TEST(test_api_stack) {
	/* make, peek, depth, empty, purge, push, pop, free */
	one_block *ob = make_one(stack);
	mu_should(depth(ob) == 0);
	mu_should(empty(ob));
	mu_shouldnt(add_first(ob, "test"));
	mu_should(push(ob, "one"));
	mu_should(push(ob, "two"));
	mu_should(depth(ob) == 2);
	mu_should(count(ob) < 0);
	mu_shouldnt(dequeue(ob));
	mu_should(depth(ob) == 2);
	mu_should(equal_string(pop(ob), "two"));
	purge(ob);
	free_one(ob);
}

MU_TEST(test_api_queue) {
	/* make, peek, count, empty, purge, enqueue, dequeue, free */
	one_block *ob = make_one(queue);
	mu_should(depth(ob) < 0);
	mu_should(count(ob) == 0);
	enqueue(ob, "one");
	enqueue(ob, "two");
	mu_shouldnt(push_front(ob, "three"));
	mu_shouldnt(pop_back(ob));
	mu_should(count(ob) == 2);
	mu_shouldnt(empty(ob));
	mu_should(equal_string(dequeue(ob), "one"));
	mu_should(equal_string(peek(ob), "two"));
	mu_shouldnt(peek_last(ob));
	mu_shouldnt(peek_front(ob));
	purge(ob);
	free_one(ob);
}

MU_TEST(test_api_deque) {
	/* make, push_back/front, pop_, peek_, count, empty, purge, free */
	one_block *ob = make_one(deque);
	mu_should(depth(ob) < 0);
	push_front(ob, "one");
	push_back(ob, "two");
	mu_should(count(ob) == 2);
	mu_shouldnt(enqueue(ob, "three"));
	mu_shouldnt(dequeue(ob));
	mu_shouldnt(peek(ob));
	mu_should(equal_string(peek_front(ob), "one"));
	mu_should(equal_string(peek_back(ob), "two"));
	mu_should(equal_string(pop_back(ob), "two"));
	purge(ob);
	free_one(ob);
}

MU_TEST(test_api_dynarray) {
	/* make, purge, free, sort(not yet), high_index, get_at, put_at */
	one_block *ob = make_one(dynarray);
	mu_should(ob);
	mu_should(high_index(ob) < 0);   /* nothing written yet */
	mu_should(put_at(ob, "fred", 1));
	mu_should(equal_string(get_from(ob, 1), "fred"));
	mu_shouldnt(get_from(ob, 10));
	mu_shouldnt(get_first(ob));      /* illegal op */
	mu_should(purge(ob) < 0);        /* illegal op */
	mu_should(free_one(ob));
}

MU_TEST(test_api_keyval) {
	one_block *ob = make_one(keyval);
	mu_shouldnt(ob);
	/* tbd */
}

MU_TEST(test_api_bst) {
	one_block *ob = make_one(bst);
	mu_shouldnt(ob);
	/* tbd */
}

MU_TEST(test_api_hash) {
	one_block *ob = make_one(hash);
	mu_shouldnt(ob);
	/* determine insert/delete/find or put/reove/get */
	/* make, free, purge, count, insert/put, delete/remove, find/get, keys, values */
}

/*
 * singly linked lists are also the basis for stacks (until i get
 * around to switching to doubly linked list backing). peek iterators
 * are needed.
 */

MU_TEST(test_singly_firsts) {
	one_block *ob = make_one(singly);

	mu_should(peek_first(ob) == NULL);
	add_first(ob, "free");
	mu_should(equal_string(peek_first(ob), "free"));
	char *first = get_first(ob);
	mu_should(equal_string(first, strdup("free")));
	char *second = get_first(ob);
	mu_should(second == NULL);

	free_one(ob);
}

MU_TEST(test_singly_count_purge) {
	one_block *ob = make_one(singly);

	mu_should(count(ob) == 0);
	mu_should(purge(ob) == 0);
	mu_should(empty(ob));
	add_first(ob, "free");
	mu_should(count(ob) == 1);
	add_first(ob, "beer");
	mu_should(count(ob) == 2);
	mu_should(equal_string(peek_first(ob), "beer"));
	mu_should(count(ob) == 2);
	char *s = get_first(ob);
	mu_should(equal_string(s, "beer"));
	mu_should(count(ob) == 1);
	add_first(ob, s);
	add_first(ob, "wine");
	add_first(ob, "scotch");
	mu_should(count(ob) == 4);
	mu_shouldnt(empty(ob));
	mu_should(purge(ob) == 4);
	mu_should(empty(ob));

	free_one(ob);
}

MU_TEST(test_singly_lasts) {
	one_block *ob = make_one(singly);

	mu_should(empty(ob));
	add_first(ob, "one");
	add_first(ob, "two");
	add_first(ob, "three");
	add_first(ob, "four");
	mu_should(count(ob) == 4);
	mu_should(equal_string(peek_first(ob), "four"));
	mu_should(equal_string(peek_last(ob), "one"));
	mu_should(equal_string(get_first(ob), "four"));
	mu_should(equal_string(get_first(ob), "three"));
	mu_should(equal_string(get_first(ob), "two"));
	mu_should(equal_string(get_first(ob), "one"));
	mu_shouldnt(get_first(ob));
	mu_shouldnt(get_last(ob));

	mu_should(empty(ob));
	add_last(ob, "one");
	add_last(ob, "two");
	add_last(ob, "three");
	add_last(ob, "four");
	mu_should(count(ob) == 4);
	mu_should(equal_string(peek_first(ob), "one"));
	mu_should(equal_string(peek_last(ob), "four"));
	mu_should(equal_string(get_first(ob), "one"));
	mu_should(equal_string(get_first(ob), "two"));
	mu_should(equal_string(get_first(ob), "three"));
	mu_should(equal_string(get_first(ob), "four"));
	mu_shouldnt(get_first(ob));
	mu_shouldnt(get_last(ob));

	mu_should(empty(ob));
	add_first(ob, "one");             /* 1 */
	add_last(ob, "two");              /* 1 2 */
	mu_should(count(ob) == 2);
	add_first(ob, "three");           /* 3 1 2 */
	add_last(ob, "four");             /* 3 1 2 4 */
	mu_should(count(ob) == 4);
	mu_should(equal_string(peek_first(ob), "three"));
	mu_should(equal_string(peek_last(ob), "four"));
	mu_should(equal_string(get_first(ob), "three"));
	mu_should(equal_string(get_first(ob), "one"));
	mu_should(equal_string(get_first(ob), "two"));
	mu_should(equal_string(get_first(ob), "four"));

	/* unbelievable, i didn't test get_last? wtf? */
	mu_should(count(ob) == 0);
	add_first(ob, "one");
	mu_should(equal_string(get_last(ob), "one"));
	add_first(ob, "one");
	add_first(ob, "two");
	add_first(ob, "three");
	mu_should(equal_string(get_last(ob), "one"));
	mu_should(equal_string(get_last(ob), "two"));
	mu_should(equal_string(get_last(ob), "three"));
	mu_should(empty(ob));

	free_one(ob);
}

/*
 * stacks are implemented over singly linked lists, and exercise
 * only a subset of singly linked list functions.
 */

MU_TEST(test_stack) {
	one_block *ob = make_one(stack);
	mu_should(empty(ob));

	char *testing[] = { "one", "two", "three", "four", "five", NULL};
	int i = 0;
	while (testing[i]) {
		push(ob, testing[i]);
		i += 1;
	}

	mu_should(depth(ob) == 5);
	mu_should(equal_string(pop(ob), "five"));
	mu_should(equal_string(peek(ob), "four"));
	mu_should(depth(ob) == 4);
	mu_should(equal_string(pop(ob), "four"));
	mu_should(equal_string(pop(ob), "three"));
	mu_should(equal_string(pop(ob), "two"));
	mu_should(equal_string(pop(ob), "one"));
	mu_shouldnt(pop(ob));
	mu_should(empty(ob));

	free_one(ob);
}

/*
 * doubly linked lists parallel singly linked lists. both still lack peek
 * iterators.
 */

MU_TEST(test_doubly_firsts) {
	one_block *ob = make_one(doubly);

	mu_should(peek_first(ob) == NULL);
	add_first(ob, "free");
	mu_should(equal_string(peek_first(ob), "free"));
	char *first = get_first(ob);
	mu_should(equal_string(first, strdup("free")));
	char *second = get_first(ob);
	mu_should(second == NULL);

	free_one(ob);
}
MU_TEST(test_doubly_count_purge) {
	one_block *ob = make_one(doubly);

	mu_should(count(ob) == 0);
	mu_should(purge(ob) == 0);
	mu_should(empty(ob));
	add_first(ob, "free");
	mu_should(count(ob) == 1);
	add_first(ob, "beer");
	mu_should(count(ob) == 2);
	mu_should(equal_string(peek_first(ob), "beer"));
	mu_should(count(ob) == 2);
	char *s = get_first(ob);
	mu_should(equal_string(s, "beer"));
	mu_should(count(ob) == 1);
	add_first(ob, s);
	add_first(ob, "wine");
	add_first(ob, "scotch");
	mu_should(count(ob) == 4);
	mu_shouldnt(empty(ob));
	mu_should(purge(ob) == 4);
	mu_should(empty(ob));

	free_one(ob);
}

MU_TEST(test_doubly_lasts) {
	one_block *ob = make_one(doubly);

	mu_should(empty(ob));
	add_first(ob, "one");
	add_first(ob, "two");
	add_first(ob, "three");
	add_first(ob, "four");
	mu_should(count(ob) == 4);
	mu_should(equal_string(peek_first(ob), "four"));
	mu_should(equal_string(peek_last(ob), "one"));
	mu_should(equal_string(get_first(ob), "four"));
	mu_should(equal_string(get_first(ob), "three"));
	mu_should(equal_string(get_first(ob), "two"));
	mu_should(equal_string(get_first(ob), "one"));
	mu_shouldnt(get_first(ob));
	mu_shouldnt(get_last(ob));

	mu_should(empty(ob));
	add_last(ob, "one");
	add_last(ob, "two");
	add_last(ob, "three");
	add_last(ob, "four");
	mu_should(count(ob) == 4);
	mu_should(equal_string(peek_first(ob), "one"));
	mu_should(equal_string(peek_last(ob), "four"));
	mu_should(equal_string(get_first(ob), "one"));
	mu_should(equal_string(get_first(ob), "two"));
	mu_should(equal_string(get_first(ob), "three"));
	mu_should(equal_string(get_first(ob), "four"));
	mu_shouldnt(get_first(ob));
	mu_shouldnt(get_last(ob));

	mu_should(empty(ob));
	add_first(ob, "one");             /* 1 */
	add_last(ob, "two");              /* 1 2 */
	mu_should(count(ob) == 2);
	add_first(ob, "three");           /* 3 1 2 */
	add_last(ob, "four");             /* 3 1 2 4 */
	mu_should(count(ob) == 4);
	mu_should(equal_string(peek_first(ob), "three"));
	mu_should(equal_string(peek_last(ob), "four"));
	mu_should(equal_string(get_first(ob), "three"));
	mu_should(equal_string(get_first(ob), "one"));
	mu_should(equal_string(get_first(ob), "two"));
	mu_should(equal_string(get_first(ob), "four"));

	/* unbelievable, i didn't test get_last? wtf? */
	mu_should(count(ob) == 0);
	add_first(ob, "one");
	mu_should(equal_string(get_last(ob), "one"));
	add_first(ob, "one");
	add_first(ob, "two");
	add_first(ob, "three");
	mu_should(equal_string(get_last(ob), "one"));
	mu_should(equal_string(get_last(ob), "two"));
	mu_should(equal_string(get_last(ob), "three"));
	mu_should(empty(ob));


	free_one(ob);
}

MU_TEST(test_trailing_links) {
	one_block *ob = make_one(doubly);

	char *td[] = {
		"a", "b", "c", "d",
		"e", "f", "g", "h",
		"i", "j", "k", "l",
		"m", "n", "o", "p",
		"q", "r", "s", "t",
		"u", "v", "w", "x",
		"y", "z", NULL
	};

	int i = 0;
	while (td[i]) {
		add_first(ob, td[i]);
		i += 1;
	}
	mu_should(count(ob) == 26);

	i = 0;
	while (!empty(ob)) {
		char *p = get_last(ob);
		/* printf("%s\n", p); */
		mu_should(equal_string(td[i], p));
		i += 1;
	}
	mu_should(count(ob) == 0);

	i = 0;
	while (td[i]) {
		add_last(ob, td[i]);
		i += 1;
	}
	mu_should(count(ob) == 26);

	i = 0;
	while (!empty(ob)) {
		char *p = get_first(ob);
		/* printf("%s\n", p); */
		mu_should(equal_string(td[i], p));
		i += 1;
	}
	mu_should(count(ob) == 0);

	i = 0;
	while (td[i]) {
		if (i & 1) add_first(ob, td[i]);
		else add_last(ob, td[i]);
		i += 1;
	}
	mu_should(count(ob) == 26);

	printf("\nreversed alphabet: ");
	i = 26;
	while (i > 0) {
		if (i & 1) printf("%s", (char*)get_last(ob));
		else printf("%s", (char*)get_first(ob));
		i -= 1;
	}
	printf("\n");
	mu_should(count(ob) == 0);

	free_one(ob);
}

/*
 * a queue (fifo) is implemented on top of a doubly linked list. as
 * those have been tested already, not too much needed here.
 */

MU_TEST(test_queue) {
	one_block *ob = make_one(queue);

	enqueue(ob, "one");
	enqueue(ob, "two");
	enqueue(ob, "three");

	mu_should(count(ob) == 3);

	mu_should(equal_string(peek(ob), "one"));
	mu_should(count(ob) == 3);
	mu_should(equal_string(dequeue(ob), "one"));
	mu_should(equal_string(dequeue(ob), "two"));
	mu_should(equal_string(dequeue(ob), "three"));
	mu_shouldnt(dequeue(ob));
	mu_shouldnt(peek(ob));

	free_one(ob);
}

/*
 * a deque (double ended queue) is implemented on top of a doubly
 * linked list. as those have been tested already, not too much needed
 * here.
 */

MU_TEST(test_deque) {
	one_block *ob = make_one(deque);

	push_front(ob, "one");
	push_front(ob, "two");
	push_front(ob, "three");

	mu_should(count(ob) == 3);

	mu_should(equal_string(peek_back(ob), "one"));
	mu_should(equal_string(peek_front(ob), "three"));
	mu_should(count(ob) == 3);
	mu_should(equal_string(pop_back(ob), "one"));
	mu_should(equal_string(pop_back(ob), "two"));
	mu_should(equal_string(pop_back(ob), "three"));
	mu_shouldnt(pop_front(ob));
	mu_shouldnt(peek_back(ob));

	push_front(ob, "one");
	push_back(ob, "two");
	push_front(ob, "three");
	push_back(ob, "four");

	mu_should(count(ob) == 4);
	mu_shouldnt(empty(ob));
	mu_should(purge(ob) == 4);
	mu_should(empty(ob));

	free_one(ob);
}

/*
 * dynamic arrray.
 */

MU_TEST(test_dynarray) {
	one_block *ob = make_one(dynarray);
	/* default capacity is 512 */
	for (long i = 0; i < 4096; i++) {
		put_at(ob, (void *)(i * -1), i);
	}
	printf("\n");
	for (long i = 500; i < 700; i++) {
		long v = (long)get_from(ob, i);
		printf("%ld", v);
	}
	printf("\n");
	mu_should(high_index(ob) == 4095);
	free_one(ob);
}

/*
 * hook up the tests
 */

MU_TEST_SUITE(test_suite) {

	MU_SUITE_CONFIGURE(test_setup, test_teardown);

	/* test that apis are wired up correctly and that only the
	 * expected exposed apis are usable. */

	MU_RUN_TEST(test_api_singly);
	MU_RUN_TEST(test_api_doubly);
	MU_RUN_TEST(test_api_stack);
	MU_RUN_TEST(test_api_queue);
	MU_RUN_TEST(test_api_deque);
	MU_RUN_TEST(test_api_dynarray);
	MU_RUN_TEST(test_api_keyval);
	MU_RUN_TEST(test_api_bst);
	MU_RUN_TEST(test_api_hash);

	/* deeper functionality tests, starting with the underpinning
	 * linked lists. */

	MU_RUN_TEST(test_singly_firsts);
	MU_RUN_TEST(test_singly_count_purge);
	MU_RUN_TEST(test_singly_lasts);

	MU_RUN_TEST(test_doubly_firsts);
	MU_RUN_TEST(test_doubly_count_purge);
	MU_RUN_TEST(test_doubly_lasts);

	MU_RUN_TEST(test_queue);

	MU_RUN_TEST(test_deque);

	MU_RUN_TEST(test_stack);

	MU_RUN_TEST(test_dynarray);

	/* throw a little volume at link chaining */

	MU_RUN_TEST(test_trailing_links);

	return;
}

int
main(int argc, char *argv[]) {
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}
/* unitlist.c ends here */
