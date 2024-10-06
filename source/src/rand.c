/* rand.c -- blametroi's common utility functions -- */

/*
 * frequently used random number related functions. these are not meant
 * to be cryptographically secure. use of the old style deterministic
 * rand() and srand() are supported for repeatable testing, and a
 * better generator using arc4random() is available if needed.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdlib.h>

#include "rand.h"


/*
 * should the deterministic rand() or non-deterministic arc4random()
 * generators be used?
 */

static
bool
use_rand = true;


/*
 * set either stdlib's rand() or arc4random() as the generator to use.
 */

bool
set_random_generator(
	int which
) {
	switch (which) {
	case RAND_DEFAULT:
		use_rand = true;
		return true;
	case RAND_RANDOM:
		use_rand = false;
		return true;
	default:
		return false;
	}
}


/*
 * reseed rand() using srand(). if rand is not being used this has
 * no effect.
 */

bool
seed_random_generator(
	uint32_t seed
) {
	if (use_rand) {
		srand(seed);
		return true;
	}
	return false;
}


/*
 * generate a pseudo random integer between low and high inclusive.
 *
 * when using rand(), this isn't really secure randomness, but it's
 * suitable for many purposes. the idea is from
 * https://stackoverflow.com/a/1202706
 *
 * when using arc4rand_uniform(), the quality of randomness is better.
 */

uint32_t
random_between(
	uint32_t low,
	uint32_t high
) {
	if (use_rand)
		return rand() % (high + 1 - low) + low;

	return arc4random_uniform(high + 1 - low) + low;
}


/*
 * shuffle an array of items using the fisher-yates algorithm. the
 * array is updated in place. by using an array of void pointers, any
 * objects can be shuffled.
 *
 * uses random_between() which is deterministic by default using rand().
 * use set_random_generator() to select between the deterministic rand() and
 * non-deterministic arc4random(). seeding for rand() can be done via
 * seed_random_generator().
 */

void
shuffle(
	void **cards,
	int n
) {
	int i = n;
	while (i > 0) {
		int r = random_between(1, i);
		void *s = cards[r-1];
		cards[r-1] = cards[i-1];
		cards[i-1] = s;
		i -= 1;
	}
}
