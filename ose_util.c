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

#include <string.h>

#include "ose.h"
#include "ose_assert.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_match.h"

int32_t ose_pstrlen(const char * const s)
{
    ose_assert(s);
    return ose_pnbytes(strlen(s));
}

bool ose_isAddressChar(const char c)
{
    if(c < 32)
    {
        return false;
    }
    switch(c)
    {
    case ' ':
    case '#':
    case '*':
    case ',':
        /* case '/': */
    case '?':
    case '[':
    case ']':
    case '{':
    case '}':
        return false;
    default: return true;
    }
}

bool ose_isKnownTypetag(const char typetag)
{
    switch(typetag)
    {
    case OSETT_ID:
    case OSETT_INT32:
    case OSETT_FLOAT:
    case OSETT_STRING:
    case OSETT_BLOB:
        return true;
#ifdef OSE_PROVIDE_TYPE_SYMBOL
    case OSETT_SYMBOL:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_DOUBLE
    case OSETT_DOUBLE:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
    case OSETT_INT8:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
    case OSETT_UINT8:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
    case OSETT_UINT32:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
    case OSETT_INT64:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
    case OSETT_UINT64:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_TIMETAG
    case OSETT_TIMETAG:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_TRUE
    case true:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_FALSE
    case OSETT_FALSE:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_NULL
    case OSETT_NULL:
        return true;
#endif
#ifdef OSE_PROVIDE_TYPE_INFINITUM
    case OSETT_INFINITUM:
        return true;
#endif
    default:
        return false;
    }
}

bool ose_isStringType(const char typetag)
{
    switch(typetag)
    {
    case OSETT_STRING:
#ifdef OSE_PROVIDE_TYPE_SYMBOL
    case OSETT_SYMBOL:
#endif
        return true;
    default:
        return false;
    }
}

bool ose_isIntegerType(const char typetag)
{
    switch(typetag)
    {
    case OSETT_INT32:
#ifdef OSE_PROVIDE_TYPE_INT8
    case OSETT_INT8:
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
    case OSETT_UINT8:
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
    case OSETT_UINT32:
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
    case OSETT_INT64:
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
    case OSETT_UINT64:
#endif
        return true;
    default:
        return false;
    }
}

bool ose_isFloatType(const char typetag)
{
    switch(typetag)
    {
    case OSETT_FLOAT:
#ifdef OSE_PROVIDE_TYPE_DOUBLE
    case OSETT_DOUBLE:
#endif
        return true;
    default:
        return false;
    }
}

bool ose_isNumericType(const char typetag)
{
    if(ose_isIntegerType(typetag)
       || ose_isFloatType(typetag)
#ifdef OSE_PROVIDE_TYPE_TIMETAG
       || typetag == OSETT_TIMETAG
#endif
        )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ose_isUnitType(const char typetag)
{
    switch(typetag)
    {
    case OSETT_TRUE:
    case OSETT_FALSE:
#ifdef OSE_PROVIDE_TYPE_NULL
    case OSETT_NULL:
#endif
#ifdef OSE_PROVIDE_TYPE_INFINITUM
    case OSETT_INFINITUM:
#endif
        return true;
    default:
        return false;
    }
}

bool ose_isBoolType(const char typetag)
{
    switch(typetag)
    {
    case OSETT_TRUE:
    case OSETT_FALSE:
        return true;
    default:
        return false;
    }
}

static bool isBundle(const char * const b)
{
    if(b)
    {
        if(b[0] && !strncmp(b, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool ose_isBundle(ose_constbundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    return isBundle(ose_getBundlePtr(bundle));
}

bool ose_bundleIsEmpty(ose_constbundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    {
        const int32_t s = ose_readSize(bundle);
        ose_assert(s >= 0);
        if(s > OSE_BUNDLE_HEADER_LEN)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

int32_t ose_getBundleElemCount(ose_constbundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    {
        const int32_t s = ose_readSize(bundle);
        int32_t o = OSE_BUNDLE_HEADER_LEN;
        int32_t i = 0;
        int32_t ss = 0;
        ose_assert(s >= 0);
        while(o < s)
        {
            ss = ose_readInt32(bundle, o);
            ose_assert(ss > 0);
            o += ss + 4;
            ++i;
        }
        return i;
    }
}

int32_t ose_getBundleElemElemCount(ose_constbundle bundle,
                                   const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(offset < ose_readSize(bundle));
    {
        const char * const b = ose_getBundlePtr(bundle);
        const char tt = ose_getBundleElemType(bundle, offset);
        ose_assert(tt == OSETT_BUNDLE || tt == OSETT_MESSAGE);
        if(tt == OSETT_BUNDLE)
        {
            ose_constbundle bb = ose_makeConstBundle(b + offset + 4);
            const int32_t c = ose_getBundleElemCount(bb);
            return c;
        }
        else
        {
            const int32_t tto = ose_getBundleElemTTOffset(bundle,
                                                          offset);
            ose_assert(tto > offset);
            ose_assert(tto < ose_readInt32(bundle, offset));
            {
                const int32_t c = strlen(b + tto) - 1;
                return c;
            }
        }
    }
}

bool ose_bundleHasAtLeastNElems(ose_constbundle bundle,
                                const int32_t n)
{
	ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(n >= 0);
    if(n == 0)
    {
        return true;
    }
    {
        const int32_t s = ose_readSize(bundle);
        int32_t o = OSE_BUNDLE_HEADER_LEN;
        int32_t i = 0;
        ose_assert(s >= OSE_BUNDLE_HEADER_LEN);
        while(o < s && i < n)
        {
            const int32_t ss = ose_readInt32(bundle, o);
            ose_assert(ss >= 0);
            o += ss + 4;
            i++;
        }
        return i == n ? true : false;
    }
}

char ose_getBundleElemType(ose_constbundle bundle,
                           const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= OSE_BUNDLE_HEADER_LEN);
    {
        const char * const b = ose_getBundlePtr(bundle);
        const int32_t s = ose_readSize(bundle); (void)s;
        ose_assert(s >= OSE_BUNDLE_HEADER_LEN);
        ose_assert(offset + 4 < s);
        if(isBundle(b + offset + 4))
        {
            return OSETT_BUNDLE;
        }
        else
        {
            return OSETT_MESSAGE;
        }
    }
}

#ifdef OSE_DEBUG
char ose_readByte(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset < ose_readSize(bundle));
    return ose_getBundlePtr(bundle)[offset];
}
#endif

#ifdef OSE_DEBUG
int32_t ose_writeByte(ose_bundle bundle,
                      const int32_t offset,
                      const char i)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset < ose_readSize(bundle));
    ose_getBundlePtr(bundle)[offset] = i;
    return 1;
}

#endif

#ifdef OSE_DEBUG
int32_t ose_readInt32(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    return ose_ntohl(*((int32_t *)(ose_getBundlePtr(bundle) + offset)));
}
#endif

#ifdef OSE_DEBUG
int32_t ose_writeInt32(ose_bundle bundle,
                       const int32_t offset,
                       const int32_t i)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    *((int32_t *)(ose_getBundlePtr(bundle) + offset)) = ose_htonl(i);
    return 4;
}
#endif

float ose_readFloat(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    {
    	const int32_t i = ose_readInt32(bundle, offset);
        const char * const p = (char *)&i;
        return *((float *)p);
    }
}

int32_t ose_writeFloat(ose_bundle bundle,
                       const int32_t offset,
                       const float f)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    {
        const char * const p = (char *)&f;
        ose_writeInt32(bundle, offset, *((int32_t *)p));
        return 4;
    }
}

#ifdef OSE_DEBUG
const char *ose_readString(ose_constbundle bundle,
                           const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset < ose_readSize(bundle));
#ifdef OSE_DEBUG
    /* if this function is ever included in non-debug builds, this
       inner block should still be removed at compile-time */
    {
        const char * const str = ose_getBundlePtr(bundle) + offset;
        const int32_t bs = ose_readSize(bundle);
    	const char * const p = memchr(str, 0, bs - offset);
        ose_assert(p && "string is not NULL-terminated");
    }
#endif 
    return ose_getBundlePtr(bundle) + offset;
}
#endif

#ifdef OSE_DEBUG
int32_t ose_getStringLen(ose_constbundle bundle,
                         const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset < ose_readSize(bundle));
    {
        const char * const str = ose_getBundlePtr(bundle) + offset;
        const int32_t bs = ose_readSize(bundle);
    	const char * const p = memchr(str, 0, bs - offset);
        ose_assert(p && "string is not NULL-terminated");
        return p - str;
    }
}
#endif

#ifdef OSE_DEBUG
int32_t ose_getPaddedStringLen(ose_constbundle bundle,
                               const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset < ose_readSize(bundle));
    {
        const char * const str = ose_getBundlePtr(bundle) + offset;
        const int32_t bs = ose_readSize(bundle);
    	const char * const p = memchr(str, 0, bs - offset);
        ose_assert(p && "string is not NULL-terminated");
        {
            const int32_t ps = ose_pnbytes(p - str);
            ose_assert(offset + ps <= bs);
            return ps;
        }
    }
}
#endif

#ifdef OSE_DEBUG
int32_t ose_writeString(ose_bundle bundle,
                        const int32_t offset,
                        const char * const s,
                        const int32_t len,
                        const int32_t plen)
{
    
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - plen);
    ose_assert(s);
    ose_assert(len >= 0);
    ose_assert(plen > len);
    memcpy(ose_getBundlePtr(bundle) + offset, s, len);
    {
        int32_t i;
        for(i = 0; i < plen - len; i++)
        {
            ose_writeByte(bundle, offset + len + i, 0);
        }
        return plen;
    }
}
#endif

#ifdef OSE_DEBUG
const char *ose_readBlob(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    return ose_getBundlePtr(bundle);
}

int32_t ose_readBlobSize(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    return ose_readInt32(bundle, offset);
}
#endif

int32_t ose_getBlobPaddingForNBytes(const int32_t n)
{
    ose_assert(n >= 0);
    {
        const int32_t nm4 = n % 4;
        if(nm4 != 0)
        {
            const int32_t r = 4 - (nm4);
            ose_assert(r >= 0);
            return r;
        }
        else
        {
        	return 0;
        }
    }
}

int32_t ose_getPaddedBlobSize(ose_constbundle bundle,
                              const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    {
        int32_t s = ose_readBlobSize(bundle, offset);
        ose_assert(s >= 0);
        {
            const int32_t sm4 = s % 4;
            if(sm4 != 0)
            {
                const int32_t r = s + 4 - sm4;
                ose_assert(r >= 0);
                return r;
            }
            else
            {
                return s;
            }
        }
    }
}

const char *ose_readBlobPayload(ose_constbundle bundle,
                                const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    ose_assert(ose_readInt32(bundle, offset) > 0);
    return ose_getBundlePtr(bundle) + offset + 4;
}

int32_t ose_writeBlob(ose_bundle bundle,
                      const int32_t offset,
                      const int32_t blobsize,
                      const char * const blob)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(blobsize >= 0);
    ose_assert(offset <= ose_readSize(bundle) -
               (blobsize + ose_getBlobPaddingForNBytes(blobsize) + 4));
    ose_assert(blob || !blob);
    {
        int32_t o = offset;
        o += ose_writeInt32(bundle, o, blobsize);
        if(blobsize == 0)
        {
            return 4;
        }
        if(blob)
        {
            memcpy(ose_getBundlePtr(bundle) + o, blob, blobsize);
        }
        else
        {
            memset(ose_getBundlePtr(bundle) + o, 0, blobsize);
        }
        o += blobsize;
        while(o % 4)
        {
            ose_writeByte(bundle, o, 0);
            o++;
        }
        return o - offset;
    }
}

#ifdef OSE_PROVIDE_TYPE_DOUBLE
double ose_readDouble(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    const char * const b = ose_getBundlePtr(bundle) + offset;
    const int64_t i = ose_ntohll(*((int64_t *)b));
    return *((double *)&i);
}

int32_t ose_writeDouble(ose_bundle bundle,
                        const int32_t offset,
                        const double f)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    char *b = ose_getBundlePtr(bundle) + offset;
    const int64_t i = *((int64_t *)&f);
    *((int64_t *)b) = ose_htonll(i);
    return 8;
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT32
uint32_t ose_readUInt32(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    return ose_ntohl(*((uint32_t *)(ose_getBundlePtr(bundle) + offset)));
}

int32_t ose_writeUInt32(ose_bundle bundle,
                        const int32_t offset,
                        const uint32_t i)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 4);
    *((uint32_t *)(ose_getBundlePtr(bundle) + offset)) = ose_htonl(i);
    return 4;
}
#endif

#ifdef OSE_PROVIDE_TYPE_INT64
int64_t ose_readInt64(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    return ose_ntohll(*((int64_t *)(ose_getBundlePtr(bundle) + offset)));
}

int32_t ose_writeInt64(ose_bundle bundle,
                       const int32_t offset,
                       const int64_t i)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    *((int64_t *)(ose_getBundlePtr(bundle) + offset)) = ose_htonl(i);
    return 8;
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT64
uint64_t ose_readUInt64(ose_constbundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    return ose_ntohll(*((uint64_t *)(ose_getBundlePtr(bundle) +
                                     offset)));
}

int32_t ose_writeUInt64(ose_bundle bundle,
                        const int32_t offset,
                        const uint64_t i)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    *((uint64_t *)(ose_getBundlePtr(bundle) + offset)) = ose_htonl(i);
    return 8;
}
#endif

#ifdef OSE_PROVIDE_TYPE_TIMETAG
struct ose_timetag ose_readTimetag(ose_constbundle bundle,
                                   const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    {
        const char * const b = ose_getBundlePtr(bundle) + offset;
        struct ose_timetag tt =
            {
                ose_ntohl(*((uint32_t *)b)),
                ose_ntohl(*((uint32_t *)(b + 4)))
            };
        return tt;
    }
}

int32_t ose_writeTimetag(ose_bundle bundle,
                         const int32_t offset,
                         const uint32_t sec,
                         const uint32_t fsec)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(offset <= ose_readSize(bundle) - 8);
    char *b = ose_getBundlePtr(bundle) + offset;
    *((uint32_t *)b) = ose_htonl(sec);
    *((uint32_t *)(b + 4)) = ose_htonl(fsec);
    return 8;
}
#endif

const void *ose_readAlignedPtr(ose_constbundle bundle,
                               const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(OSE_INTPTR2 == sizeof(intptr_t) * 2);
    ose_assert(offset <= ose_readSize(bundle) - OSE_INTPTR2);
    {
        const char * const b = ose_getBundlePtr(bundle) + offset;
        const int32_t alignment = ose_readInt32(bundle, offset);
        const intptr_t i = *((intptr_t *)(b + 4 + alignment));
        return (void *)i;
    }
}

int32_t ose_writeAlignedPtr(ose_bundle bundle,
                            const int32_t offset,
                            const void *ptr)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(OSE_INTPTR2 == sizeof(intptr_t) * 2);
    ose_assert(offset <= ose_readSize(bundle) - OSE_INTPTR2);
    {
        int32_t a = 0;
        char *b = ose_getBundlePtr(bundle);
        b += offset;
        memset(b, 0, OSE_INTPTR2);
        while((uintptr_t)(b + 4 + a) % sizeof(intptr_t))
        {
            a++;
        }
        *((int32_t *)b) = ose_htonl(a);
        *((intptr_t *)(b + 4 + a)) = (intptr_t)ptr;
        return OSE_INTPTR2;
    }
}

void ose_alignPtr(ose_bundle bundle, const int32_t offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(offset >= 0);
    ose_assert(OSE_INTPTR2 == sizeof(intptr_t) * 2);
    ose_assert(offset <= ose_readSize(bundle) - OSE_INTPTR2);
    {
        char * const b = ose_getBundlePtr(bundle) + offset;
        const int32_t aold = ose_readInt32(bundle, offset);
        int32_t anew = 0;
        while((uintptr_t)(b + 4 + anew) % sizeof(intptr_t))
        {
            anew++;
        }
        if(anew != aold)
        {
            *((int32_t *)b) = ose_htonl(anew);
            memmove(b + 4 + anew, b + 4 + aold, sizeof(intptr_t));
        }
    }
}

int32_t ose_getLastBundleElemOffset(ose_constbundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(ose_readSize(bundle) >= OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t bs = ose_readSize(bundle);
        if(bs == OSE_BUNDLE_HEADER_LEN)
        {
            return OSE_BUNDLE_HEADER_LEN;
        }
        {
            int32_t o = OSE_BUNDLE_HEADER_LEN;
            int32_t s = ose_readInt32(bundle, o);
            ose_assert(s >= 0);
            ose_assert(o + s + 4 <= bs);
            while(o + s + 4 < bs)
            {
                o += s + 4;
                s = ose_readInt32(bundle, o);
                ose_assert(s >= 0);
                ose_assert(o + s + 4 <= bs);
            }
            return o;
        }
    }
}

int32_t ose_getBundleElemAddressOffset(ose_constbundle bundle,
                                       const int32_t elemoffset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(elemoffset >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(elemoffset < ose_readSize(bundle) - (4 + OSE_ADDRESS_MIN_PLEN));
    ose_assert(ose_readInt32(bundle, elemoffset) > 0);
    return elemoffset + 4;
}

int32_t ose_getBundleElemTTOffset(ose_constbundle bundle,
                                  const int32_t elemoffset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(elemoffset >= OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t ao = ose_getBundleElemAddressOffset(bundle,
                                                          elemoffset);
        const int32_t to = ao + ose_getPaddedStringLen(bundle, ao);
        ose_assert(to <= ose_readSize(bundle) - 4);
        ose_assert(to - elemoffset < ose_readInt32(bundle, elemoffset) + 4);
        return to;
    }
}

int32_t ose_getBundleElemPayloadOffset(ose_constbundle bundle,
                                       const int32_t elemoffset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(elemoffset >= OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t to = ose_getBundleElemTTOffset(bundle, elemoffset);
        int32_t po = 0;
        if(ose_getBundleElemType(bundle, elemoffset) == OSETT_BUNDLE)
        {
            po = to + OSE_TIMETAG_LEN;
        }
        else
        {
            po = to + ose_getPaddedStringLen(bundle, to);
        }
        ose_assert(po < ose_readSize(bundle));
        ose_assert(po - elemoffset < ose_readInt32(bundle, elemoffset) + 4);
        return po;
    }
}

int32_t ose_getFirstOffsetForMatch(ose_constbundle bundle,
                                   const char * const addr)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(addr);
    {
        const char * const b = ose_getBundlePtr(bundle);
        int32_t o = OSE_BUNDLE_HEADER_LEN;
        const int32_t s = ose_readSize(bundle);
        ose_assert(s >= OSE_BUNDLE_HEADER_LEN);
        while(o < s)
        {
            const int32_t ss = ose_readInt32(bundle, o);
            ose_assert(ss > 0);
            if(!strncmp(b + o + 4, addr, ss))
            {
                return o;
            }
            o += ss + 4;
        }
        return 0;
    }
}

int32_t ose_getFirstOffsetForPMatch(ose_constbundle bundle,
                                    const char * const addr)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(addr);
    {
        const char * const b = ose_getBundlePtr(bundle);
        int32_t o = OSE_BUNDLE_HEADER_LEN;
        const int32_t s = ose_readSize(bundle);
        ose_assert(s >= OSE_BUNDLE_HEADER_LEN);
        while(o < s)
        {
            const int32_t ss = ose_readInt32(bundle, o);
            int po, ao;
            int r = 0;
            ose_assert(ss > 0);
            r = ose_match_pattern(b + o + 4, addr, &po, &ao);
            if(r & OSE_MATCH_ADDRESS_COMPLETE)
            {
                return o;
            }
            o += ss + 4;
        }
        return 0;
    }
}

int32_t ose_getFirstOffsetForFullPMatch(ose_constbundle bundle,
                                        const char * const addr)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(addr);
    {
        const char * const b = ose_getBundlePtr(bundle);
        int32_t o = OSE_BUNDLE_HEADER_LEN;
        const int32_t s = ose_readSize(bundle);
        ose_assert(s >= OSE_BUNDLE_HEADER_LEN);
        while(o < s)
        {
            const int32_t ss = ose_readInt32(bundle, o);
            int po, ao;
            int r = 0;
            ose_assert(ss > 0);
			r = ose_match_pattern(b + o + 4, addr, &po, &ao);
            if(r & OSE_MATCH_ADDRESS_COMPLETE
               && r & OSE_MATCH_PATTERN_COMPLETE)
            {
                return o;
            }
            o += ss + 4;
        }
        return 0;
    }
}

int32_t ose_getTypedDatumSize(const char typetag,
                              const char * const ptr)
{
    ose_assert(ose_isKnownTypetag(typetag));
    switch(typetag)
    {
    case OSETT_ID:
#ifdef OSE_PROVIDE_TYPE_TRUE
    case OSETT_TRUE:
#endif
#ifdef OSE_PROVIDE_TYPE_FALSE
    case OSETT_FALSE:
#endif
#ifdef OSE_PROVIDE_TYPE_NULL
    case OSETT_NULL:
#endif
#ifdef OSE_PROVIDE_TYPE_INFINITUM
    case OSETT_INFINITUM:
#endif
        return 0;
    case OSETT_INT32:
    case OSETT_FLOAT:
#ifdef OSE_PROVIDE_TYPE_INT8
    case OSETT_INT8:
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
    case OSETT_UINT8:
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
    case OSETT_UINT32:
#endif
        return 4;
    case OSETT_STRING:
#ifdef OSE_PROVIDE_TYPE_SYMBOL
    case OSETT_SYMBOL:
#endif
        ose_assert(ptr);
        return ose_pstrlen(ptr);
    case OSETT_BLOB:
        ose_assert(ptr);
    {
        int32_t s = ose_ntohl(*((int32_t *)ptr));
        while(s % 4)
        {
            s++;
        }
        return s + 4;
    }
#ifdef OSE_PROVIDE_TYPE_DOUBLE
    case OSETT_DOUBLE:
        return 8;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
    case OSETT_INT64:
        return 8;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
    case OSETT_UINT64:
        return 8;
#endif
#ifdef OSE_PROVIDE_TYPE_TIMETAG
    case OSETT_TIMETAG:
        return 8;
#endif
    default:
        return 0;
    }
}

int32_t ose_getPayloadItemSize(ose_constbundle bundle,
                               const char typetag,
                               const int32_t payload_offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(ose_isKnownTypetag(typetag));
    ose_assert(payload_offset >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(payload_offset < ose_readSize(bundle));
    return ose_getTypedDatumSize(typetag,
                                 ose_getBundlePtr(bundle)
                                 + payload_offset);
}

int32_t ose_getTypedDatumLength(const char typetag,
                                const char * const ptr)
{
    ose_assert(ose_isKnownTypetag(typetag));
    switch(typetag)
    {
    case OSETT_ID:
#ifdef OSE_PROVIDE_TYPE_TRUE
    case OSETT_TRUE:
#endif
#ifdef OSE_PROVIDE_TYPE_FALSE
    case OSETT_FALSE:
#endif
#ifdef OSE_PROVIDE_TYPE_NULL
    case OSETT_NULL:
#endif
#ifdef OSE_PROVIDE_TYPE_INFINITUM
    case OSETT_INFINITUM:
#endif
        return 0;
    case OSETT_INT32:
    case OSETT_FLOAT:
#ifdef OSE_PROVIDE_TYPE_INT8
    case OSETT_INT8:
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
    case OSETT_UINT8:
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
    case OSETT_UINT32:
#endif
        return 4;
    case OSETT_STRING:
#ifdef OSE_PROVIDE_TYPE_SYMBOL
    case OSETT_SYMBOL:
#endif
        ose_assert(ptr);
        return strlen(ptr);
    case OSETT_BLOB:
        ose_assert(ptr);
    {
        const int32_t s = ose_ntohl(*((int32_t *)ptr));
        return s;
    }
#ifdef OSE_PROVIDE_TYPE_DOUBLE
    case OSETT_DOUBLE:
        return 8;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
    case OSETT_INT64:
        return 8;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
    case OSETT_UINT64:
        return 8;
#endif
#ifdef OSE_PROVIDE_TYPE_TIMETAG
    case OSETT_TIMETAG:
        return 8;
#endif
    default:
        return 0;
    }
}

int32_t ose_getPayloadItemLength(ose_constbundle bundle,
                                 const char typetag,
                                 const int32_t payload_offset)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(ose_isKnownTypetag(typetag));
    ose_assert(payload_offset >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(payload_offset < ose_readSize(bundle));
    return ose_getTypedDatumLength(typetag,
                                 ose_getBundlePtr(bundle)
                                 + payload_offset);
}

/* n = 1 => rightmost item */
void ose_getNthPayloadItem(ose_constbundle bundle,
                           const int32_t n,
                           const int32_t o,
                           int32_t *_to,
                           int32_t *_ntt,
                           int32_t *_lto,
                           int32_t *_po,
                           int32_t *_lpo)
{
    int32_t to, ntt, po, i;
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(n > 0);
    ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(ose_readSize(bundle) > OSE_BUNDLE_HEADER_LEN);
    ose_assert(ose_readInt32(bundle, o) > 0);
    ose_assert(ose_readInt32(bundle, o) + 4 <= ose_readSize(bundle));
    to = o + 4 + ose_getPaddedStringLen(bundle, o + 4);
    ose_assert(to - o <= ose_readInt32(bundle, o));

    /* first typetag is the typetag id ',' */
    ntt = ose_getStringLen(bundle, to);
    ose_assert(ntt > n);

    po = to + ose_pnbytes(ntt);
    ose_assert(po - o <= ose_readInt32(bundle, o));
    
    *_to = to;
    *_po = po;
    *_ntt = ntt;
    for(i = 0; i < ntt - n; i++)
    {
        const char c = ose_readByte(bundle, to);
        ose_assert(ose_isKnownTypetag(c));
        {
            const int32_t s = ose_getPayloadItemSize(bundle, c, po);
            ose_assert(s >= 0);
            po += s;
            to++;
            ose_assert(po - (o + 4) < ose_readInt32(bundle, o));
            ose_assert(to < po);
        }
    }
    ose_assert(to < o + ose_readSize(bundle));
    ose_assert(po < o + ose_readSize(bundle));
    *_lto = to;
    *_lpo = po;
}

int32_t ose_vcomputeMessageSize(ose_bundle bundle,
                                const char * const address,
                                const int32_t addresslen,
                                const int32_t n,
                                va_list ap)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(address);
    ose_assert(addresslen >= 0);
    ose_assert(n >= 0);
    {
        const int32_t alenp = ose_pnbytes(addresslen);
        int32_t i;
        char tt;
        int32_t newmsg_size = alenp;
        int32_t newmsg_numtypetags = 1;
        ose_assert(alenp >= OSE_ADDRESS_MIN_PLEN);

        for(i = 0; i < n; i++)
        {
            tt = va_arg(ap, int);
            ++newmsg_numtypetags;
            switch(tt)
            {
            case OSETT_INT32:
                newmsg_size += 4;
                va_arg(ap, int32_t);
                break;
            case OSETT_FLOAT:
                newmsg_size += 4;
                va_arg(ap, double);
                break;
            case OSETT_STRING:
            {
                const char * const p = va_arg(ap, char*);
                ose_assert(p);
                newmsg_size += ose_pstrlen(p);
            }
            break;
            case OSETT_BLOB:
            {
                /* no need for assertions;
                   blobs can be 0 size and NULL pointers */
                const int32_t s = va_arg(ap, int32_t);
                newmsg_size += 4 + s + ose_getBlobPaddingForNBytes(s);
                va_arg(ap, char*);
            }
            break;
            case OSETT_ALIGNEDPTR:
                va_arg(ap, ose_fn);
                newmsg_size += OSE_INTPTR2 + 4;
                break;
            default:
                ose_assert(0 && "unknown typetag");
            }
        }
        newmsg_size += ose_pnbytes(newmsg_numtypetags);
        return newmsg_size + 4;
    }
}

int32_t ose_vwriteMessage(ose_bundle bundle,
                          const int32_t offset,
                          const char * const address,
                          const int32_t addresslen,
                          const int32_t n,
                          va_list ap)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(address);
    ose_assert(addresslen >= 0);
    ose_assert(n >= 0);
    {
        const int32_t o = offset;
        const int32_t alenp = ose_pnbytes(addresslen);
        const int32_t _tto = o + 4 + alenp;
        const int32_t _plo = _tto + ose_pnbytes(n + 1);
        int32_t tto = _tto;
        int32_t plo = _plo;
        int32_t s, i;
        char tt;
        ose_assert(alenp >= OSE_ADDRESS_MIN_PLEN);
        
        ose_writeString(bundle, o + 4, address, addresslen, alenp);
        ose_writeByte(bundle, tto++, OSETT_ID);

        if(!n)
        {
            s = plo - o;
            ose_writeInt32(bundle, o, s - 4);
            return s;
        }
    
        for(i = 0; i < n; i++)
        {
            tt = va_arg(ap, int);
            switch(tt)
            {
            case OSETT_INT32:
            {
                const int32_t v = va_arg(ap, int32_t);
                ose_writeByte(bundle, tto++, OSETT_INT32);
                plo += ose_writeInt32(bundle, plo, v);
                break;
            }
            case OSETT_FLOAT:
            {
                const float v = va_arg(ap, double);
                ose_writeByte(bundle, tto++, OSETT_FLOAT);
                plo += ose_writeFloat(bundle, plo, v);
                break;
            }
            case OSETT_STRING:
            {
                const char * const v = va_arg(ap, char*);
                {
                    const int32_t sl = strlen(v);
                    const int32_t psl = ose_pnbytes(sl);
                    ose_writeByte(bundle, tto++, OSETT_STRING);
                    plo += ose_writeString(bundle, plo, v, sl, psl);
                }
                break;
            }
            default:
            case OSETT_BLOB:
            {
                const int32_t len = va_arg(ap, int);
                const char * const blob = va_arg(ap, char*);
                ose_writeByte(bundle, tto++, tt);
                {
                    const int32_t wlen = ose_writeBlob(bundle,
                                                       plo,
                                                       len,
                                                       blob);
                    plo += wlen;
                }
                break;
            }
#ifdef OSE_PROVIDE_TYPE_SYMBOL
            case OSETT_SYMBOL:
            {
                const char * const v = va_arg(ap, char*);
                ose_assert(v);
                const int32_t sl = strlen(v);
                const int32_t psl = ose_pnbytes(sl);
                ose_writeByte(bundle, tto++, OSETT_STRING);
                plo += ose_writeString(bundle, plo, v, sl, psl);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_DOUBLE
            case OSETT_DOUBLE:
            {
                const double v = va_arg(ap, double);
                ose_writeByte(bundle, tto++, OSETT_DOUBLE);
                plo += ose_writeDouble(bundle, plo, v);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
            case OSETT_INT8:
            {
                const int v = va_arg(ap, int);
                ose_writeByte(bundle, tto++, OSETT_INT8);
                plo += ose_writeInt8(bundle, plo, v);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
            case OSETT_UINT8:
            {
                const int v = va_arg(ap, int);
                ose_writeByte(bundle, tto++, OSETT_UINT8);
                plo += ose_writeUInt8(bundle, plo, v);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
            case OSETT_UINT32:
            {
                const uint32_t v = va_arg(ap, uint32_t);
                ose_writeByte(bundle, tto++, OSETT_UINT32);
                plo += ose_writeUInt32(bundle, plo, v);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
            case OSETT_INT64:
            {
                const int64_t v = va_arg(ap, int64_t);
                ose_writeByte(bundle, tto++, OSETT_INT64);
                plo += ose_writeInt64(bundle, plo, v);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
            case OSETT_UINT64:
            {
                const uint64_t v = va_arg(ap, uint64_t);
                ose_writeByte(bundle, tto++, OSETT_UINT64);
                plo += ose_writeUInt64(bundle, plo, v);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_TIMETAG
            case OSETT_TIMETAG:
            {
                const int32_t sec = va_arg(ap, int32_t);
                const int32_t fsec = va_arg(ap, int32_t);
                ose_writeByte(bundle, tto++, OSETT_TIMETAG);
                plo += ose_writeTimetag(bundle, plo, sec, fsec);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_TRUE
            case OSETT_TRUE:
            {
                ose_writeByte(bundle, tto++, OSETT_TRUE);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_FALSE
            case OSETT_FALSE:
            {
                ose_writeByte(bundle, tto++, OSETT_FALSE);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_NULL
            case OSETT_NULL:
            {
                ose_writeByte(bundle, tto++, OSETT_NULL);
                break;
            }
#endif
#ifdef OSE_PROVIDE_TYPE_INFINITUM
            case OSETT_INFINITUM:
            {
                ose_writeByte(bundle, tto++, OSETT_INFINITUM);
                break;
            }
#endif
            case OSETT_ALIGNEDPTR:
            {
                const ose_fn fn = va_arg(ap, ose_fn);
                ose_writeByte(bundle, tto++, OSETT_BLOB);
                plo += ose_writeInt32(bundle, plo, OSE_INTPTR2);
                plo += ose_writeAlignedPtr(bundle, plo, (void *)fn);
                break;
            }
            }
        }

        s = plo - o;
        ose_writeInt32(bundle, o, s - 4);
        return s;
    }
}

int32_t ose_writeMessage(ose_bundle bundle,
                         const int32_t offset,
                         const char * const address,
                         const int32_t addresslen,
                         const int32_t n, ...)
{
    va_list ap;
    /* va_start(ap, n); */
    /* int32_t len = ose_vwriteMessage(bundle, */
    /*                                 offset, */
    /*                                 address, */
    /*                                 addresslen, */
    /*                                 n, */
    /*                                 ap); */
    /* va_end(ap); */
    /* return len; */
    int32_t ms = ose_vcomputeMessageSize(bundle,
                                         address,
                                         addresslen,
                                         n,
                                         ap);
    int32_t ms2 = 0;
    va_end(ap);
    ose_incSize(bundle, ms);
    va_start(ap, n);
    ms2 = ose_vwriteMessage(bundle,
                            offset,
                            address,
                            addresslen,
                            n,
                            ap);
    va_end(ap);
    ose_assert(ms == ms2);
    return ms;
}

struct ose_SLIPBuf ose_initSLIPBuf(unsigned char *buf,
                                   int32_t buflen)
{
    ose_assert(buf);
    ose_assert(buflen);
    {
        struct ose_SLIPBuf sb;
        memset(buf, 0, buflen);
        sb.buf = buf;
        sb.buflen = buflen;
        sb.count = sb.state = 0;
        return sb;
    }
}

int ose_SLIPDecode(unsigned char c, struct ose_SLIPBuf *s)
{
    int state = s->state;
    switch(state)
    {
    case 0:
        s->state = 1;
        if(c == OSE_SLIP_END)
        {
            break;
        }
        /* fall through */
    case 1:
        switch(c)
        {
        case OSE_SLIP_END:
            if(s->count > 0)
            {
                s->state = 0;
                if(s->count % 4 == 0)
                {
                    return 0; /* done */
                }
                else
                {
                    return -1; /* error */
                }
            }
            s->state = 0;
            break;
        case OSE_SLIP_ESC:
            s->state = 2;
            break;
        case 10:
        case 13:
            if(s->havenullbyte == 0)
            {
                /* s->buf[s->count++] = c; */
                s->count++;
                while(s->count % 4)
                {
                    s->count++;
                }
                s->buf[s->count] = OSETT_ID;
                s->count += 4;
                return 0;
            }
            else
            {
                /* fall through */
            }
        case 0:
            s->havenullbyte = 1;
        default:
            if(s->count < s->buflen)
            {
                s->buf[(s->count)++] = c;
            }
            else
            {
                s->state = 3;
            }
            break;
        }
        break;
    case 2:
        switch(c)
        {
        case OSE_SLIP_ESC_END:
            if(s->count < s->buflen)
            {
                s->buf[(s->count)++] = OSE_SLIP_END;
                s->state = 1;
            }
            else
            {
                s->state = 3;
            }
            break;
        case OSE_SLIP_ESC_ESC:
            if(s->count < s->buflen)
            {
                s->buf[(s->count)++] = OSE_SLIP_ESC;
                s->state = 1;
            }
            else
            {
                s->state = 3;
            }
            break;
        default:
            s->state = 3;
        }
        break;
    case 3:
        if(c == OSE_SLIP_END)
        {
            s->count = 0;
            s->state = 0;
        }
        break;
    }
    return 1;
}

/* -1 error */
/* >0 length */
int32_t ose_SLIPEncode(const unsigned char * const src,
                       int32_t srclen,
                       unsigned char *dest,
                       int32_t destlen)
{
    int32_t j = 0, i = 0;
    if(!dest)
    {
        return -1;
    }
    for( ; i < srclen; i++)
    {
        switch(src[i])
        {
        case OSE_SLIP_END:
            if(j + 2 > destlen)
            {
                return -1;
            }
            dest[j++] = OSE_SLIP_ESC;
            dest[j++] = OSE_SLIP_ESC_END;
            break;
        case OSE_SLIP_ESC:
            if(j + 2 > destlen)
            {
                return -1;
            }
            dest[j++] = OSE_SLIP_ESC;
            dest[j++] = OSE_SLIP_ESC_ESC;
            break;
        default:
            if(j + 1 > destlen)
            {
                return -1;
            }
            dest[j++] = src[i];
        }
    }
    if(j + 1 > destlen)
    {
        return -1;
    }
    dest[j++] = OSE_SLIP_END;
    return j;
}
