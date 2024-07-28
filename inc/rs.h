/* rs.h -- blametroi's string read stream library */

/*
 * a header only implementation of a string backed read stream.
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
 * an instance of a particular string read stream.
 */

typedef struct rscb rscb;

/*
 * create a new string read stream on a copy of a string.
 */

rscb *
rs_create_string(
   const char *str
);

/*
 * create a clone of a string read stream.
 */

rscb *
rs_clone(
   rscb *original
);

/*
 * free resources for the string read stream.
 */

void
rs_destroy_string(
   rscb *rs
);

/*
 * has the stream reached end of string?
 */

bool
rs_at_end(
   rscb *rs
);

/*
 * how far into the string has the stream advanced?
 */

size_t
rs_position(
   rscb *rs
);

/*
 * length of entire string in buffer.
 */

size_t
rs_length(
   rscb *rs
);

/*
 * length still to read in buffer.
 */

size_t
rs_remaining(
   rscb *rs
);

/*
 * reposition the stream to the start of the string.
 */

void
rs_rewind(
   rscb *rs
);

/*
 * reposition the stream to a particular character in the
 * string.
 */

bool
rs_seek(
   rscb *rs,
   size_t n
);

/*
 * reposition the stream forward or backward by a signed
 * number of characters.
 */

bool
rs_skip(
   rscb *rs,
   long n
);

/*
 * get the next character from stream. EOF is returned at
 * end of string, not \0.
 */

int
rs_getc(
   rscb *rs
);

/*
 * back up one character in the stream. while named ungetc,
 * the backing string is not updated. essentially a skip -1.
 */

void
rs_ungetc(
   rscb *rs
);

/*
 * get the next character from the stream without advancing
 * its position.
 */

int
rs_peekc(
   rscb *rs
);
