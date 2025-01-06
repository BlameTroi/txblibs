/* txbabort.h -- a replacement for assert -- blametroi@gmail.com */
#ifndef TXBABORT_H
#define TXBABORT_H
/*
 * some conditions are both "should not occur" and "should not
 * continue" errors. i prefer using assert to detect errors and end
 * execution but it can be disabled via -D NDEBUG. that's a standard
 * definition for release builds. while i don't expect any of these
 * libraries to end up in released software i will follow the
 * standard.
 *
 * i'm replacing my preferred 'assert(must be true condition &&
 * "descriptive message")' with this 'abort_if' macro. this will
 * reverse various conditional checks throughout these libraries.
 *
 * released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * this software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#include <stdio.h>

#define abort_if(condition, message) \
	do { \
		if ((condition)) { \
			fprintf(stderr, "abort: '%s' at %s:%d\n", message, __FILE__, __LINE__); \
			abort(); \
		} \
	} while(0)
#endif /* TXBABORT_H */
/* txbabort.h ends here */
