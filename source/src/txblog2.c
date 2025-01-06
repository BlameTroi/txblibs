/* txblog2.h -- a fast integer log base 2 calculation */

/*
 * this is a header only implementation of a fast integer log base 2
 * function for 32 bit integers. it originally comes from:
 *
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
 *
 * but i've tweaked some naming and formatting. i don't claim the
 * algorithm, just this particular implementation, which as always
 * i have --
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef TXBLOG2_H
#define TXBLOG2_H

#include <stdint.h>

uint32_t
uint32_log2(
	uint32_t v
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBLOG2_H */

#ifdef TXBLOG2_IMPLEMENTATION
#undef TXBLOG2_IMPLEMENTATION

/*
 * this is a header only implementation of a fast integer log base 2
 * function for 32 bit integers. it originally comes from:
 *
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
 *
 * but i've tweaked some naming and formatting. i don't claim the
 * algorithm, just this particular implementation, which as always
 * i have --
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

/*
 * implementation notes:
 *
 * see the following url for the original plus much more:
 *
 * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
 *
 * i had to make three changes, after which i felt free to do some
 * reformatting of the original code to match my sense of esthetics.
 *
 * 1) for an unknown (to me) reason, having the #define followed by
 *    immediately by log table gets an error complaining about
 *    include for stdint, or we get an error about an empty
 *    translation unit on the type specifier for log table 256.
 *    'iso c requires that a translation unit have at least one
 *    declaration' but there is a declaration.
 *
 *    while in 'wtf' mode, i put an #include between the #define
 *    and the table the error goes away.
 *
 * 2) in order to avoid the 'include not directly used' pedantic
 *    warning, i changed unsigned int to uint32_t and char to
 *    uint8_t.
 *
 * 3) the original was in open code format, not as a function.
 */

#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n

#include <stdint.h>
#include <txblog2.h>

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

#endif /* TXBLOG2_IMPLEMENTATION */
/* txblog2.h ends here */
