/* txbabort.h -- blametroi's common utility functions -- */
#ifndef TXBABORT_H
#define TXBABORT_H


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


#ifdef TXBABORT_H_IMPLEMENTATION

#include <assert.h>
#include <string.h>
#include <stdio.h>

#endif /* TXBABORT_H_IMPLEMENTATION */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * forward declarations
 */

void
do_abort(const char *msg, const char *func, const char *file, int line);

void
do_abort_if(const char *cond, const char *msg, const char *func, const char *file, int line);

#define abort(msg) do {\
   do_abort(msg, __func__, __FILE__, __LINE__);\
   } while(0)

#define abort_if(cond, msg) do {\
   if (cond) {\
   do_abort_if(#cond, msg, __func__, __FILE__, __LINE__);\
   }\
   } while(0)

#ifdef TXBABORT_H_IMPLEMENTATION

void
do_abort_if(const char *cond, const char *msg, const char *func, const char *file, int line) {
   fprintf(stderr, "abort:\n\tcondition: %s\n\t%s\n\t%s %s %d\n", cond, msg, func, file, line);
   exit(EXIT_FAILURE);
}

void
do_abort(const char *msg, const char *func, const char *file, int line) {
   fprintf(stderr, "abort:\n\t%s\n\t%s %s %d\n", msg, func, file, line);
   exit(EXIT_FAILURE);
}

#endif /* TXBABORT_H_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TXBABORT_H */
