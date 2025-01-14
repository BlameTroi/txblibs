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
