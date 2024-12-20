/* txballoc.h -- memory allocation tracker */

/*
 * this is a header only implementation of a memory allocation tracker
 * for use in my library code. i have hopes to add pooling and limited
 * garbage collection in the future.
 *
 * i had some leaky code and this was written to find it. the approach
 * is to replace the standard library calls malloc, calloc, and free
 * with wrapper macros that call hooks with some additional
 * information for tracking.
 *
 * this is opt-in tracking. eventually all of my library code will use
 * this but it won't interfere with non-library code.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TXBALLOC_H
#define TXBALLOC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define TXBALLOC_USER        true
#define TXBALLOC_LIBRARY     false

#define txballoc_f_allocs    (1 << 0)
#define txballoc_f_frees     (1 << 1)
#define txballoc_f_dup_frees (1 << 2)
#define txballoc_f_leaks     (1 << 3)
#define txballoc_f_silent    (0)
#define txballoc_f_trace     (txballoc_f_allocs + txballoc_f_frees)
#define txballoc_f_errors    (txballoc_f_dup_frees + txballoc_f_leaks)
#define txballoc_f_full      (txballoc_f_trace + txballoc_f_errors)

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
#endif /* TXBALLOC_H */

#ifdef TXBALLOC_IMPLEMENTATION
#undef TXBALLOC_IMPLEMENTATION

/*
 * this is a header only implementation of a memory allocation tracker
 * for use in my library code. i have hopes to add pooling and limited
 * garbage collection in the future.
 *
 * i had some leaky code and this was written to find it. the approach
 * is to replace the standard library calls malloc, calloc, and free
 * with wrapper macros that call hooks with some additional
 * information for tracking.
 *
 * this is opt-in tracking. eventually all of my library code will use
 * this but it won't interfere with non-library code.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <txballoc.h>

/*
 * to use this trace, you first call txballoc_initialize with the
 * maximum number of concurrent allocations (allocated but not yet
 * freed) to track, logging options, and a file stream handle for the
 * log.
 *
 * then replace the malloc/calloc:free pairs you want to trace with
 * the macros tmalloc, tcalloc, and tfree. the arguments are unchanged
 * but the tracing hooks are invoked with the __FILE__ and __LINE__ of
 * the replaced calls.
 *
 * finally, call txballoc_terminate to report on any leaked
 * allocations along with some other information.
 *
 * only one instance of the trace may be active at a time. if you
 * attempt two txballoc_initization calls without an intervening
 * txballoc_termination call, the program will terminate via an
 * assert.
 *
 * allocations are logged when made and removed when freed. the
 * address of the allocation is carried and is used as a key when
 * removing entries from the trace.
 *
 * entries are laid down sequentially with no attempt at ordering. an
 * allocation is logged in the first empty slot found. freeing clears
 * a slot, so there are likely to be holes in the trace table.
 *
 * there is a maximum number of active entries (set when initialized)
 * and execution terminates via an assert if the table fills.
 *
 * only c/malloc and free calls that have been replaced by tcalloc,
 * tmalloc, and tfree are tracked. if the trace is not active, the
 * request is just passed directly to the run time library.
 */

typedef struct trace trace;

struct trace {
	int number;           /* allocation call number, the odometer */
	int line;             /* __LINE__ of the allocation */
	void *addr;           /* address of allocation */
	size_t size;          /* size requested */
	char file[32];        /* basename of __FILE__ */
};

/*
 * globals here, only one tracer can be active at a time.
 */

typedef struct pool pool;
struct pool {
	trace *table;
	bool active;           /* initialized and running? */
	int odometer;          /* an indication of how many allocations */
	int capacity;          /* number of trace table entries */
	int high;              /* high water mark for active allocations */
	uint16_t flags;        /* bit flags txballoc_f_... */
	FILE *report;          /* file to report on, defaults to stderr */
};

static pool user_pool;
static pool library_pool;

/*
 * txballoc_initialize
 *
 * initialize and enable allocation trace.
 *
 *     in: size_t number of entries in trace table
 *
 *     in: uint16_t reporting flags txballoc_f_...
 *
 *     in: FILE * file to report on, if NULL use stderr
 *
 * return: nothing
 *
 * entries are assigned on c/malloc and released on free, so the
 * capacity of the trace needs to be large enough to handle the
 * maximum number of expected active (allocated but not yet freed)
 * entries.
 *
 * as memory is plentiful these days, i recommend over allocation to
 * deal with creeping leaks.
 */


void
txballoc_initialize(
	size_t n,
	uint16_t request,
	bool user_or_libs,
	FILE *f
) {
	pool *pool = user_or_libs ? &user_pool : &library_pool;
	assert(!pool->active);
	pool->active = true;

	pool->odometer = 0;
	pool->capacity = n;
	pool->table = calloc(pool->capacity, sizeof(trace));
	assert(pool->table);
	pool->high = 0;
	pool->flags = request;
	pool->report = f == NULL ? stderr : f;
}

/*
 * txballoc_calloc
 *
 * hook for tracing calloc calls.
 *
 *     in: int number of 'cells'
 *
 *     in: size_t length of one 'cell'
 *
 *     in: string __FILE__
 *
 *     in: integer __LINE__
 *
 * return: address of allocated storage
 *
 * if tracing is not active, pass the request straight through to
 * calloc.
 *
 * if tracing is active, multiply number of cells * cell length and
 * call txballoc_malloc, clearing the memory before returning it to
 * the client.
 */

void *
txballoc_calloc(
	int c,
	size_t len,
	bool user_or_libs,
	char *f,
	int l
) {
	pool *pool = user_or_libs ? &user_pool : &library_pool;
	if (!pool->active) return calloc(c, len);

	return memset(txballoc_malloc(c * len, user_or_libs, f, l), c, len);
}

/*
 * find the basename of a file path. this should work on either
 * macos/linux or windows.
 */

static
char *
file_basename(
	char *f
) {
	char *p = f + strlen(f);
	while (p > f && *(p-1) != '/' && *(p-1) != '\\')
		p -= 1;
	return p;
}

/*
 * txballoc_malloc
 *
 * hook for tracing malloc calls.
 *
 *     in: size_t bytes requested
 *
 *     in: string __FILE__
 *
 *     in: integer __LINE__
 *
 * return: address of allocated storage
 *
 * if tracing is not active, return the result of the intended malloc.
 *
 * if tracing is active, find a free entry in the trace table, fill it
 * in, and then malloc the requested memory.
 *
 * if the trace table is full, fail via an assert.
 */

void *
txballoc_malloc(
	size_t n,
	bool user_or_libs,
	char *f,
	int l
) {
	pool *pool = user_or_libs ? &user_pool : &library_pool;
	if (!pool->active) return malloc(n);

	pool->odometer += 1;

	if (pool->high >= pool->capacity)
		assert(false); /* TODO: grow table or not? */

	/* find free trace table entry */
	int i;
	for (i = 0; i < pool->capacity; i++)
		if (pool->table[i].number == 0)
			break;

	/* no free entry found, abort */
	assert(i < pool->capacity);

	/* track high water mark */
	if (i > pool->high)
		pool->high = i;

	/* fill in table entry */
	pool->table[i].number = pool->odometer;
	pool->table[i].size = n;
	char *ft = file_basename(f);
	int c = strlen(ft);
	if (c > sizeof(pool->table[i].file) - 1)
		c = sizeof(pool->table[i].file) - 1;
	strncpy(pool->table[i].file, ft, c);
	pool->table[i].line = l;

	/* get the memory */
	pool->table[i].addr = malloc(n);

	/* report if enabled */
	if (pool->flags & txballoc_f_allocs)
		fprintf(pool->report, "alloc: %5d %p len %lu for %s %d\n",
			pool->table[i].number, pool->table[i].addr, pool->table[i].size,
			pool->table[i].file, pool->table[i].line);

	return pool->table[i].addr;
}

/*
 * txballoc_free
 *
 * hook for tracing free calls
 *
 *     in: addres of c/malloc to free
 *
 *     in: string __FILE__
 *
 *     in: integer __LINE__
 *
 * return: nothing
 *
 * if tracing is not active, just free and return.
 *
 * if tracing is active, find the entry in the trace table for this
 * allocation, clear it out, and then free the memory block.
 *
 * if the trace table somehow underflows (an impossibility) or the
 * requested allocation does not exist in the trace table, report it
 * and return. we could abort here but i decided not to.
 */

void
txballoc_free(
	void *p,
	bool user_or_libs,
	char *f,
	int l
) {
	pool *pool = user_or_libs ? &user_pool : &library_pool;
	if (!pool->active) {
		free(p);
		return;
	}

	/* find the entry for this allocation in the trace table.
	 * allocation address is the key. */
	int i;
	for (i = 0; i < pool->capacity; i++)
		if (pool->table[i].addr == p)
			break;

	/* no entry found. if the memory is already freed, calling
	 * free again will abort. i've decided to log the event and
	 * and return. */
	if (i >= pool->capacity) {
		char *ft = file_basename(f);
		if (pool->flags & txballoc_f_errors)
			fprintf(pool->report,
				"error: %5d %p for %s %d -- free not in trace, dup free?\n",
				pool->odometer, p, ft, l);
		return;
	}

	/* log the free. */
	if (pool->flags & txballoc_f_frees) {
		char *ft = file_basename(f);
		fprintf(pool->report, "free : %5d %p len %lu for %s %d\n",
			pool->table[i].number, p,
			pool->table[i].size, ft, l);
	}

	/* clear table entry and release the requested storage. */
	memset(&pool->table[i], 0, sizeof(pool->table[i]));
	free(p);
}

/*
 * txballoc_terminate
 *
 * terminate c/malloc logging and report any dangling allocations left
 * in the trace table.
 *
 *     in: nothing
 *
 * return: nothing
 *
 * if tracing is not active we fail via an assert.
 *
 * the report is self explanatory.
 *
 * after the report completes, counters are cleared and the trace
 * table storage is released.
 */

void
txballoc_terminate(
	bool user_or_libs
) {
	pool *pool = user_or_libs ? &user_pool : &library_pool;
	assert(pool->active);
	pool->active = false;
	if (pool->flags & txballoc_f_full) {
		fprintf(pool->report, "\n***txballoc termination memory leak report***\n");
		fprintf(pool->report, "%s pool\n", user_or_libs ? "user" : "library");
		int leaked = 0;
		size_t size = 0;
		for (int i = 0; i < pool->capacity; i++)
			if (pool->table[i].number > 0) {
				leaked += 1;
				size += pool->table[i].size;
				fprintf(pool->report, "%d @ %5d %p len %lu %s %d\n",
					leaked, pool->table[i].number, pool->table[i].addr,
					pool->table[i].size, pool->table[i].file, pool->table[i].line);
			}
		fprintf(pool->report,
			"\ntxballoc termination summary:\n[high %d][odometer %d][leaked %d][size %lu]\n",
			pool->high+1, pool->odometer, leaked, size);
	}
	free(pool->table);
	pool->table = NULL;
	pool->high = 0;
	pool->odometer = 0;
	pool->capacity = 0;
	pool->flags = 0;
}

#endif /* TXBALLOC_IMPLEMENTATION */
/* txballoc.h ends here */
