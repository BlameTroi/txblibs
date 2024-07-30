/* md5.h -- blametroi's md5 hash functions */

/*
 * a header only implementation of an md-5 hash.
 *
 * the original code was by Bryce Wilson and he released it to the
 * public domain. i've massaged it to fit in my libraries, and as
 * with the rest of them, it's all public domain as far as i am
 * concerned.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * these are the preferred functions for calculating and md5, hiding the
 * details behind functions to work over an arbitrary block of memory,
 * a string, or an opened and correctly positioned file stream.
 *
 * returns the hash to *result, which is expected to be large enough
 * to hold 16 bytes.
 */

void
md5_bytes(
   void *input,
   size_t len,
   uint8_t *result
);

void
md5_string(
   char *input,
   uint8_t *result
);

void
md5_file(
   FILE *file,
   uint8_t *result
);

/*
 * these are the lower level api calls. initialize a context for a
 * hash calculation, update the hash on some input, finalize the
 * calculation, and return the result in the digest.
 */

typedef struct md5_context md5_context;

md5_context *
md5_allocate_context(
   void
);

void
md5_release_release(
   md5_context *ctx
);

void
md5_initialize(
   md5_context *ctx
);

void
md5_update(
   md5_context *ctx,
   uint8_t *input,
   size_t input_len
);

void
md5_finalize(
   md5_context *ctx
);

void
md5_get_digest(
   md5_context *ctx,
   uint8_t *result
);

#ifdef __cplusplus
}
#endif /* __cplusplus */
