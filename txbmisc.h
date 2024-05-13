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
 * forward definitions
 */

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

#endif /* TXBMISC_H_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TXBMISC_H */
