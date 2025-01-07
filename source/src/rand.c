/* txbrand.c -- Random number support -- Troy Brumley BlameTroi@gmail.com */

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

/*
 * frequently used random number related functions. these are not meant
 * to be cryptographically secure. use of the old style deterministic
 * rand() and srand() are supported for repeatable testing, and a
 * better generator using arc4random() is available if needed.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "../inc/rand.h"

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
 * random_dice
 *
 * roll some number of dice.
 *
 *     in: integer number of dice
 *
 *     in: integer number of sides on each die
 *
 * return: integer roll of num*sides.
 */

int
random_dice(
	int num,
	int sides
) {
	int res = 0;
	while (num > 0) {
		res += random_between(1, sides);
		num -= 1;
	}
	return res;
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

/*
 * the base character groupings.
 */

static const char lowers[] = {
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z'
};
#define NUM_LOWERS 26

static const char uppers[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z'
};
#define NUM_UPPERS 26

static const char digits[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};
#define NUM_DIGITS 10

static const char specials[] = {
	'\'', '"', '\\', '!', '@', '#', '$', '%', '^', '&',
	'*', '(', ')', '-', '_', '=', '+', '[', ']',
	'{', '}', '|', ';', ':', ',', '.', '<', '>',
	'`', '~', ' ', '/', '?'
};
#define NUM_SPECIALS 33

/*
 * random_lower/upper/digit/special
 *
 * return a randomly selected character from the requested set
 * following us ascii conventions.
 */

char
random_lower(void) {
	return lowers[random_between(0, NUM_LOWERS-1)];
}

char
random_upper(void) {
	return uppers[random_between(0, NUM_UPPERS-1)];
}

char
random_digit(void) {
	return digits[random_between(0, NUM_DIGITS-1)];
}

char
random_special(void) {
	return specials[random_between(0, NUM_SPECIALS-1)];
}

/*
 * random_character_from
 *
 * return one random character one or more of the standard character
 * sets of lower case letters, upper case letters, decimal digits, and
 * special characters.
 *
 *     in: bit flags RAND_CHAR_?
 *
 * return: a character
 *
 * the distribution of characters is even throughout, which does
 * not mimic the distribution in real text samples.
 */

char
random_character_from(
	int pool
) {
	if (!(pool & RAND_CHAR_ALL))
		return '\0';

	int range = 0;
	if (pool & RAND_CHAR_LOWER) range += NUM_LOWERS;
	if (pool & RAND_CHAR_UPPER) range += NUM_UPPERS;
	if (pool & RAND_CHAR_DIGIT) range += NUM_DIGITS;
	if (pool & RAND_CHAR_SPECIAL) range += NUM_SPECIALS;

	int rand = random_between(1, range);

	if (pool & RAND_CHAR_LOWER && rand <= NUM_LOWERS) {
		return lowers[rand-1];
	}
	if (pool & RAND_CHAR_LOWER) {
		range -= NUM_LOWERS;
		rand -= NUM_LOWERS;
	}

	if (pool & RAND_CHAR_UPPER && rand <= NUM_UPPERS) {
		return uppers[rand-1];
	}
	if (pool & RAND_CHAR_UPPER) {
		range -= NUM_UPPERS;
		rand -= NUM_UPPERS;
	}

	if (pool & RAND_CHAR_DIGIT && rand <= NUM_DIGITS) {
		return digits[rand-1];
	}
	if (pool & RAND_CHAR_DIGIT) {
		range -= NUM_DIGITS;
		rand -= NUM_DIGITS;
	}

	return specials[rand-1];
}

/* txbrand.c ends here */
