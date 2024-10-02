/*
 * single file header generated via:
 * buildhdr --macro TXBSB --intro LICENSE --pub inc/sb.h --priv src/sb.c 
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

#ifndef TXBSB_SINGLE_HEADER
#define TXBSB_SINGLE_HEADER
/* *** begin pub *** */
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * an opaque instance of a string builder.
 */

typedef struct sbcb sbcb;

/*
 * override SBCB_DEFAULT_BLKSIZE if you wish. #define it before you
 * #include this library.
 */

#ifndef SBCB_DEFAULT_BLKSIZE
#define SBCB_DEFAULT_BLKSIZE 4096
#endif

/*
 * sb_create_blksize
 *
 * you should probably use sb_create, sb_create_string,
 * sb_create_null, or sb_create_file, but this is exposed if you want
 * to use it.
 *
 * create a new string builder with the specified buffer block size.
 *
 * allocates an initial buffer to hold incoming characgters. the
 * buffer grows as needed.
 *
 *     in: a blocksize in bytes, can be zero
 *
 * return: the sb instance
 */

sbcb *
sb_create_blksize(
	size_t blksize
);

/*
 * sb_create_null
 *
 * create a new empty string builder with no backing buffer. it's
 * /dev/null for string builders.
 *
 *     in: nothing
 *
 * return: the sb instance
 */

sbcb *
sb_create_null(
	void
);

/*
 * sb_create
 *
 * create a new string builder with a default buffer block size.
 *
 *     in: nothing
 *
 * return: the sb instance
 */

sbcb *
sb_create(
	void
);

/*
 * sb_create_string
 *
 * create a new string builder initialized with a string.
 *
 *     in: a string
 *
 * return: the sb instance
 */

sbcb *
sb_create_string(
	char *str
);

/*
 * sb_create_file
 *
 * create a new string builder initialized with the contents
 * of the provided file stream. the entire file is read and
 * left positioned at the beginning of the file.
 *
 *     in: an open file stream
 *
 * return: the sb instance
 */

sbcb *
sb_create_file(
	FILE *ifile
);

/*
 * sb_reset
 *
 * reset string builder to an initial empty state. this clears but doesnot
 * release the buffer.
 *
 *     in: the sb instance
 *
 * return: nothing
 */

void
sb_reset(
	sbcb *sb
);

/*
 * sb_destroy
 *
 * clear and release all storage for this instance.
 *
 *     in: the sb instance
 *
 * return: nothing
 */

void
sb_destroy(
	sbcb *sb
);

/*
 * sb_length
 *
 * how long is the current string?
 *
 *     in: the sb instance
 *
 * return: character count
 */

size_t
sb_length(
	sbcb *sb
);

/*
 * sb_putc
 *
 * append a single character to the string builder.
 *
 *     in: the sb instance
 *
 *     in: character
 *
 * return: nothing
 */

void
sb_putc(
	sbcb *sb,
	int c
);

/*
 * sb_puts
 *
 * append a string to the string bulder.
 *
 *     in: the sb instance
 *
 * return: string
 *
 * sb_puts behaves like fputs() and not puts() in the standard
 * library. it does not append a newline after the string.
 */

void
sb_puts(
	sbcb *sb,
	char *str
);

/*
 * sb_to_string
 *
 * return a copy of the string builder's contents.
 *
 *     in: the sb instance
 *
 * return: string
 *
 * the client is reponsible for freeing the string.
 */

char *
sb_to_string(
	sbcb *sb
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* *** end pub *** */

#endif /* TXBSB_SINGLE_HEADER */

#ifdef TXBSB_IMPLEMENTATION
#undef TXBSB_IMPLEMENTATION
/* *** begin priv *** */
/* sb.c -- blametroi's string builder library */

/*
 * a header only implementation of a very basic string builder.
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


/*
 * an instance of a string builder.
 */

#define SBCB_TAG "__SBCB__"
#define SBCB_TAG_LEN 8
#define ASSERT_SBCB(p, m) assert((p) && memcmp((p), SBCB_TAG, SBCB_TAG_LEN) == 0 && (m))
#define ASSERT_SBCB_OR_NULL(p) assert((p) == NULL || memcmp((p), SBCB_TAG, SBCB_TAG_LEN) == 0)

struct sbcb {
	char tag[SBCB_TAG_LEN];
	char *buf;
	size_t blksize;
	size_t buf_len;
	size_t buf_used;
	bool is_null;
};

/*
 * sb_create_blksize
 *
 * you should probably use sb_create, sb_create_string,
 * sb_create_null, or sb_create_file, but this is exposed if you want
 * to use it.
 *
 * create a new string builder with the specified buffer block size.
 *
 * allocates an initial buffer to hold incoming characgters. the
 * buffer grows as needed.
 *
 *     in: a blocksize in bytes, can be zero
 *
 * return: the sb instance
 */

sbcb *
sb_create_blksize(
	size_t blksize
) {
	/* allocate the base block */
	sbcb *sb = malloc(sizeof(sbcb));
	assert(sb);
	memset(sb, 0, sizeof(sbcb));
	memcpy(sb->tag, SBCB_TAG, sizeof(sb->tag));
	sb->is_null = blksize == 0;

	/* if this is a null sink, we're done */
	if (sb->is_null)
		return sb;

	/* allocate the initial buffer */
	sb->buf = malloc(blksize);
	assert(sb->buf);
	memset(sb->buf, 0, blksize);
	sb->buf_len = blksize;
	sb->blksize = blksize;

	return sb;
}

/*
 * sb_create_null
 *
 * create a new empty string builder with no backing buffer. it's
 * /dev/null for string builders.
 *
 *     in: nothing
 *
 * return: the sb instance
 */

sbcb *
sb_create_null(
	void
) {
	return sb_create_blksize(0);
}

/*
 * sb_create
 *
 * create a new string builder with a default buffer block size.
 *
 *     in: nothing
 *
 * return: the sb instance
 */

sbcb *
sb_create(
	void
) {
	return sb_create_blksize(SBCB_DEFAULT_BLKSIZE);
}

/*
 * sb_create_string
 *
 * create a new string builder initialized with a string.
 *
 *     in: a string
 *
 * return: the sb instance
 */

sbcb *
sb_create_string(
	char *str
) {
	sbcb *sb = sb_create();
	sb_puts(sb, str);
	return sb;
}

/*
 * sb_create_file
 *
 * create a new string builder initialized with the contents
 * of the provided file stream. the entire file is read and
 * left positioned at the beginning of the file.
 *
 *     in: an open file stream
 *
 * return: the sb instance
 */

sbcb *
sb_create_file(
	FILE *ifile
) {
	rewind(ifile);
	struct stat info;
	fstat(fileno(ifile), &info);
	char *data_buf = malloc(info.st_size + 1);
	assert(data_buf);
	memset(data_buf, 0, info.st_size + 1);
	fread(data_buf, info.st_size, 1, ifile);
	sbcb *sb = sb_create_string(data_buf);
	memset(data_buf, 253, info.st_size + 1);
	free(data_buf);
	rewind(ifile);
	return sb;
}

/*
 * sb_reset
 *
 * reset string builder to an initial empty state. this clears but doesnot
 * release the buffer.
 *
 *     in: the sb instance
 *
 * return: nothing
 */

void
sb_reset(
	sbcb *sb
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	sb->buf_used = 0;
	if (sb->is_null)
		return;
	memset(sb->buf, 0, sb->buf_len);
}

/*
 * sb_destroy
 *
 * clear and release all storage for this instance.
 *
 *     in: the sb instance
 *
 * return: nothing
 */

void
sb_destroy(
	sbcb *sb
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	if (!sb->is_null) {
		memset(sb->buf, 253, sb->buf_len);
		free(sb->buf);
	}
	memset(sb, 253, sizeof(sbcb));
	free(sb);
}

/*
 * sb_length
 *
 * how long is the current string?
 *
 *     in: the sb instance
 *
 * return: character count
 */

size_t
sb_length(
	sbcb *sb
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	return sb->buf_used;
}

/*
 * sb_grow_buffer
 *
 * increase the buffer storage as needed. presently the buffer is a
 * contiguous block, but it could be segmented.
 *
 *     in: the sb instance
 *
 * return: nothing
 *
 * the buffer increases in 'blksize' increments. the old buffer
 * contents are copied to the new buffer and then the old buffer is
 * scrubbed and freed.
 */

static void
sb_grow_buffer(
	sbcb *sb
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	assert(!sb->is_null && "error trying to expand empty SBCB");
	int new_len = sb->buf_len + sb->blksize;
	char *new_buf = malloc(new_len);
	assert(new_buf);
	memset(new_buf, 0, new_len);
	memcpy(new_buf, sb->buf, sb->buf_len);
	memset(sb->buf, 253, sb->buf_len);
	free(sb->buf);
	sb->buf = new_buf;
	sb->buf_len = new_len;
}

/*
 * sb_putc
 *
 * append a single character to the string builder.
 *
 *     in: the sb instance
 *
 *     in: character
 *
 * return: nothing
 */

void
sb_putc(
	sbcb *sb,
	int c
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	if (!sb->is_null) {
		if (sb->buf_used == sb->buf_len)
			sb_grow_buffer(sb);
		sb->buf[sb->buf_used] = c;
	}
	sb->buf_used += 1;
}

/*
 * sb_to_string
 *
 * return a copy of the string builder's contents.
 *
 *     in: the sb instance
 *
 * return: string
 *
 * the client is reponsible for freeing the string.
 */

char *
sb_to_string(
	sbcb *sb
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	char *str = NULL;
	if (sb->is_null) {
		str = malloc(2);
		assert(str);
		str[0] = '\0';
		str[1] = '\0';
	} else {
		str = malloc(sb->buf_used + 1);
		assert(str);
		memcpy(str, sb->buf, sb->buf_used);
		str[sb->buf_used] = '\0';
	}
	return str;
}

/*
 * sb_puts
 *
 * append a string to the string bulder.
 *
 *     in: the sb instance
 *
 * return: string
 *
 * sb_puts behaves like fputs() and not puts() in the standard
 * library. it does not append a newline after the string.
 */

void
sb_puts(
	sbcb *sb,
	char *str
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	assert(str && "missing string on SB_PUTS");
	size_t additional = strlen(str);
	if (!sb->is_null) {
		size_t new_length = sb->buf_used + additional;
		while (new_length >= sb->buf_len)
			sb_grow_buffer(sb);
		strcpy(&sb->buf[sb->buf_used], str);
	}
	sb->buf_used += additional;
}
/* *** end priv *** */

#endif /* TXBSB_IMPLEMENTATION */
