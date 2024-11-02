/*
 * single file header generated via:
 * buildhdr --macro TXBRAND --intro LICENSE --pub ./source/inc/rand.h --priv ./source/src/rand.c 
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2024 Troy Brumley 
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   *** end intro ***
 */

#ifndef TXBRAND_SINGLE_HEADER
#define TXBRAND_SINGLE_HEADER
/* *** begin pub *** */
/* rand.h -- blametroi's common utility functions -- */

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
/* *** end pub *** */

#endif /* TXBRAND_SINGLE_HEADER */

#ifdef TXBRAND_IMPLEMENTATION
#undef TXBRAND_IMPLEMENTATION
/* *** begin priv *** */
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
/* *** end priv *** */

#endif /* TXBRAND_IMPLEMENTATION */
