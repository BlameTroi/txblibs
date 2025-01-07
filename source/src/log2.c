/* txblog2.c -- fast integer log base(2) -- Troy Brumley BlameTroi@gmail.com */

/*
 * This is a header only implementation of a fast integer log base 2
 * function for 32 bit integers. It originally comes from:
 *
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
 *
 * But I've tweaked some naming and formatting. I don't claim the
 * algorithm, just this particular implementation, which as always
 * I have --
 * -- released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * This software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

/*
 * Implementation notes:
 *
 * See the following url for the original and so much more:
 *
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
 *
 * I had to make three changes, after which I felt free to do some
 * reformatting of the original code to match my sense of esthetics.
 *
 * 1) For an unknown (to me) reason, having the `#define' followed by
 *    immediately by log table gets either an error complaining about
 *    the `include' of `stdint', or one about an empty translation
 *    unit on the type specifier for `log_table_256'. "ISO C requires
 *    that a translation unit have at least one declaration" but there
 *    is a declaration.
 *
 *    While in WTF mode I put an `#include' between the `#define'
 *    and the table. Hey presto! the error goes away.
 *
 *    I expect this is some problem with my configuration of `clangd'
 *    because a straight compile is clean.
 *
 * 2) In order to avoid the "include not directly used" pedantic
 *    warning, I changed `unsigned int' to `uint32_t' and `char' to
 *    `uint8_t'.
 *
 * 3) The original was in open code format, not a function.
 */

#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n

#include <stdint.h>
#include "../inc/log2.h"

static const uint8_t
log_table_256[256] = {
	-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	LT(4),
	LT(5),
	LT(5),
	LT(6),
	LT(6),
	LT(6),
	LT(6),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7),
	LT(7)
};

uint32_t
uint32_log2(uint32_t v) {
	uint32_t r;
	register uint32_t t, tt;
	if ((tt = v >> 16))
		r = (t = tt >> 8) ? 24 + log_table_256[t] : 16 + log_table_256[tt];
	else
		r = (t = v >> 8) ? 8 + log_table_256[t] : log_table_256[v];
	return r;
}

/* txblog2.c ends here */
