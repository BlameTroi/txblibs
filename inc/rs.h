/* rs.h -- blametroi's string read stream library */

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
 * an instance of a particular string read stream.
 */

typedef struct rscb rscb;

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
 * the entire file will be read and stored as a single string.
 * the file left positioned at the beginning of the file
 */

rscb *
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

rscb *
rs_clone(
	rscb *original
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
	rscb *rs
);

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
	rscb *rs
);

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
);

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
	rscb *rs
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
	rscb *rs,
	size_t n
);

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
	rscb *rs
);

/*
 * rs_ungetc
 *
 * back the stream position up by one character. in spite of the name ungetc,
 * no character is pushed back onto the stream.
 *
 *     in: the rs instance
 *
 * return: the character or EOF
 */

int
rs_ungetc(
	rscb *rs
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
	rscb *rs
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
	rscb *rs,
	char *buffer,
	int buflen
);


#ifdef __cplusplus
}
#endif /* __cplusplus */
