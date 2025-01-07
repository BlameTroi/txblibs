/* txballoc.h -- Memory allocation tracker -- Troy Brumley BlameTroi@gmail.com */

/*
 * This is a header only implementation of a memory allocation tracker
 * for use in my library code. While I would like to add real pooling
 * and garbage collection in the future, I doubt I'll get to it.
 *
 * I had some leaky code and this was written to find it. The approach
 * is to replace the standard library calls `malloc', `calloc', and
 * `e' with wrapper macros that call hooks that include tracking.
 *
 * This is opt-in tracking. Eventually all of my library code will use
 * this but it won't interfere with non-library code.
 *
 * Released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * This software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TXBALLOC_USER        true
#define TXBALLOC_LIBRARY     false

/*
 * These functions should not be called directly. Instead use the
 * wrapper macros. `tinitialize' for user code initialization, and
 * `tsinitialize' for library code initialization, and so forth.
 */

void
txballoc_initialize(
	size_t pool,    /* max number of active allocates to track */
	uint16_t flags, /* configuration */
	bool user_or_libs,
	FILE *f         /* file stream to log on */
);

void *
txballoc_malloc(        /* *** do not call directly, use tmalloc *** */
	size_t n,       /* as in malloc # bytes */
	bool user_or_libs,
	char *f,        /* __FILE__ */
	int l           /* __LINE__ */
);

void *
txballoc_calloc(        /* *** do not call directly, use tcalloc *** */
	int c,          /* as in calloc, # cells */
	size_t n,       /* as in calloc, # bytes */
	bool user_or_libs,
	char *f,        /* __FILE__ */
	int l           /* __LINE__ */
);

void
txballoc_free(          /* *** do not call directly, use tfree *** */
	void *p,        /* as in free, @ block */
	bool user_or_libs,
	char *f,        /* __FILE__ */
	int l           /* __LINE__ */
);

void
txballoc_terminate(
	bool user_or_libs
);

/*
 * Wrapper macros for the library. There are two sets, one for user
 * code (prefix `t'), and one for library space code (prefix `ts').
 *
 * My library code uses the `ts' macros for memory managed by the
 * library. Memory that is returned for the user to manage is
 * allocated using the standard system calls.
 *
 * If either user or library space is not initialized, memory is
 * allocated without tracking.
 *
 * User space code can get a report of the library memory usage
 * and any leaks by issuing a `tsinitialize' before invoking
 * library code, and `tsterminate' at end of run.
 *
 * t(s)initialize(n, r, f) -- track up to 'n' concurrent allocations
 *                            with 'r' as option bits (see below)
 *                            and write any log/trace to stream 'f'
 * t(s)terminate           -- terminate tracking, report as in 'r'
 * t(s)malloc(n)           -- allocate 'n' bytes
 * t(s)calloc(c, n)        -- allocate and zero contiguous memory
 *                            to hold 'c' blocks each of 'n' bytes
 * t(s)free(p)             -- free the allocated memory at 'p'
 *
 * The reporting option bits will report allocations (malloc, calloc),
 * frees, freeing an already freed block (does not abort the run),
 * and any leaks detected.
 */

/* The report options: */
#define txballoc_f_allocs    (1 << 0)
#define txballoc_f_frees     (1 << 1)
#define txballoc_f_dup_frees (1 << 2)
#define txballoc_f_leaks     (1 << 3)

/* Common report flag combinations: */
#define txballoc_f_silent    (0)
#define txballoc_f_trace     (txballoc_f_allocs + txballoc_f_frees)
#define txballoc_f_errors    (txballoc_f_dup_frees + txballoc_f_leaks)
#define txballoc_f_full      (txballoc_f_trace + txballoc_f_errors)

/* User space wrappers: */
#define tinitialize(n, r, f) \
	txballoc_initialize((n), (r), TXBALLOC_USER, (f))

#define tterminate() \
	txballoc_terminate(TXBALLOC_USER)

#define tmalloc(n) \
	txballoc_malloc((n), TXBALLOC_USER, __FILE__, __LINE__)

#define tcalloc(c, n) \
	txballoc_calloc((c), (n), TXBALLOC_USER, __FILE__, __LINE__)

#define tfree(p) \
	txballoc_free((p), TXBALLOC_USER, __FILE__, __LINE__)

/* Library space wrappers: */
#define tsinitialize(n, r, f) \
	txballoc_initialize((n), (r), TXBALLOC_LIBRARY, (f))

#define tsterminate() \
	txballoc_terminate(TXBALLOC_LIBRARY)

#define tsmalloc(n) \
	txballoc_malloc((n), TXBALLOC_LIBRARY, __FILE__, __LINE__)

#define tscalloc(c, n) \
	txballoc_calloc((c), (n), TXBALLOC_LIBRARY, __FILE__, __LINE__)

#define tsfree(p) \
	txballoc_free((p), TXBALLOC_LIBRARY, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* txballoc.h ends here */
