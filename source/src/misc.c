/* txbmisc.c -- Miscellaneous functions -- Troy Brumley BlameTroi@gmail.com */

/*
 * This is a header only implementation of various bits of code that I
 * keep repeating in my hobby programming that I want around without
 * the hassle of managing library dependencies. All functions are
 * small and I think pretty obvious.
 *
 * Some of the numeric functions use longs instead of ints to deal
 * with some of the large numbers seen in problems from Advent of Code
 * and other puzzle sites.
 *
 * Released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * This software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/misc.h"

/*
 * Various min/max functions. The typed functions are wrapped by
 * generic macros. The macros only type check the first argument to
 * determine which function to call. It's the client's responsibility
 * to ensure that the arguments compatible.
 *
 * Use the macros `min' and `max' (defined in txbmisc.h) instead of
 * the following functions.
 */

int
i_max(int x, int y) {
	return x > y ? x : y;
}

int
i_min(int x, int y) {
	return x < y ? x : y;
}

long
l_max(long x, long y) {
	return x > y ? x : y;
}

long
l_min(long x, long y) {
	return x < y ? x : y;
}

unsigned int
ui_max(unsigned int x, unsigned int y) {
	return x > y ? x : y;
}

unsigned int
ui_min(unsigned int x, unsigned int y) {
	return x < y ? x : y;
}

unsigned long
ul_max(unsigned long x, unsigned long y) {
	return x > y ? x : y;
}

unsigned long
ul_min(unsigned long x, unsigned long y) {
	return x < y ? x : y;
}

float
f_max(float x, float y) {
	return x > y ? x : y;
}

float
f_min(float x, float y) {
	return x > y ? x : y;
}

double
d_max(double x, double y) {
	return x > y ? x : y;
}

double
d_min(double x, double y) {
	return x < y ? x : y;
}

/*
 * is_even & is_odd
 *
 *     in: a signed integer that promotes to a long
 *
 * return: bool
 */

bool
is_even(long n) {
	return !(n & 1);
}

bool
is_odd(long n) {
	return (n & 1);
}

/*
 * is_* various character predicates
 *
 * Quick character classification from the point of view of this
 * US-ASCII based programmer.
 *
 *     in: a char
 *
 * return: bool
 *
 * Whether or not a hyphen is a word character (hypen, dash, em-dash)
 * or a mathematical symbol and other such edge cases are not
 * accounted for here. These definitions work for 99% of the things I
 * am likely to do.
 *
 * is_digit          0-9
 * is_word_char      alphabetic and underscore
 * is_lowercase      a-z
 * is_uppercase      A-Z
 * is_whitespace     space, cr, lf, ff, tab
 * is_control        0x00->0x1f
 * is_punctuation    .,?!;:
 * is_bracketing     [](){}
 */

bool
is_digit(char c) {
	return c >= '0' && c <= '9';
}

bool
is_word_char(char c) {
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

bool
is_lowercase(char c) {
	return c >= 'a' && c <= 'z';
}

bool
is_uppercase(char c) {
	return c >= 'A' && c <= 'Z';
}

bool
is_whitespace(char c) {
	return c == ' ' ||
		c == '\n' ||
		c == '\f' ||
		c == '\r' ||
		c == '\t';
}

bool
is_control(char c) {
	return c > '\0' && c < ' ';
}

bool
is_punctuation(char c) {
	return c == '.' ||
		c == ',' ||
		c == '?' ||
		c == '!' ||
		c == ';' ||
		c == ':';
}

bool
is_bracketing(char c) {
	return c == '[' ||
		c == '(' ||
		c == '{' ||
		c == '}' ||
		c == ')' ||
		c == ']';
}

/*
 * one_bits_in
 *
 * Brian Kernighan's algorithm for counting set bits in a variable.
 *
 *     in: an unsigned long
 *
 * return: int number of bits set.
 */

int
one_bits_in(unsigned long n) {
	int count = 0;
	while (n) {
		n = n & (n-1);
		count += 1;
	}
	return count;
}

/*
 * sum_one_to
 *
 * sum the integers 1 to n as Gauss would.
 *
 *     in: an int
 *
 * return: 1 + 2 + ... + n
 */

long
sum_one_to(long n) {
	return (n * (n + 1)) / 2;
}

/*
 * Some common comparator functions for things like `qsort'.
 */

int
fn_cmp_int_asc(const void *a, const void *b) {
	return *(int *)a - *(int *)b;
}

int
fn_cmp_int_dsc(const void *a, const void *b) {
	return *(int *)b - *(int *)a;
}

/*
 * factors_of
 *
 * Returns an array of long integers big enought to at least hold the
 * factors of 'n' and a trailing NULL. The caller is responsible for
 * freeing the array when it is no longer needed.
 *
 *     in: a long integer 'n'
 *
 * return: the array as above, or NULL if 'n' < 1
 */

long *
factors_of(
	long n
) {
	if (n < 1)
		return NULL;

	/* allocate space for a dynamically sized array of longs.
	 * starting size is 64 items, of which the last is reserved
	 * for a trailing NULL value. grows by doubling, but that
	 * should be rarely needed. there are 64 factors of 999,999
	 * and 49 of 1,000,000. */
	int lim = 64;
	long *factors = calloc(lim, sizeof(*factors));
	int f = 0;

	/* just count up from 1 to half, tack on n, and we're done. */
	long i = 1;
	long half = n / 2;
	while (i <= half) {
		if (n % i == 0) {
			factors[f] = i;
			f += 1;

			/* if we're approaching the end of the allocation,
			 * we know we need at least one more item so
			 * grow the array. */
			if (f + 2 >= lim) {
				long *grow = calloc(lim * 2, sizeof(*factors));
				memcpy(grow, factors, lim * sizeof(*factors));
				free(factors);
				factors = grow;
				lim = lim * 2;
			}
		}
		i += 1;
	}

	factors[f] = n;
	return factors;
}

/*
 * hex_pack hex_unpack
 *
 * Convert run of bytes to displayable hex digits (unpack hex) or a
 * string of hex digits to bytes (pack hex).
 *
 * Returns the address of the first byte of the output buffer so the
 * function can be used as an argument to printf. Returns NULL if any
 * error in arguments is detected.
 *
 * The function arguments parallel each other.
 *
 *     in: first byte of output buffer
 *
 *     in: maximum length of output buffer
 *
 *     in: first byte of input buffer
 *
 *     in: maximum length of output buffer
 *
 * return: first byte of output buffer
 */

uint8_t *
hex_pack(
	uint8_t *hex,
	int hexlen,
	char *chr,
	int chrlen
) {
	if (!hex || hexlen * 2 < chrlen || is_odd(chrlen) || !chr)
		return NULL;
	memset(hex, 0, hexlen);
	char *c = chr;
	uint8_t *h = hex;
	for (int i = 0; i < hexlen; i++) {
		uint8_t dh = 0;
		if (*c >= '0' && *c <= '9')
			dh = *c - '0';
		else if (*c >= 'a' && *c <= 'f')
			dh = *c - 'a' + 10;
		else if (*c >= 'A' && *c <= 'F')
			dh = *c - 'A' + 10;
		else
			return NULL;
		c += 1;
		uint8_t dl = 0;
		if (*c >= '0' && *c <= '9')
			dl = *c - '0';
		else if (*c >= 'a' && *c <= 'f')
			dl = *c - 'a' + 10;
		else if (*c >= 'A' && *c <= 'F')
			dl = *c - 'A' + 10;
		else
			return NULL;
		c += 1;
		*h = (dh << 4) | dl;
		h += 1;
	}
	return hex;
}

char *
hex_unpack(
	char *chr,
	int chrlen,
	uint8_t *hex,
	int hexlen
) {
	if (!chr || chrlen < 2 * hexlen + 1 || !hex)
		return NULL;
	memset(chr, 0, chrlen);
	char *c = chr;
	uint8_t *h = hex;
	for (int i = 0; i < hexlen; i++) {
		*c = "0123456789abcdef"[*h >> 4];
		c += 1;
		*c = "0123456789abcdef"[*h & 15];
		c += 1;
		h += 1;
	}
	return chr;
}

/* txbmisc.c ends here */
