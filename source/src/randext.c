/* randext.c -- working on extensions to txbrand -- */

/*
 * i find myself wanting random text, so i write a generator.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */
//#include <assert.h>
#include <stdbool.h>
//#include <stdio.h>
#include "txbrand.h"

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
random_dice(int num, int sides) {
	int res = 0;
	while (num > 0) {
		res += random_between(1, sides);
		num -= 1;
	}
	return res;
}

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
random_character_from(int pool) {
	if (pool == 0) return '\0';

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

/* randext.c ends here */
