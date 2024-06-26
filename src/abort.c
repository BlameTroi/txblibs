/* abort.c -- blametroi's common utility functions -- */

/*
 * a header only implementation of abort and report functions.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../inc/abort.h"

void
do_abort_if(
   const char *cond,
   const char *msg,
   const char *func,
   const char *file,
   int line
) {
   fprintf(stderr, "abort:\n\tcondition: %s\n\t%s\n\t%s %s %d\n", cond, msg, func, file, line);
   exit(EXIT_FAILURE);
}

void
do_abort(
   const char *msg,
   const char *func,
   const char *file,
   int line
) {
   fprintf(stderr, "abort:\n\t%s\n\t%s %s %d\n", msg, func, file, line);
   exit(EXIT_FAILURE);
}
