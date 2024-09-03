/* rand.h -- blametroi's common utility functions -- */

/*
 * this is a header only implementation of non cryptographic quality
 * random number support that i keep
 * repeating in my hobby programming. all functions are
 * small and i think pretty obvious.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * set which random generator to use. repeatable uses rand() and can
 * be seeded with the seed value to srand(). non-repeatable uses
 * arc4random() and siblings.
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
