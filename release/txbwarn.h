/* txbwarn.h -- debugging assist macros */

/*
 * temporary drop in macros to help with debugging. enable by defining
 * TXBWARN before including txbwarn.h, otherwise the macros are noops.
 *
 * i needed these while chasing a very obscure bug and figured i'd
 * keep them even though i don't think i'll use them often. even if
 * the exact macro isn't needed, it's a reasonable starting template.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef TXBWARN
#define WARN_NULL(n, f)\
	if (!(n)) \
		fprintf(stderr, "%s called with unexpected NULL\n", #f)
#define WARN_NULL_TWO(n, o, f)\
	if ((n) && (!o)) \
		fprintf(stderr, "%s dropping a NULL on a pointer [%d]\n", #f)
#else
#define WARN_NULL(n, f)
#define WARN_NULL_TWO(n, o, f)
#endif

/* txbwarn.h ends here */
