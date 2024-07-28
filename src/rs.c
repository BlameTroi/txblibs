/* rs.c -- blametroi's string read stream library */

/*
 * a header only implementation of a string read stream.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/rs.h"

/*
 * an instance of a string read stream.
 */

#define RSCB_TAG "__RSCB__"
struct rscb {
   char tag[8];
   char *str;
   size_t len;
   size_t pos;
   bool eos;
};

/*
 * create a new string read stream on a copy of a string.
 */

rscb *
rs_create_string(
   const char *str
) {
   assert(str);
   rscb *rs = malloc(sizeof(rscb));
   assert(rs);
   memset(rs, 0, sizeof(rscb));
   memcpy(rs->tag, RSCB_TAG, sizeof(rs->tag));
   rs->len = strlen(str);
   rs->str = malloc(rs->len+1);
   strcpy(rs->str, str);
   rs->pos = 0;
   rs->eos = false;
   return rs;
}

/*
 * create a copy of an existing read stream, duplicating its state and
 * making a fresh copy of the backing string.
 */

rscb *
rs_clone(
   rscb *original
) {
   assert(original && memcmp(original->tag, RSCB_TAG, sizeof(original->tag)) == 0);
   rscb *rs = malloc(sizeof(rscb));
   assert(rs);
   memset(rs, 0, sizeof(rscb));
   memcpy(rs->tag, RSCB_TAG, sizeof(rs->tag));
   rs->len = original->len;
   rs->pos = original->pos;
   rs->eos = original->eos;
   rs->str = malloc(rs->len + 1);
   strcpy(rs->str, original->str);
   return rs;
}
/*
 * release all resources for the string read stream.
 */

void
rs_destroy_string(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   memset(rs->str, 253, rs->len);
   free(rs->str);
   memset(rs, 253, sizeof(rscb));
   free(rs);
}

/*
 * has the stream reached the end? only set -after- having read to the end.
 *
 * this is consistent with feof(). to see if the next read will eof, use
 * rs_peekc().
 */

bool
rs_at_end(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   return rs->eos;
}

/*
 * return the next character from the stream without advancing the
 * stream's position. EOF is returned instead of \0 for the end of
 * string.
 */

int
rs_peekc(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   if (rs->eos) {
      return EOF;
   }
   return rs->str[rs->pos] ? rs->str[rs->pos] : EOF;
}

/*
 * what is the current position within the stream.
 */

size_t
rs_position(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   return rs->pos;
}

/*
 * total length of string in buffer. rs->len includes the NUL byte, so we
 * have to subtract it.
 */

size_t
rs_length(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   return (rs->len - 1);
}

/*
 * length still to read in buffer. rs->len includes the NUL byte, so we
 * have to subtract it.
 */

size_t
rs_remaining(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   return (rs->len - 1) - rs->pos;
}

/*
 * reposition the stream to its beginning.
 */

void
rs_rewind(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   rs->pos = 0;
   rs->eos = false;
}

/*
 * set the stream position to an absolute position. not yet implemented.
 */

bool
rs_seek(
   rscb *rs,
   size_t n
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   assert(NULL); /* not implemented */
};

/*
 * get the next character from the stream and advance its position. returns
 * EOF when end of stream is reached.
 */

int
rs_getc(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   if (rs->eos) {
      return EOF;
   }
   int next = rs->str[rs->pos];
   if (next == '\0') {
      next = EOF;
   }
   rs->eos = next == EOF;
   rs->pos += 1;
   return next;
}

/*
 * back the stream position up by one character. in spite of the name ungetc,
 * no character is pushed back onto the stream.
 */

void
rs_ungetc(
   rscb *rs
) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   if (rs->pos > 0) {
      rs->pos -= 1;
      rs->eos = false;
   }
}

/*
 * adjust the stream position by a signed number of characters.
 */

bool
rs_skip(
   rscb *rs,
   long n) {
   assert(rs && memcmp(rs->tag, RSCB_TAG, sizeof(rs->tag)) == 0);
   assert(n >= 0); /* reading backward not implemented */
   /* n == 0 is a nop but we'll allow it */
   int c = rs_getc(rs);
   n -= 1;
   while (c != EOF && n) {
      c = rs_getc(rs);
      n -= 1;
   }
   return c != EOF && n == 0;
}
