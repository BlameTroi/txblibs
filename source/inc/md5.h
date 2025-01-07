/* txbmd5.h -- Calculate an MD5 hash -- Troy Brumley BlameTroi@gmail.com */

/*
 * A header only implementation of an md-5 hash.
 *
 * The original code was by Bryce Wilson and he released it to the
 * public domain. I've massaged it to fit in my libraries, and as
 * with the rest of them, it's all public domain as far as I am
 * concerned.
 *
 * This is not considered secure. I wanted it for several Advent
 * of Code problems.
 *
 * The externally visible functions and types are all prefixed by
 * upper cased 'MD5_'. Internal functions and types are all in lower
 * case.
 *
 * Released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * This software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * These are the preferred functions for calculating an MD5 hash, hiding the
 * details behind functions to work over an arbitrary block of memory,
 * a string, or an opened and correctly positioned file stream.
 *
 * Returns the hash to *result, which is expected to be at least 16 bytes.
 */

void
MD5_bytes(
	void *input,
	size_t len,
	uint8_t *result
);

void
MD5_string(
	char *input,
	uint8_t *result
);

void
MD5_file(
	FILE *file,
	uint8_t *result
);

/*
 * These are the lower level api calls. Initialize a context for a
 * hash calculation, update the hash on some input, finalize the
 * calculation, and retrieve the result from the digest.
 */

typedef struct md5_context MD5_context;

MD5_context *
MD5_allocate_context(
	void
);

void
MD5_release_context(
	MD5_context *ctx
);

void
MD5_initialize(
	MD5_context *ctx
);

void
MD5_update(
	MD5_context *ctx,
	uint8_t *input,
	size_t input_len
);

void
MD5_finalize(
	MD5_context *ctx
);

void
MD5_get_digest(
	MD5_context *ctx,
	uint8_t *result
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/* txbmd5.h ends here */
