/*
  Copyright (c) 2019-22 John MacCallum Permission is hereby granted,
  free of charge, to any person obtaining a copy of this software
  and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute,
  sublicense, and/or sell copies of the Software, and to permit
  persons to whom the Software is furnished to do so, subject to the
  following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#ifndef OSE_ERRNO
#define OSE_ERRNO

#ifdef __cplusplus
extern "C" {
#endif

enum ose_errno
{
    OSE_ERR_NONE = 0,
    OSE_ERR_ELEM_TYPE,
    OSE_ERR_ITEM_TYPE,
    OSE_ERR_ELEM_COUNT,
    OSE_ERR_ITEM_COUNT,
    OSE_ERR_RANGE,
    OSE_ERR_UNKNOWN_TYPETAG,
};

#define ose_errno_set(b, e) ose_context_set_status(b, e)
#define ose_errno_get(b) ose_context_get_status(b)

#ifdef __cplusplus
}
#endif

#endif
