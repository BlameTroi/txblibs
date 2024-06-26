/*
 * single file header generated via:
 * buildhdr --macro TXBPMUTE --intro LICENSE --pub inc/pmute.h --priv src/pmute.c 
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

#ifndef TXBPMUTE_SINGLE_HEADER
#define TXBPMUTE_SINGLE_HEADER
/* *** begin pub *** */
/* pmute.h -- blametroi's common utility functions -- */

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
/* *** end pub *** */

#endif /* TXBPMUTE_SINGLE_HEADER */

#ifdef TXBPMUTE_IMPLEMENTATION
#undef TXBPMUTE_IMPLEMENTATION
/* *** begin priv *** */
/* pmute.c -- blametroi's common utility functions -- */

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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



/* permute the 'n' unique integers in 'ints' on successive calls until
   no more permutations are available. the integers start in ascending
   order, and the algorithm reverses that order over successive calls
   by working from right to left through the array.

   the 6 permutations of 123 (3*2*1 = 6):

     1: 123        starting sequence
     2: 132        3 < 2
     3: 213        3 > 1, so 312, then moves lowest after up, so 213
     4: 231        3 > 1, so 231
     5: 312        3 > 2, so 321, then moves lowest after up so 312
     6: 321        2 > 1, so 321

   the way i envision this being used is to generate permuted indices
   into other structures. on my current m2 mac system this starts bogging
   down as n passes 10. if all permutations are actually need (as in
   the tavelling salesman problem, it's going to be slow. random
   permutations of n items are left for another day.

   returns 1 if a new permutation was provided ('ints' was updated).

   returns 0 when there are no more permutations possible in the
   list. */

int
permute_next(int n, int *ints) {

   int swap, j;

   for (int i = n - 1; i > 0; i--) {

      /* find the first out of order pair working from the right */
      if (ints[i] > ints[i - 1]) {
         j = n - 1;

         /* find the smallest int larger than current one behind it */
         while (ints[i - 1] > ints[j]) {
            j -= 1;
         }

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
/* *** end priv *** */

#endif /* TXBPMUTE_IMPLEMENTATION */
