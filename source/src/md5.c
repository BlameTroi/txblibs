/* txbmd5.c -- Calculate an MD5 hash -- Troy Brumley BlameTroi@gmail.com */

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

/*
 * B. Wilson's original header comment:
 *
 * Derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm
 * and modified slightly to be functionally identical but condensed into
 * control structures.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/abort_if.h"
#include "../inc/md5.h"

/*
 * Transparent definition, it's opaque in txbmd5.h.
 */

#define MD5_TAG "__MD5___"
#define MD5_TAG_LEN 8

#define ASSERT_MD5(p, m) \
	abort_if(!(p) || memcmp((p), MD5_TAG, MD5_TAG_LEN) != 0, (m));

#define ASSERT_MD5_OR_NULL(p, m) \
	abort_if(p && memcmp((p), MD5_TAG, MD5_TAG_LEN) != 0, (m));

struct md5_context {
	char tag[MD5_TAG_LEN];          /* eye catcher */
	uint64_t size;        /* size of input in bytes */
	uint32_t buffer[4];   /* current accumulation of hash */
	uint8_t input[64];    /* input to be used in the next step */
	uint8_t digest[16];   /* result of algorithm */
};

/*
 * Constants defined by the MD5 algorithm.
 */

#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476

static uint32_t S[] = {
	7, 12, 17, 22, 7, 12, 17, 22,
	7, 12, 17, 22, 7, 12, 17, 22,
	5,  9, 14, 20, 5,  9, 14, 20,
	5,  9, 14, 20, 5,  9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23,
	4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21,
	6, 10, 15, 21, 6, 10, 15, 21
};

static uint32_t K[] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

/*
 * Padding used to make the size (in bits) of the input congruent to
 * 448 mod 512.
 */

static uint8_t PADDING[] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
 * Bit-manipulation functions defined by the MD5 algorithm.
 */

#define F(X, Y, Z) ((X & Y) | (~X & Z))
#define G(X, Y, Z) ((X & Z) | (Y & ~Z))
#define H(X, Y, Z) (X ^ Y ^ Z)
#define I(X, Y, Z) (Y ^ (X | ~Z))

/*
 * Rotate 32-bit word left by `n' bits
 */

static uint32_t
rotate_left(
	uint32_t x,
	uint32_t n
) {
	return (x << n) | (x >> (32 - n));
}

/*
 * Step on 512 bits of input with the main MD5 algorithm.
 */

static void
md5_step(
	uint32_t *buffer,
	uint32_t *input
) {
	uint32_t AA = buffer[0];
	uint32_t BB = buffer[1];
	uint32_t CC = buffer[2];
	uint32_t DD = buffer[3];

	uint32_t E;

	unsigned int j;

	for (unsigned int i = 0; i < 64; ++i) {
		switch (i / 16) {
		case 0:
			E = F(BB, CC, DD);
			j = i;
			break;
		case 1:
			E = G(BB, CC, DD);
			j = ((i * 5) + 1) % 16;
			break;
		case 2:
			E = H(BB, CC, DD);
			j = ((i * 3) + 5) % 16;
			break;
		default:
			E = I(BB, CC, DD);
			j = (i * 7) % 16;
			break;
		}

		uint32_t temp = DD;
		DD = CC;
		CC = BB;
		BB = BB + rotate_left(AA + E + K[i] + input[j], S[i]);
		AA = temp;
	}

	buffer[0] += AA;
	buffer[1] += BB;
	buffer[2] += CC;
	buffer[3] += DD;
}

/*
 * Allocate and free, opaquely.
 */

MD5_context *
MD5_allocate_context(
	void
) {
	MD5_context *ctx = malloc(sizeof(MD5_context));
	memset(ctx, 0, sizeof(MD5_context));
	memcpy(ctx->tag, MD5_TAG, sizeof(MD5_TAG));
	return ctx;
}

void
MD5_release_context(
	MD5_context *ctx
) {
	ASSERT_MD5(ctx, "invalid md5 context");
	memset(ctx, 253, sizeof(MD5_context));
	free(ctx);
}

/*
 * (Re)Initialize a context
 */

void
MD5_initialize(
	MD5_context *ctx
) {
	ASSERT_MD5(ctx, "invalid md5 context");
	ctx->size = (uint64_t)0;
	ctx->buffer[0] = (uint32_t)A;
	ctx->buffer[1] = (uint32_t)B;
	ctx->buffer[2] = (uint32_t)C;
	ctx->buffer[3] = (uint32_t)D;
}

/*
 * dd some amount of input to the context
 *
 * If the input fills out a block of 512 bits, apply the algorithm
 * (md5_step) and save the result in the buffer. Also updates the
 * overall size.
 */

void
MD5_update(
	MD5_context *ctx,
	uint8_t *input_buffer,
	size_t input_len
) {
	ASSERT_MD5(ctx, "invalid md5 context");
	uint32_t input[16];
	unsigned int offset = ctx->size % 64;
	ctx->size += (uint64_t)input_len;

	/* copy each byte in input_buffer into the next space in our context input */
	for (unsigned int i = 0; i < input_len; ++i) {
		ctx->input[offset++] = (uint8_t)*(input_buffer + i);

		/* if we've filled our context input, copy it into our local
		 * array input then reset the offset to 0 and fill in a new
		 * buffer.
		 *
		 * every time we fill out a chunk, we run it through the
		 * algorithm to enable some back and forth between cpu and
		 * i/o */

		if (offset % 64 == 0) {
			for (unsigned int j = 0; j < 16; ++j) {

				/* convert to little-endian
				 *
				 * the local variable `input` our 512-bit chunk separated
				 * into 32-bit words we can use in calculations */

				input[j] = (uint32_t)(ctx->input[(j * 4) + 3]) << 24 |
					(uint32_t)(ctx->input[(j * 4) + 2]) << 16 |
					(uint32_t)(ctx->input[(j * 4) + 1]) <<  8 |
					(uint32_t)(ctx->input[(j * 4)]);
			}
			md5_step(ctx->buffer, input);
			offset = 0;
		}
	}
}

/*
 * Pad the current input to get to 448 bytes, append the size in bits
 * to the very end, and save the result of the final iteration into
 * digest.
 */

void
MD5_finalize(
	MD5_context *ctx
) {
	ASSERT_MD5(ctx, "invalid md5 context");
	uint32_t input[16];
	unsigned int offset = ctx->size % 64;
	unsigned int padding_length = offset < 56 ? 56 - offset : (56 + 64) - offset;

	/* fill in the padding and undo the changes to size that resulted
	 * from the update */

	MD5_update(ctx, PADDING, padding_length);
	ctx->size -= (uint64_t)padding_length;

	/* do a final update (internal to this function)
	 *
	 * last two 32-bit words are the two halves of the size (converted
	 * from bytes to bits) */

	for (unsigned int j = 0; j < 14; ++j) {
		input[j] = (uint32_t)(ctx->input[(j * 4) + 3]) << 24 |
			(uint32_t)(ctx->input[(j * 4) + 2]) << 16 |
			(uint32_t)(ctx->input[(j * 4) + 1]) <<  8 |
			(uint32_t)(ctx->input[(j * 4)]);
	}
	input[14] = (uint32_t)(ctx->size * 8);
	input[15] = (uint32_t)((ctx->size * 8) >> 32);

	md5_step(ctx->buffer, input);

	/* move the result into digest (convert from little-endian) */
	for (unsigned int i = 0; i < 4; ++i) {
		ctx->digest[(i * 4) + 0] = (uint8_t)((ctx->buffer[i] & 0x000000FF));
		ctx->digest[(i * 4) + 1] = (uint8_t)((ctx->buffer[i] & 0x0000FF00) >>  8);
		ctx->digest[(i * 4) + 2] = (uint8_t)((ctx->buffer[i] & 0x00FF0000) >> 16);
		ctx->digest[(i * 4) + 3] = (uint8_t)((ctx->buffer[i] & 0xFF000000) >> 24);
	}
}

/*
 * Functions that run the algorithm on the provided input and put the
 * digest into result. The result should be able to store at least 16
 * bytes.
 */

void
MD5_bytes(
	void *input,
	size_t len,
	uint8_t *result
) {
	MD5_context *ctx = MD5_allocate_context();
	MD5_initialize(ctx);
	MD5_update(ctx, (uint8_t *)input, len);
	MD5_finalize(ctx);
	memcpy(result, ctx->digest, 16);
	MD5_release_context(ctx);
}

void
MD5_string(
	char *input,
	uint8_t *result
) {
	MD5_context *ctx = MD5_allocate_context();
	MD5_initialize(ctx);
	MD5_update(ctx, (uint8_t *)input, strlen(input));
	MD5_finalize(ctx);
	memcpy(result, ctx->digest, 16);
	MD5_release_context(ctx);
}

void
MD5_file(
	FILE *file,
	uint8_t *result
) {
	char *input_buffer = malloc(1024);
	size_t input_size = 0;

	MD5_context *ctx = MD5_allocate_context();
	MD5_initialize(ctx);

	while ((input_size = fread(input_buffer, 1, 1024, file)) > 0)
		MD5_update(ctx, (uint8_t *)input_buffer, input_size);

	MD5_finalize(ctx);
	free(input_buffer);
	memcpy(result, ctx->digest, 16);
	MD5_release_context(ctx);
}

/*
 * Provide access to the result of initialize/update.../finalize for
 * someone not using the md5_file, md5_string, or md5_bytes functions.
 */

void
MD5_get_digest(
	MD5_context *ctx,
	uint8_t *result
) {
	ASSERT_MD5(ctx, "invalid md5 context");
	memcpy(result, ctx->digest, 16);
}

/* txbmd5.c ends here */
