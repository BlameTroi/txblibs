/* pmute.h -- blametroi's common utility functions -- */

/*
 * this is a header only implementation of a function to permute an
 * array of integers.
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

/*
 * permute_next(int n, int *ints)
 *
 * return the next possible permutation of the n unique integers in
 * the array ints. to get all possible iterations, start with the
 * elements of ints in ascending order. the array is updated on each
 * call and the function returns 0 when no more permutations are
 * possible.
 *
 * sample code:
 *
 * int n = 5;
 * int ints[n];
 * for (int i = 0; i < n; i++)
 *    ints[i] = i;
 * print_current(n, ints);
 * while (permute_next(n, ints))
 *    print_current(&iter, n, ints);
 *
 */

int
permute_next(
	int n,        /* number of integers in the array *ints */
	int *ints     /* n unique integers */
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
