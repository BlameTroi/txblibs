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
 * create a new empty string builder with no backing buffer. it's
 * /dev/null for string builders.
 */

sbcb *
sb_create_null(
	void
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
 * create a new string buffer with an initial string.
 */

sbcb *
sb_create_string(
	char *str
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

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*
 * an instance of a string builder.
 */

#define SBCB_TAG "__SBCB__"
#define SBCB_TAG_LEN 8
#define ASSERT_SBCB(p, m) assert((p) && memcmp((p), SBCB_TAG, SBCB_TAG_LEN) == 0 && (m))
#define ASSERT_SBCB_OR_NULL(p) assert((p) == NULL || memcmp((p), SBCB_TAG, SBCB_TAG_LEN) == 0)


#define SBCB_DEFAULT_BLKSIZE 256

struct sbcb {
	char tag[SBCB_TAG_LEN];
	char *buf;
	size_t blksize;
	size_t buf_len;
	size_t buf_used;
	bool is_null;
};

/*
 * create a new string builder with the specified buffer block size.
 */

sbcb *
sb_create_blksize(
	size_t blksize
) {
	sbcb *sb = malloc(sizeof(sbcb));
	assert(sb);
	memset(sb, 0, sizeof(sbcb));
	memcpy(sb->tag, SBCB_TAG, sizeof(sb->tag));
	sb->is_null = blksize == 0;
	if (!sb->is_null) {
		sb->buf = malloc(blksize);
		assert(sb->buf);
		memset(sb->buf, 0, blksize);
		sb->buf_len = blksize;
		sb->blksize = blksize;
	}
	return sb;
}

/*
 * a '/dev/null' sink.
 */

sbcb *
sb_create_null(
	void
) {
	return sb_create_blksize(0);
}

/*
 * create a new string builder with a default buffer block size.
 */

sbcb *
sb_create(
	void
) {
	return sb_create_blksize(SBCB_DEFAULT_BLKSIZE);
}

/*
 * create a new string buffer with an initial string.
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
 * reset string builder to empty.
 */

void
sb_reset(
	sbcb *sb
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	if (!sb->is_null)
		memset(sb->buf, 0, sb->buf_len);
	sb->buf_used = 0;
}

/*
 * free all resources for this string builder.
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
 * how long is the current string in the builder?
 */

size_t
sb_length(
	sbcb *sb
) {
	ASSERT_SBCB(sb, "invalid SBCB");
	return sb->buf_used;
}

/*
 * increase the buffer storage as needed. presently the buffer
 * is a contiguous block, but it could be segmented.
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
 * append a single character to the string builder.
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
 * return a copy of the string builder's contents.
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
 * append a string to the string bulder.
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
