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

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../inc/abort_if.h"

#include "../inc/rs.h"

/*
 * an instance of a string read stream.
 */

#define RSCB_TAG "__RSCB__"
#define RSCB_TAG_LEN 8

#define ASSERT_RSCB(p, m) \
	abort_if(!(p) || memcmp((p), RSCB_TAG, RSCB_TAG_LEN) != 0, (m));

#define ASSERT_RSCB_OR_NULL(p, m) \
	abort_if(p && memcmp((p), RSCB_TAG, RSCB_TAG_LEN) != 0, (m));

struct rscb {
	char tag[RSCB_TAG_LEN];
	char *str;
	size_t len;
	size_t pos;
	bool eos;
};

/*
 * rs_create_string
 *
 * create a new string read stream on a copy of a string.
 *
 *     in: a string
 *
 * return: the rs instance
 *
 * rs_create_string allocates its own copy of the string passed and
 * assumes responsibility for managing storage for that copy.
 */

rscb *
rs_create_string(
	const char *str
) {
	abort_if(!str,
		"rs_create_string no string provided");
	rscb *rs = malloc(sizeof(rscb));
	abort_if(!rs,
		"rs_create_string could not allocate RSCB");
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
 * rs_create_string_From_file
 *
 * create a new string readstream from the contents of an open file
 * stream.
 *
 *     in: a file stream
 *
 * return: the rs instance
 *
 * the entire file will be read and stored as a single string.
 * the file left positioned at the beginning of the file
 */

rscb *
rs_create_string_from_file(
	FILE *ifile
) {
	rewind(ifile);
	struct stat info;
	fstat(fileno(ifile), &info);
	char *data_buf = malloc(info.st_size + 1);
	abort_if(!data_buf,
		"rs_create_file could not allocate file buffer");
	memset(data_buf, 0, info.st_size + 1);
	fread(data_buf, info.st_size, 1, ifile);
	rscb *rs = rs_create_string(data_buf);
	memset(data_buf, 253, info.st_size + 1);
	free(data_buf);
	rewind(ifile);
	return rs;
}

/*
 * rs_clone
 *
 * create a deep copy of an existing read stream.
 *
 *     in: the rs instance
 *
 * return: the cloned rs instance
 */

rscb *
rs_clone(
	rscb *original
) {
	ASSERT_RSCB(original, "invalid RSCB");
	rscb *rs = malloc(sizeof(rscb));
	abort_if(!rs,
		"rs_clone could not allocate new RSCB");
	memset(rs, 0, sizeof(rscb));
	memcpy(rs->tag, RSCB_TAG, sizeof(rs->tag));
	rs->len = original->len;
	rs->pos = original->pos;
	rs->eos = original->eos;
	rs->str = malloc(rs->len + 1);
	abort_if(!rs->str,
		"rs_clone could not allocate space for new buffer");
	memset(rs->str, 0, rs->len + 1);
	strcpy(rs->str, original->str);
	return rs;
}

/*
 * rs_destroy_string
 *
 * release all resources for the string read stream.
 *
 *     in: the rs instance
 *
 * return: nothing
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
 * rs_at_end
 *
 * has the stream reached the end? only set -after- having read to the end.
 *
 * this is consistent with feof(). to see if the next read will eof, use
 * rs_peekc().
 *
 *     in: the rs instance
 *
 * return: bool
 */

bool
rs_at_end(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	return rs->eos;
}

/*
 * rs_peekc
 *
 * return the next character from the stream without advancing the
 * stream's position. EOF is returned instead of \0 for the end of
 * string.
 *
 *     in: the rs instance
 *
 * return: a signed character as an int
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
 * rs_position
 *
 * what is the current position within the stream.
 *
 *     in: the rs instance
 *
 * return: size_t position
 */

size_t
rs_position(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	return rs->pos;
}

/*
 * rs_length
 *
 * the total length of the string.
 *
 *     in: the rs instance
 *
 * return: length
 */

size_t
rs_length(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	return rs->len;
}

/*
 * rs_remaining
 *
 * length of the unread portion of the string. rs->len includes the
 * NUL byte, so we have to subtract to account for it.
 *
 *     in: the rs instance
 *
 * return: length
 */

size_t
rs_remaining(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	return (rs->len - 1) - rs->pos;
}

/*
 * rs_rewind
 *
 * reposition the stream to its beginning.
 *
 *     in: the rs instance
 *
 * return: nothing
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
 * rs_seek
 *
 * position the stream to a particular location.
 *
 *     in: the rs instance
 *
 *     in: location
 *
 * return: boolean
 */

bool
rs_seek(
	rscb *rs,
	size_t n
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	if (n < 0 || n > rs->len-1)
		return false;
	rs->pos = n;
	rs->eos = false;
	return true;
}

/*
 * rs_getc
 *
 * get the next character from the stream and advance its position. returns
 * EOF when end of stream is reached.
 *
 *     in: the rs instance
 *
 * return: the character or EOF
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
 * rs_ungetc
 *
 * back the stream position up by one character. in spite of the name ungetc,
 * no character is pushed back onto the stream.
 *
 * this is the same as rs_skip(rs, -1).
 *
 *     in: the rs instance
 *
 * return: the character or EOF
 */

int
rs_ungetc(
	rscb *rs
) {
	ASSERT_RSCB(rs, "invalid RSCB");
	if (rs->pos > 0) {
		rs->pos -= 1;
		rs->eos = false;
	}
	return rs->str[rs->pos];
}

/*
 * rs_skip
 *
 * change the current position in the stream by some
 * number of bytes.
 *
 *     in: the rs instance
 *
 *     in: signed number of characters to skip
 *
 * return: bool, false if skip would move the position out of the
 *         string
 */

bool
rs_skip(
	rscb *rs,
	long n
) {
	ASSERT_RSCB(rs, "invalid RSCB");

	if (rs->pos + n < 0 || rs->pos + n > rs->len)
		return false;

	if (n == 0)
		return true;

	/* using the getc and ungetc functions
	 * might seem slow but it will generalize
	 * to buffered streams. */

	typedef int (*fn_getter)(rscb *);
	fn_getter getter = rs_getc;
	if (n < 0) {
		getter = rs_ungetc;
		n = -n;
	}

	int c = getter(rs);
	n -= 1;
	while (c != EOF && n) {
		c = getter(rs);
		n -= 1;
	}
	return true;
}

/*
 * rs_gets
 *
 * return a line from the read stream mimicing the behavior of
 * [f]gets(). returns at most buflen-1 characters. reading stops on a
 * newline character or at endof stream. if a newline is read, it is
 * stored in the output buffer. appends '\0' to the string. returns
 * NULL if the stream is empty.
 *
 *     in: the rs instance
 *
 *     in: start of buffer
 *
 *     in: maximum length of buffer
 *
 * return: buffer or NULL
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
