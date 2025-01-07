/*
 * single file header generated via:
 * buildhdr --macro TXBRS --intro LICENSE --pub ./inc/rs.h --priv ./src/rs.c
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2025 Troy Brumley
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

#ifndef TXBRS_SINGLE_HEADER
#define TXBRS_SINGLE_HEADER
/* *** begin pub *** */
/* txbrs.h -- Simple Read Stream -- Troy Brumley BlameTroi@gmail.com */

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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * a handle to a string read stream instance.
 */

typedef struct rscb hrs;

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

hrs *
rs_create_string(
	const char *str
);

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
 * the entire file will be read and stored as a single string. the
 * file is left positioned at the beginning of the file.
 */

hrs *
rs_create_string_from_file(
	FILE *ifile
);

/*
 * rs_clone
 *
 * create a deep copy of an existing read stream.
 *
 *     in: the rs instance
 *
 * return: the cloned rs instance
 */

hrs *
rs_clone(
	hrs *original
);

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
	hrs *rs
);

/*
 * rs_at_end
 *
 * has the stream reached the end? only set -after- having read to the end.
 *
 * this is consistent with feof(). to see if the next read will reach
 * the end of file, use rs_peekc().
 *
 *     in: the rs instance
 *
 * return: boolean
 */

bool
rs_at_end(
	hrs *rs
);

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
	hrs *rs
);

/*
 * rs_length
 *
 * the total length of the string.
 *
 *     in: the rs instance
 *
 * return: size_t length
 */

size_t
rs_length(
	hrs *rs
);

/*
 * rs_remaining
 *
 * length of the unread portion of the string. rs->len includes the
 * NUL byte, so we have to subtract to account for it.
 *
 *     in: the rs instance
 *
 * return: size_t remaining length
 */

size_t
rs_remaining(
	hrs *rs
);

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
	hrs *rs
);

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
	hrs *rs,
	size_t n
);

/*
 * rs_skip
 *
 * change the current position in the stream by some number of bytes.
 * this is not wide character friendly.
 *
 *     in: the rs instance
 *
 *     in: signed number of characters to skip
 *
 * return: boolean false if skip would move the position out of the
 *         string
 */

bool
rs_skip(
	hrs *rs,
	long n
);

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
	hrs *rs
);

/*
 * rs_ungetc
 *
 * back the stream position up by one character. in spite of the name
 * ungetc, no character is pushed back onto the stream. string gets
 * are not destructive.
 *
 *     in: the rs instance
 *
 * return: the character or EOF
 */

int
rs_ungetc(
	hrs *rs
);

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
	hrs *rs
);

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
	hrs *rs,
	char *buffer,
	int buflen
);


#ifdef __cplusplus
}
#endif /* __cplusplus */
/* txbrs.h ends here */
/* *** end pub *** */

#endif /* TXBRS_SINGLE_HEADER */

#ifdef TXBRS_IMPLEMENTATION
#undef TXBRS_IMPLEMENTATION
/* *** begin priv *** */
/* txbrs.c -- Simple Read Stream -- Troy Brumley BlameTroi@gmail.com */

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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "txbabort_if.h"

/*
 * an instance of a string read stream.
 */

#define HRS_TAG "__HRS___"
#define HRS_TAG_LEN 8

#define ASSERT_HRS(p, m) \
	abort_if(!(p) || memcmp((p), HRS_TAG, HRS_TAG_LEN) != 0, (m));

#define ASSERT_HRS_OR_NULL(p, m) \
	abort_if(p && memcmp((p), HRS_TAG, HRS_TAG_LEN) != 0, (m));

struct rscb {
	char tag[HRS_TAG_LEN];
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
 * rs_create_string allocates storage for its copy of the string
 * passed and assumes responsibility for managing storage for that
 * copy.
 */

hrs *
rs_create_string(
	const char *str
) {
	abort_if(!str,
		"rs_create_string no string provided");
	hrs *rs = malloc(sizeof(*rs));
	abort_if(!rs,
		"rs_create_string could not allocate HRS");
	memset(rs, 0, sizeof(*rs));
	memcpy(rs->tag, HRS_TAG, sizeof(rs->tag));
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

hrs *
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
	hrs *rs = rs_create_string(data_buf);
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

hrs *
rs_clone(
	hrs *original
) {
	ASSERT_HRS(original, "invalid HRS");
	hrs *rs = malloc(sizeof(*rs));
	abort_if(!rs,
		"rs_clone could not allocate new HRS");
	memset(rs, 0, sizeof(*rs));
	memcpy(rs->tag, HRS_TAG, sizeof(rs->tag));
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
	memset(rs->str, 253, rs->len);
	free(rs->str);
	memset(rs, 253, sizeof(*rs));
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs,
	size_t n
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs
) {
	ASSERT_HRS(rs, "invalid HRS");
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
	hrs *rs,
	long n
) {
	ASSERT_HRS(rs, "invalid HRS");

	if (rs->pos + n < 0 || rs->pos + n > rs->len)
		return false;

	if (n == 0)
		return true;

	/* using the getc and ungetc functions
	 * might seem slow but it will generalize
	 * to other buffered streams. */

	typedef int (*fn_getter)(hrs *);
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
	hrs *rs,
	char *buffer,
	int buflen
) {
	ASSERT_HRS(rs, "invalid HRS");

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

/* txbrs.c ends here */
/* *** end priv *** */

#endif /* TXBRS_IMPLEMENTATION */
