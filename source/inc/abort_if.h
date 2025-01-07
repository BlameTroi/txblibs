/* txbabort.h -- `assert' replacement -- Troy Brumley BlameTroi@gmail.com */

/*
 * Some conditions are both "should not occur" and "should not
 * continue" errors. I prefer using assert to detect errors and end
 * execution but it can be disabled via -D NDEBUG. That's a standard
 * definition for release builds. While I don't expect any of these
 * libraries to end up in released software I will follow the
 * standard.
 *
 * I'm replaced my preferred 'assert(must be true condition &&
 * "descriptive message")' with this 'abort_if' macro. This reversed
 * many conditional checks throughout these libraries.
 *
 * Released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * This software is dual-licensed to the public domain and under the
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

/* txbabort.h ends here */
