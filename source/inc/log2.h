/* txblog2.h -- fast integer log base(2) -- Troy Brumley BlameTroi@gmail.com */

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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

uint32_t
uint32_log2(
	uint32_t v
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* txblog2.h ends here */
