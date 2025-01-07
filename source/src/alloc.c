/* txballoc.c -- Memory allocation tracker -- Troy Brumley BlameTroi@gmail.com */

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
#include <stdlib.h>
#include <string.h>

#include "../inc/alloc.h"

/*
 * This allocation tracer uses wrapper macros to add instrumentation
 * (currently only location and size) to selected allocations. There
 * are two sets of wrappers, one for client code (prefix `t') and one
 * for my library code (prefix `ts').
 *
 * My library code uses the `ts' macros for memory managed by the
 * library. Memory returned for the user to manage is allocated using
 * the standard system calls.
 *
 * To use this trace, you first call `t(s)initialize' with the
 * maximum number of concurrent allocations (allocated but not yet
 * freed) to track, logging options, and a file stream handle for the
 * log. There is support for tracing my library code and client code.
 *
 * Then replace the `malloc/calloc':`free' pairs you want to trace
 * with the macros `t(s)malloc', `t(s)calloc', and `t(s)free'. The
 * arguments are unchanged but the tracing hooks are invoked with the
 * __FILE__ and __LINE__ of the replaced calls.
 *
 * Finally, call `t(s)terminate' to report on any leaked allocations
 * along with some other information.
 *
 * Only one instance of each "space" allocator may be active at a
 * time. If you attempt two `t(s)initization' calls without an
 * intervening `t(c)termination' call, the program will terminate via
 * an `abort'.
 *
 * Allocations are logged when made and removed when freed. The
 * address of the allocation is carried and is used as a key when
 * removing entries from the trace.
 *
 * Entries are laid down sequentially with no attempt at ordering. An
 * allocation is logged in the first empty slot found. Freeing clears
 * a slot, so there are likely to be holes in the trace table.
 *
 * There is a maximum number of active entries (set when initialized)
 * and execution terminates via an `abort' if the table fills.
 *
 * Only `c/malloc' and 'free' calls that have been replaced by
 * `t(s)calloc', `t(s)malloc', and `t(s)free' are tracked. If the
 * trace is not active (not started with t(s)initialize), the request
 * is passed directly to the run time library.
 */

/*
 * Use the wrapper macros for library managed memory:
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
 * Entries are assigned on c/malloc and released on free, so the
 * capacity of the trace needs to be large enough to handle the
 * maximum number of expected active (allocated but not yet freed)
 * entries.
 *
 * As memory is plentiful these days, I recommend over allocation to
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
	if (pool->active) abort();
	pool->active = true;

	pool->odometer = 0;
	pool->capacity = n;
	pool->table = calloc(pool->capacity, sizeof(trace));
	if (!pool->table) abort();
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
 * If tracing is not active, pass the request straight through to
 * calloc.
 *
 * If tracing is active, multiply number of cells * cell length and
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
 * Find the basename of a file path. This should work on either
 * MacOS/Linux or Windows.
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
 * If tracing is not active, return the result of the intended malloc.
 *
 * If tracing is active, find a free entry in the trace table, fill it
 * in, and then malloc the requested memory.
 *
 * If the trace table is full, fail via an `abort'.
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
		abort(); /* TODO: grow table or not? */

	/* find free trace table entry */
	int i;
	for (i = 0; i < pool->capacity; i++)
		if (pool->table[i].number == 0)
			break;

	/* no free entry found, abort */
	if (i >= pool->capacity) abort();

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
 * If tracing is not active, just free and return.
 *
 * If tracing is active, find the entry in the trace table for this
 * allocation, clear it out, and then free the memory block.
 *
 * If the trace table somehow underflows (an impossibility) or the
 * requested allocation does not exist in the trace table, report it
 * and return.
 *
 * I decided not to abort on underflow.
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
 * If tracing is not active we fail via an `abort'.
 *
 * The report is self explanatory.
 *
 * After the report completes, counters are cleared and the trace
 * table storage is released.
 */

void
txballoc_terminate(
	bool user_or_libs
) {
	pool *pool = user_or_libs ? &user_pool : &library_pool;
	if (!pool->active) abort();
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

/* txballoc.c ends here */
