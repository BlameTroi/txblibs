/* misc.h -- blametroi's common utility functions -- */

/*
 * this is a header only implementation of various bits of code that i
 * keep repeating in my hobby programming that i want around without
 * the hassle of managing library dependencies. all functions are
 * small and i think pretty obvious.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* a pseudo random integer range */
int rand_between(int, int);

/* shuffle n items using the fisher-yates algorithm */
void
shuffle(void **cards, int n);

/* return an array of the factors of n */
long *
factors_of(long n);

/* various min/max functions */
int max(int, int);
int min(int, int);
int imax(int, int);
int imin(int, int);
long lmax(long, long);
long lmin(long, long);
double dmax(double, double);
double dmin(double, double);

/* common tests */
bool is_even(long);
bool is_odd(long);

bool is_digit(char);
bool is_word_char(char);
bool is_whitespace(char);
bool is_control(char);
bool is_punctuation(char);
bool is_bracketing(char);

#ifdef __cplusplus
}
#endif /* __cplusplus */
