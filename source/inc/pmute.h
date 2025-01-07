/* txbpmute.h -- A small permutation library -- Troy Brumley BlameTroi@gmail.com */

/*
 * This is a header only implementation of a function to permute an
 * array of integers.
 *
 * Released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * This software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * permute_next
 *
 *     in: length of array to permute
 *
 * in/out: array of ints to permute
 *
 * return the next possible permutation of the n unique integers in
 * the array ints. to get all possible iterations, start with the
 * elements of ints in ascending order. the array is updated on each
 * call and the function returns 0 when no more permutations are
 * possible.
 *
 * (the array has been reversed, and is now highest to lowest values.)
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

/* txbpmute.h ends here */
