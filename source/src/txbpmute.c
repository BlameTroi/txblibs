/* txbpmute.h -- blametroi's permutation library */

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

#ifndef TXBPMUTE_H
#define TXBPMUTE_H

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
#endif /* TXBPMUTE_H */

#ifdef TXBPMUTE_IMPLEMENTATION
#undef TXBPMUTE_IMPLEMENTATION

/*
 * permute the 'n' unique integers in 'ints' on successive calls until
 * no more permutations are available. the integers start in ascending
 * order, and the algorithm reverses that order over successive calls
 * by working from right to left through the array.
 *
 * the 6 permutations of 123 (3*2*1 = 6):
 *
 *   1: 123        starting sequence
 *   2: 132        3 < 2
 *   3: 213        3 > 1, so 312, then moves lowest after up, so 213
 *   4: 231        3 > 1, so 231
 *   5: 312        3 > 2, so 321, then moves lowest after up so 312
 *   6: 321        2 > 1, so 321
 *
 * the way i envision this being used is to generate permuted indices
 * into other structures. on my current m2 mac system this starts bogging
 * down as n passes 10. if all permutations are actually need (as in
 * the tavelling salesman problem, it's going to be slow. random
 * permutations of n items are left for another day.
 *
 * returns 1 if a new permutation was provided ('ints' was updated).
 *
 * returns 0 when there are no more permutations possible in the
 * list.
 */

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
	int n, int *ints
) {

	int swap, j;

	for (int i = n - 1; i > 0; i--) {

		/* find the first out of order pair working from the right */
		if (ints[i] > ints[i - 1]) {
			j = n - 1;

			/* find the smallest int larger than current one behind it */
			while (ints[i - 1] > ints[j])
				j -= 1;

			swap = ints[i - 1];
			ints[i - 1] = ints[j];
			ints[j] = swap;

			/* reverse ordering behind current */
			int start = i;
			int end = n - 1;
			while (start < end) {
				swap = ints[start];
				ints[start] = ints[end];
				ints[end] = swap;
				start += 1;
				end -= 1;
			}

			/* and return this permutation */
			return 1;
		}
	}

	/* no more permutations available */
	return 0;
}
#endif /* TXBPMUTE_IMPLEMENTATION */
/* txbpmute.h ends here */
