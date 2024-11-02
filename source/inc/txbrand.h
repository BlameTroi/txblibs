/* txbrand.h -- random number suppport header library */

/*
 * this is a header only implementation of non cryptographic quality
 * random number support that i keep repeating in my hobby
 * programming. all functions are small and i think pretty obvious.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TXBRAND_H
#define TXBRAND_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * set which random generator to use. repeatable uses rand() and can
 * be seeded with the seed value to srand(). non-repeatable uses
 * arc4random() and siblings.
 *
 * arc4random() is only available with _DARWIN_C_SOURCE defined.
 */

#define RAND_DEFAULT 0
#define RAND_RANDOM 1

bool
set_random_generator(
	int which
);

bool
seed_random_generator(
	uint32_t seed
);

/*
 * return a probably non-cyptography safe pseudo random unsigned
 * integer in an inclusive range.
 */

uint32_t
random_between(
	uint32_t,
	uint32_t
);

/*
 * shuffle an array of n items using the fisher-yates algorithm.
 */

void
shuffle(
	void **cards,
	int n
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBRAND_H */

#ifdef TXBRAND_IMPLEMENTATION
#undef TXBRAND_IMPLEMENTATION

/*
 * frequently used random number related functions. these are not meant
 * to be cryptographically secure. use of the old style deterministic
 * rand() and srand() are supported for repeatable testing, and a
 * better generator using arc4random() is available if needed.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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
	//#ifdef _DARWIN_C_SOURCE
	return arc4random_uniform(high + 1 - low) + low;
	//#else
	//      return rand() % (high + 1 - low) + low;
	//#endif /* _DARWIN_C_SOURCE */
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
#endif /* TXBRAND_IMPLEMENTATION */
/* txbrand.h ends here */
