/*
 * single file header generated via:
 * buildhdr --macro TXBWARN --intro LICENSE --pub ./inc/warn.h
 */
/* *** begin intro ***
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2025 Troy Brumley
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   *** end intro ***
 */

#ifndef TXBWARN_SINGLE_HEADER
#define TXBWARN_SINGLE_HEADER
/* *** begin pub *** */
/* txbwarn.h -- Debugging helper macros -- Troy Brumley BlameTroi@gmail.com */

/*
 * Temporary drop in macros to help with debugging. Enable by defining
 * TXBWARN before including txbwarn.h, otherwise the macros are noops.
 *
 * I needed these while chasing a very obscure bug and figured I'd
 * keep them even though I don't think I'll use them often. Even if
 * the exact macro isn't needed, it's a reasonable starting template.
 *
 * Released to the public domain by Troy Brumley blametroi@gmail.com
 *
 * This software is dual-licensed to the public domain and under the
 * following license: you are granted a perpetual, irrevocable license
 * to copy, modify, publish, and distribute this file as you see fit.
 */

#ifdef TXBWARN
#define WARN_NULL(n, f)\
	if (!(n)) \
		fprintf(stderr, "%s called with unexpected NULL\n", #f)
#define WARN_NULL_TWO(n, o, f)\
	if ((n) && (!o)) \
		fprintf(stderr, "%s dropping a NULL on a pointer [%d]\n", #f)
#else
#define WARN_NULL(n, f)
#define WARN_NULL_TWO(n, o, f)
#endif

/* txbwarn.h ends here */
/* *** end pub *** */

#endif /* TXBWARN_SINGLE_HEADER */

#ifdef TXBWARN_IMPLEMENTATION
#undef TXBWARN_IMPLEMENTATION

#endif /* TXBWARN_IMPLEMENTATION */
