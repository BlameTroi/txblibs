/* txbrand.h -- Random number support -- Troy Brumley BlameTroi@gmail.com */

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
 * random some number of some sided dice (eg, 3d6).
 */

int
random_dice(
	int num,
	int sides
);

/*
 * shuffle an array of n items using the fisher-yates algorithm.
 */

void
shuffle(
	void **cards,
	int n
);

/*
 * random_lower/upper/digit/special
 *
 * return a randomly selected character from the requested set
 * following us ascii conventions.
 */

char
random_lower(
	void
);

char
random_upper(
	void
);

char
random_digit(
	void
);

char
random_special(
	void
);

/*
 * bit flags controlling which character sets to retrieve a random
 * character from.
 *
 * the distribution of characters is even throughout, which does
 * not mimic the distribution in real text samples.
 */

#define RAND_CHAR_LOWER (1 << 0)
#define RAND_CHAR_UPPER (1 << 1)
#define RAND_CHAR_DIGIT (1 << 2)
#define RAND_CHAR_SPECIAL (1 << 3)
#define RAND_CHAR_ALL (RAND_CHAR_LOWER | RAND_CHAR_UPPER | RAND_CHAR_DIGIT | RAND_CHAR_SPECIAL)
/*
 * return one random character one or more of the standard character
 * sets of lower case letters, upper case letters, decimal digits, and
 * special characters.
 */

char
random_character_from(
	int pool
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* txbrand.h ends here */
