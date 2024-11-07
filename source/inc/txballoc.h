/* txballoc.h -- memory allocation tracker */

/*
 * a memory allocation tracker. i know i have some leaky code, this
 * can help me find it. i know there are other tools, but many of
 * them don't work with macos/clang.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifndef TALLOC_H
#define TALLOC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>

void
tinit(size_t pool);

void *
itmalloc(size_t n, char *f, int l);

void *
itcalloc(size_t c, size_t n, char *f, int l);

void
itfree(void *p, char *f, int l);

void
tterm(FILE *f);

#define tmalloc(n) \
	itmalloc((n), __FILE__, __LINE__)

#define tcalloc(c, n) \
	itcalloc((c), (n), __FILE__, __LINE__)

#define tfree(p) \
	itfree((p), __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TXBALLOC_H */

#ifdef TXBALLOC_IMPLEMENTATION
#undef TXBALLOC_IMPLEMENTATION

/*
 * a memory allocation tracker. i know i have some leaky code, this
 * can help me find it. i know there are other tools, but many of
 * them don't work with macos/clang.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <txballoc.h>

/*
 * to use this trace, you first call tinit with the maximum number of
 * active (allocted but not yet freed) entries. then replace the
 * malloc/calloc:free pairs you want to trace with the macros tmalloc,
 * tcalloc, and tfree. the arguments are unchanged but the tracing
 * hooks are invoked with the __FILE__ and __LINE__ of the replaced
 * calls. finally, call tterm with an opened filestream (stdout or
 * stderr is sufficient) and a report of any leaked storage from the
 * hooked calls is printed.
 *
 * only one instance of the trace may be active at a time. if you
 * attempt two tinit calls without an intervening tterm call, the
 * program will terminate via an assert.
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
	int line;             /* __line__ of the allocation */
	void *addr;           /* address of allocation */
	size_t size;          /* size requested */
	char file[32];        /* basename of __file__ */
};

/*
 * globals here, only one tracer can be active at a time.
 */

static trace *table = NULL;
static bool active = false;   /* initialized and running? */
static int odometer = 0;      /* an indication of how many allocations */
static int capacity = 0;      /* number of trace table entries */
static int high = 0;          /* high water mark for active allocations */

/*
 * tinit
 *
 * initialize and enable allocation trace.
 *
 *     in: size_t number of entries in trace table
 *
 * return: nothing
 *
 * entries are assigned on c/malloc and released on free, so the
 * capacity of the trace needs to be large enough to handle the
 * maximum number of expected active (allocated but not yet freed)
 * entries.
 *
 * as memroy is plentiful these days, i recommend over allocation to
 * deal with creeping leaks.
 */

void
tinit(size_t n) {
	assert(!active);
	active = true;

	odometer = 0;
	capacity = n;
	table = calloc(capacity, sizeof(trace));
	assert(table);
	high = 0;
}

/*
 * itcalloc
 *
 * hook for tracing calloc calls.
 *
 *     in: size_t number of 'cells'
 *
 *     in: size_t size of one 'cell'
 *
 *     in: string __FILE__
 *
 *     in: integer __LINE__
 *
 * return: address of allocated storage
 *
 * if tracing is not active, return the result of the intended calloc.
 *
 * if tracing is active, multiply number * size and call itmalloc. the
 * tracing is all done there and it already clears allocated storage.
 */

void *
itcalloc(size_t c, size_t n, char *f, int l) {

	if (!active)
		return calloc(c, n);

	/* itmalloc clears allocated memory */
	return itmalloc(c * n, f, l);
}

/*
 * itmalloc
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
 * in, and then malloc and clear the requested memory.
 *
 * if the trace table is full, fail via an assert.
 */

void *
itmalloc(size_t n, char *f, int l) {

	if (!active)
		return malloc(n);

	odometer += 1;
	if (high >= capacity)
		assert(false); /* TODO: grow table */
	int i;
	for (i = 0; i < capacity; i++)
		if (table[i].number == 0)
			break;
	assert(i < capacity);
	if (i > high)
		high = i;
	table[i].number = odometer;
	table[i].size = n;
	char *p = f + strlen(f);
	int c = 0;
	while (p > f && *p != '/') {
		p -= 1;
		c += 1;
	}
	p += 1;
	c -= 1;
	if (c > sizeof(table[i].file) - 1)
		c = sizeof(table[i].file) - 1;
	strncpy(table[i].file, p, c);
	table[i].line = l;
	table[i].addr = malloc(n);
	memset(table[i].addr, 0, n);
	return table[i].addr;
}

/*
 * itfree
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
 * requested allocation does not exist in the trace table, fail via an
 * assert.
 *
 * i thought about logging these, but free should fail if the address
 * passed is invalid.
 */

void
itfree(void *p, char *f, int l) {
	if (!active) {
		free(p);
		return;
	}

	assert(high > 0);
	int i;
	for (i = 0; i < capacity; i++)
		if (table[i].addr == p)
			break;
	assert(i < capacity);
	memset(&table[i], 0, sizeof(table[i]));
	if (i == high)
		i -= 1;
	free(p);
}

/*
 * tterm
 *
 * terminate c/malloc logging and report any dangling allocations left
 * in the trace table.
 *
 *     in: opened filestream to write report on
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
tterm(FILE *f) {
	assert(active);
	fprintf(f, "\n***tterm memory leak report***\n");
	active = false;
	int leaked = 0;
	size_t size = 0;
	for (int i = 0; i < capacity; i++)
		if (table[i].number > 0) {
			leaked += 1;
			size += table[i].size;
			fprintf(f, "%d %s %d %zu\n", leaked, table[i].file, table[i].line,
				table[i].size);
		}
	free(table);
	fprintf(f, "\ntterm memory summary:\n high %d odometer %d leaked %d size %lu\n",
		high+1, odometer, leaked, size);
	high = 0;
	odometer = 0;
	capacity = 0;
}

#endif /* TXBALLOC_IMPLEMENTATION */
/* txballoc.h ends here */
