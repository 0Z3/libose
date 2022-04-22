/*
  Copyright (c) 2019-22 John MacCallum Permission is hereby granted,
  free of charge, to any person obtaining a copy of this software and
  associated documentation files (the "Software"), to deal in the
  Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom
  the Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/**
 * @file ose_assert.h
 * @brief Provides an assertion macro.
 */

#ifndef OSE_ASSERT_H
#define OSE_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/** @brief Assertion macro. Can be turned off by defining NDEBUG 
 */

#ifdef NDEBUG
#define ose_assert(t) do {} while(0)
#else
#define ose_assert(t)                              \
	!(t)                                                \
	? ((void)fprintf(stderr,                            \
                     "Assertion failed: %s, "           \
                     "function %s, "                    \
                     "file %s, "                        \
                     "line %d.\n",                      \
                     #t, __func__, __FILE__, __LINE__),	\
	   abort())                                         \
	: (void)0
#endif

#define ose_rassert(t, b) assert(t);

#ifdef OSE_DEBUG
#define ose_always(t) ((t) ? 1 : (ose_assert(0), 0))
#define ose_never(t) ((t) ? (assert(0), 0) : 1)
#else
#define ose_always(t) (t)
#define ose_never(t) (t)
#endif

#ifdef __cplusplus
}
#endif

#endif
