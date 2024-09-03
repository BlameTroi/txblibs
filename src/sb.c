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

#include "../inc/sb.h"

/*
 * an instance of a string builder.
 */

#define SBCB_DEFAULT_BLKSIZE 256
#define SBCB_TAG "__SBCB__"

struct sbcb {
	char tag[8];
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
	assert(sb && memcmp(sb->tag, SBCB_TAG, sizeof(sb->tag)) == 0);
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
	assert(sb && memcmp(sb->tag, SBCB_TAG, sizeof(sb->tag)) == 0);
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
	assert(sb && memcmp(sb->tag, SBCB_TAG, sizeof(sb->tag)) == 0);
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
	assert(sb && memcmp(sb->tag, SBCB_TAG, sizeof(sb->tag)) == 0);
	assert(!sb->is_null);
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
	assert(sb && memcmp(sb->tag, SBCB_TAG, sizeof(sb->tag)) == 0);
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
	assert(sb && memcmp(sb->tag, SBCB_TAG, sizeof(sb->tag)) == 0);
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
	assert(sb && memcmp(sb->tag, SBCB_TAG, sizeof(sb->tag)) == 0);
	assert(str);
	size_t additional = strlen(str);
	if (!sb->is_null) {
		size_t new_length = sb->buf_used + additional;
		while (new_length >= sb->buf_len)
			sb_grow_buffer(sb);
		strcpy(&sb->buf[sb->buf_used], str);
	}
	sb->buf_used += additional;
}
