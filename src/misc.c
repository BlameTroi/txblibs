/* misc.c -- blametroi's common utility functions -- */

/*
 * a header only implementation of various bits of code don't fit in
 * any of my other single c header file libraries.
 *
 * many of these functions are marked for inlining. some of the
 * numeric functions us long instead of int to deal with some of the
 * big numbers seen in problems from advent of code and other puzzle
 * sites.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../inc/misc.h"

/* as a general rule, i prefer to not use macros to express an algorithm */

/*
 * none of the macro solutions to not having min or max available
 * appeal to me, so instead here are inlinable definitions for various
 * common types.
 */

inline int
max(int x, int y) {
   return x > y ? x : y;
}

inline int
min(int x, int y) {
   return x < y ? x : y;
}

inline int
imax(int x, int y) {
   return x > y ? x : y;
}

inline int
imin(int x, int y) {
   return x < y ? x : y;
}

inline long
lmax(long x, long y) {
   return x > y ? x : y;
}

inline long
lmin(long x, long y) {
   return x < y ? x : y;
}

inline double
dmax(double x, double y) {
   return x > y ? x : y;
}

inline double
dmin(double x, double y) {
   return x < y ? x : y;
}

/*
 * quick bit test for even or odd.
 */

inline bool
is_even(long n) {
   return !(n & 1);
}

inline bool
is_odd(long n) {
   return (n & 1);
}

/*
 * quick character classifications, by us-ascii programmer centric
 * rules.
 */

inline bool
is_digit(char c) {
   return c >= '0' && c <= '9';
}

inline bool
is_word_char(char c) {
   return (c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          c == '_';
}

inline bool
is_whitespace(char c) {
   return c == ' ' ||
          c == '\n' ||
          c == '\f' ||
          c == '\r' ||
          c == '\t';
}

inline bool
is_control(char c) {
   return c > '\0' && c < ' ';
}

inline bool
is_punctuation(char c) {
   return c == '.' ||
          c == ',' ||
          c == '?' ||
          c == '!' ||
          c == ';' ||
          c == ':';
}

inline bool
is_bracketing(char c) {
   return c == '[' ||
          c == '(' ||
          c == '{' ||
          c == '}' ||
          c == ')' ||
          c == ']';
}

/*
 * generate a pseudo random integer between low and high inclusive. yes, this
 * isn't really secure randomness, but it's suitable for many purposes. the
 * idea is from https://stackoverflow.com/a/1202706
 */

inline int
rand_between(
   int low,
   int high
) {
   return rand() % (high + 1 - low) + low;
}

/*
 * shuffle an array of items using the fisher-yates algorithm. the
 * array is updated in place. by using an array of void pointers, any
 * objects can be shuffled. uses rand_between() and the usual comments
 * regarding rand() and srand() apply.
 */

void
shuffle(
   void **cards,
   int n
) {
   int i = n;
   while (i > 0) {
      int r = rand_between(1, i);
      void *s = cards[r-1];
      cards[r-1] = cards[i-1];
      cards[i-1] = s;
      i -= 1;
   }
}

/*
 * returns an array of long integers with at least enough entries to
 * hold the factors of n and a trailing NULL entry. the caller is
 * responsible for freeing the array when it is no longer needed.
 * returns NULL if n < 1.
 */

long *
factors_of(
   long n
) {
   if (n < 1) {
      return NULL;
   }

   /* allocate space for a dynamically sized array of longs. starting
    * size is 64 entries, of which the last entry is reserved for
    * a trailing NULL value. grows by doubling, but that should be
    * rarely needed. there are 64 factors of 999,999 and 49 of
    * 1,000,000. */
   int lim = 64;
   long *factors = calloc(lim, sizeof(*factors));
   int f = 0;

   /* just count up from 1 to half, tack on n, and we're done. */
   long i = 1;
   long half = n / 2;
   while (i <= half) {
      if (n % i == 0) {
         factors[f] = i;
         f += 1;

         /* if we're approaching the end of the allocation,
          * we know we need at least one more entry so
          * grow the array. */
         if (f + 2 >= lim) {
            long *grow = calloc(lim * 2, sizeof(*factors));
            memcpy(grow, factors, lim * sizeof(*factors));
            free(factors);
            factors = grow;
            lim = lim * 2;
         }
      }
      i += 1;
   }

   factors[f] = n;
   return factors;
}
