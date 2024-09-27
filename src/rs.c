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

#undef NDEBUG
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../inc/rs.h"

/*
 * an instance of a string read stream.
 */

#define RSCB_TAG "__RSCB__"
#define RSCB_TAG_LEN 8
#define ASSERT_RSCB(p, m) assert((p) && memcmp((p), RSCB_TAG, RSCB_TAG_LEN) == 0 && (m))
#define ASSERT_RSCB_OR_NULL(p) assert((p) == NULL || memcmp((p), RSCB_TAG, RSCB_TAG_LEN) == 0)

struct rscb {
	char tag[RSCB_TAG_LEN];
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
 * create a new string readstream from the contents of an open file
 * stream.
 */

rscb *
rs_create_string_from_file(
	FILE *ifile
) {
	rewind(ifile);
	struct stat info;
	fstat(fileno(ifile), &info);
	char *data_buf = malloc(info.st_size + 1);
	assert(data_buf);
	fread(data_buf, info.st_size, 1, ifile);
	rscb *rs = rs_create_string(data_buf);
	memset(data_buf, 253, info.st_size + 1);
	free(data_buf);
	rewind(ifile);
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
	ASSERT_RSCB(original, "invalid RSCB");
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
	ASSERT_RSCB(rs, "invalid RSCB");
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
	ASSERT_RSCB(rs, "invalid RSCB");
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
	ASSERT_RSCB(rs, "invalid RSCB");
	if (rs->eos)
		return EOF;
	return rs->str[rs->pos] ? rs->str[rs->pos] : EOF;
}

/*
 * what is the current position within the stream.
 */

size_t
rs_position(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
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
	ASSERT_RSCB(rs, "invalid RSCB");
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
	ASSERT_RSCB(rs, "invalid RSCB");
	return (rs->len - 1) - rs->pos;
}

/*
 * reposition the stream to its beginning.
 */

void
rs_rewind(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
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
	ASSERT_RSCB(rs, "invalid RSCB");
	assert(NULL); /* not implemented */
}

/*
 * get the next character from the stream and advance its position. returns
 * EOF when end of stream is reached.
 */

int
rs_getc(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	if (rs->eos)
		return EOF;
	int next = rs->str[rs->pos];
	if (next == '\0')
		next = EOF;
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
	ASSERT_RSCB(rs, "invalid RSCB");
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
	ASSERT_RSCB(rs, "invalid RSCB");
	assert(n >= 0 &&
		"backward read not implemented"); /* reading backward not implemented */
	/* n == 0 is a nop but we'll allow it */
	int c = rs_getc(rs);
	n -= 1;
	while (c != EOF && n) {
		c = rs_getc(rs);
		n -= 1;
	}
	return c != EOF && n == 0;
}

/*
 * return a line from the read stream mimicing the behavior of
 * [f]gets(). returns at most buflen-1 characters. reading stops on a
 * newline character or at endof stream. if a newline is read, it is
 * stored in the output buffer. appends '\0' to the string. returns
 * NULL if the stream is empty.
 */

char *
rs_gets(
	rscb *rs,
	char *buffer,
	int buflen
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	/* return null for bad arguments or when at eof */
	if (rs->eos || buflen < 2 || buffer == NULL)
		return NULL;
	char c = rs_getc(rs);
	if (c == EOF)
		return NULL;
	/* we could pull it all out directly with a memcpy but the slight
	 * performance improvement isn't worth losing the ability to swap
	 * out stream sources in the future. */
	char *p = buffer;
	while (c != '\n' && c != EOF && buflen > 1) {
		*p = c;
		p += 1;
		*p = '\0';
		buflen -= 1;
		c = rs_getc(rs);
	}
	/* if we hit newline and there's room, store it in the buffer
	 * otherwise put whatever character back for the next request. */
	if (c == '\n' && buflen > 1) {
		*p = c;
		*(p+1) = '\0';
	} else
		rs_ungetc(rs);
	return buffer;
}
