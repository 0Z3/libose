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

#include "ose_conf.h"
#include "ose.h"

const char * const ose_version = OSE_VERSION;
#ifdef OSE_DEBUG
const char * const ose_debug = "-- debug --";
#else
const char * const ose_debug = "";
#endif
const char * const ose_date_compiled = OSE_DATE_COMPILED;

#ifdef OSE_NEED_HTONL
int32_t htonl(int32_t x)
{
	const int32_t one = 1;
	const char test = *((char *)&one);
	if(test == 0){
		return x;
	}else{
		return OSE_BYTE_SWAP32(x);
	}
}
#endif

#ifdef OSE_NEED_NTOHL
int32_t ntohl(int32_t x)
{
	const int32_t one = 1;
	const char test = *((char *)&one);
	if(test == 0){
		return x;
	}else{
		return OSE_BYTE_SWAP32(x);
	}
}
#endif
