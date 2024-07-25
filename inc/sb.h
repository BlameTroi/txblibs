/* sb.h -- blametroi's string builder library */

/*
 * a header only implementation of a very basic string builder.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdio.h>

/*
 * an instance of a string builder.
 */

typedef struct sbcb sbcb;

/*
 * create a new empty string builder with an initial buffer size of
 * blksize.
 */

sbcb *
sb_create_blksize(
   size_t blksize
);

/*
 * create a new empty string builder with a defaulted initial buffer
 * size.
 */

sbcb *
sb_create(
   void
);

/*
 * reset string builder to empty.
 */

void
sb_reset(
   sbcb *sb
);

/*
 * release all resources of the string builder.
 */

void
sb_destroy(
   sbcb *sb
);

/*
 * current length of the string.
 */

size_t
sb_length(
   sbcb *sb
);

/*
 * append a character c (as an unsigned char) to the string.
 */

void
sb_putc(
   sbcb *sb,
   int c
);

/*
 * append a string to the string.
 */

void
sb_puts(
   sbcb *sb,
   char *str
);

/*
 * return a copy of the current string builder contents.
 */

char *
sb_to_string(
   sbcb *sb
);
