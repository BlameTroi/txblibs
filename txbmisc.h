/* txbmisc.h -- blametroi's common utility functions -- */
#ifndef TXBMISC_H
#define TXBMISC_H


/*
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this is a header only implementation of various bits of code that i
 * keep repeating in my hobby programming that i want around without
 * the hassle of managing library dependencies.
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */


#ifdef TXBMISC_H_IMPLEMENTATION
#include <string.h>
#endif /* TXBMISC_H_IMPLEMENTATION */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * forward declarations
 */

/* a pseudo random integer range */
int rand_between(int, int);

/* shuffle n items using the fisher-yates algorithm */
void
shuffle(void **cards, int n);

/* various min/max functions */

int max(int, int);
int min(int, int);
int imax(int, int);
int imin(int, int);
long lmax(long, long);
long lmin(long, long);
double dmax(double, double);
double dmin(double, double);

#ifdef TXBMISC_H_IMPLEMENTATION

/* as a general rule, i prefer to not use macros to express an algorithm */

/* none of the macro solutions to not having min or max available
   appeal to me, so instead here are inlinable definitions for various
   common types. */

inline int max(int x, int y) {
   return x > y ? x : y;
}
inline int min(int x, int y) {
   return x < y ? x : y;
}
inline int imax(int x, int y) {
   return x > y ? x : y;
}
inline int imin(int x, int y) {
   return x < y ? x : y;
}
inline long lmax(long x, long y) {
   return x > y ? x : y;
}
inline long lmin(long x, long y) {
   return x < y ? x : y;
}
inline double dmax(double x, double y) {
   return x > y ? x : y;
}
inline double dmin(double x, double y) {
   return x < y ? x : y;
}

/* generate a pseudo random integer between low and high inclusive. yes, this
   isn't really secure randomness, but it's suitable for many purposes. the
   idea is from https://stackoverflow.com/a/1202706 */

inline int rand_between(int low, int high) {
   return rand() % (high + 1 - low) + low;
}

/* shuffle an array of items using the fisher-yates algorithm. the
   array is updated in place. by using an array of void pointers, any
   objects can be shuffled. uses rand_between() and the usual comments
   regarding rand() and srand() apply. */

void
shuffle(void **cards, int n) {
   int i = 100;
   while (i > 0) {
      int r = rand_between(1, i);
      void *s = cards[r-1];
      cards[r-1] = cards[i-1];
      cards[i-1] = s;
      i -= 1;
   }
}

#endif /* TXBMISC_H_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TXBMISC_H */
