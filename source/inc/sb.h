/* txbsb.h -- Simple String Builder -- Troy Brumley BlameTroi@gmail.com */

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
 * a handle to a string builder instance.
 */

typedef struct sbcb hsb;

/*
 * override HSB_DEFAULT_BLKSIZE if you wish. #define it before you
 * #include this library.
 */

#ifndef HSB_DEFAULT_BLKSIZE
#define HSB_DEFAULT_BLKSIZE 8192
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

hsb *
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

hsb *
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

hsb *
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

hsb *
sb_create_string(
	char *str
);

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
);

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
	hsb *sb
);

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
);

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
);

/*
 * sb_puts
 *
 * append a string to the string bulder.
 *
 *     in: the sb instance
 *
 * return: nothing
 *
 * sb_puts behaves like fputs() and not puts() in the standard
 * library. it does not append a newline after the string.
 */

void
sb_puts(
	hsb *sb,
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
	hsb *sb
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* txbsb.h ends here */
