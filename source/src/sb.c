/* txbsb.c -- Simple String Builder -- Troy Brumley BlameTroi@gmail.com */

/*
 * a header only implementation of a very basic string builder.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

/*
 * a header only implementation of a very basic string builder.
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../inc/abort_if.h"
#include "../inc/sb.h"

/*
 * an instance of a string builder.
 */

#define HSB_TAG "__HSB___"
#define HSB_TAG_LEN 8

#define ASSERT_HSB(p, m) \
	abort_if(!(p) || memcmp((p), HSB_TAG, HSB_TAG_LEN) != 0, (m));

#define ASSERT_HSB_OR_NULL(p, m) \
	abort_if(p && memcmp((p), HSB_TAG, HSB_TAG_LEN) != 0, (m));

struct sbcb {
	char tag[HSB_TAG_LEN];
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

hsb *
sb_create_blksize(
	size_t blksize
) {
	/* allocate the base block */
	hsb *sb = malloc(sizeof(*sb));
	memset(sb, 0, sizeof(*sb));
	memcpy(sb->tag, HSB_TAG, sizeof(sb->tag));
	sb->is_null = blksize == 0;

	/* if this is a null sink, we're done */
	if (sb->is_null)
		return sb;

	/* allocate the initial buffer */
	sb->buf = malloc(blksize);
	abort_if(!sb->buf,
		"sb_create_blksize could not allocate initial buffer");
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

hsb *
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

hsb *
sb_create(
	void
) {
	return sb_create_blksize(HSB_DEFAULT_BLKSIZE);
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

hsb *
sb_create_string(
	char *str
) {
	hsb *sb = sb_create();
	sb_puts(sb, str);
	return sb;
}

/*
 * sb_create_file
 *
 * create a new string builder initialized with the contents of the
 * provided file stream. the entire file is read and then left
 * positioned at the beginning of the file.
 *
 *     in: an open file stream
 *
 * return: the sb instance
 */

hsb *
sb_create_file(
	FILE *ifile
) {
	rewind(ifile);
	struct stat info;
	fstat(fileno(ifile), &info);
	char *data_buf = malloc(info.st_size + 1);
	abort_if(!data_buf,
		"sb_create_file could not allocate buffer for file contents");
	memset(data_buf, 0, info.st_size + 1);
	fread(data_buf, info.st_size, 1, ifile);
	hsb *sb = sb_create_string(data_buf);
	memset(data_buf, 253, info.st_size + 1);
	free(data_buf);
	rewind(ifile);
	return sb;
}

/*
 * sb_reset
 *
 * reset string builder to an initial empty state. this clears but
 * does not release the buffer.
 *
 *     in: the sb instance
 *
 * return: nothing
 */

void
sb_reset(
	hsb *sb
) {
	ASSERT_HSB(sb, "invalid HSB");
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
	hsb *sb
) {
	ASSERT_HSB(sb, "invalid HSB");
	if (!sb->is_null) {
		memset(sb->buf, 253, sb->buf_len);
		free(sb->buf);
	}
	memset(sb, 253, sizeof(*sb));
	free(sb);
}

/*
 * sb_length
 *
 * how long is the current string?
 *
 *     in: the sb instance
 *
 * return: size_t character count
 */

size_t
sb_length(
	hsb *sb
) {
	ASSERT_HSB(sb, "invalid HSB");
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
	hsb *sb
) {
	ASSERT_HSB(sb, "invalid HSB");
	abort_if(sb->is_null,
		"sb_grow_buffer error trying to expand empty HSB");
	int new_len = sb->buf_len + sb->blksize;
	char *new_buf = malloc(new_len);
	abort_if(!new_buf,
		"sb_grow_buffer could not allocate new buffer");
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
 *     in: integer character
 *
 * return: nothing
 */

void
sb_putc(
	hsb *sb,
	int c
) {
	ASSERT_HSB(sb, "invalid HSB");
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
	hsb *sb
) {
	ASSERT_HSB(sb, "invalid HSB");
	char *str = NULL;
	if (sb->is_null) {
		str = malloc(2);
		abort_if(!str,
			"sb_to_string could not allocate output string buffer");
		str[0] = '\0';
		str[1] = '\0';
	} else {
		str = malloc(sb->buf_used + 1);
		abort_if(!str,
			"sb_to_string could not allocate output string buffer");
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
	hsb *sb,
	char *str
) {
	ASSERT_HSB(sb, "invalid HSB");
	abort_if(!str,
		"sb_puts missing string to put");
	size_t additional = strlen(str);
	if (!sb->is_null) {
		size_t new_length = sb->buf_used + additional;
		while (new_length >= sb->buf_len)
			sb_grow_buffer(sb);
		strcpy(&sb->buf[sb->buf_used], str);
	}
	sb->buf_used += additional;
}

/* txbsb.c ends here */
