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
#include "ose_util.h"
#include "ose_context.h"

#define ose_readInt32_outOfBounds(b, o)\
    ose_ntohl(*((int32_t *)(ose_getBundlePtr((b)) + (o))))
#define ose_writeInt32_outOfBounds(b, o, i)\
    *((int32_t *)(ose_getBundlePtr((b)) + (o))) = ose_htonl((i))

#ifdef OSE_DEBUG
/* generate symbols in case we're in a debugger */
const int32_t ose_context_bundle_size_offset =
    OSE_CONTEXT_BUNDLE_SIZE_OFFSET;
const int32_t ose_context_total_size_offset =
    OSE_CONTEXT_TOTAL_SIZE_OFFSET;
const int32_t ose_context_parent_bundle_offset_offset =
    OSE_CONTEXT_PARENT_BUNDLE_OFFSET_OFFSET;
const int32_t ose_context_status_offset =
    OSE_CONTEXT_STATUS_OFFSET;
const int32_t ose_context_bundle_offset =
    OSE_CONTEXT_BUNDLE_OFFSET;
const int32_t ose_context_message_overhead =
    OSE_CONTEXT_MESSAGE_OVERHEAD;
const int32_t ose_context_status_message_size =
    OSE_CONTEXT_STATUS_MESSAGE_SIZE;
const int32_t ose_context_max_overhead =
    OSE_CONTEXT_MAX_OVERHEAD;
#endif

static int32_t writeContextMessage(ose_bundle bundle,
                                   const int32_t size,
                                   const char * const address)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(address);
    ose_assert(strlen(address) == 3);
    ose_assert(size >= OSE_CONTEXT_MESSAGE_OVERHEAD);
    ose_assert(size % 4 == 0);
    {
        const int32_t o = ose_readSize(bundle);
        const int32_t alen = 3;
        const int32_t palen = 4;
        const int32_t overhead = OSE_CONTEXT_MESSAGE_OVERHEAD;
        const int32_t freespace = size - overhead;
        const int32_t msize = size - 4;
        char *p = ose_getBundlePtr(bundle) + o;
        *((int32_t *)p) = ose_htonl(msize);
        p += 4;
        strncpy(p, address, alen);
        p += palen;
        /*
          ,
          i : unused
          i : status
          i : offset of data section relative to start of bundle
          i : total number of bytes
          b : bundle (blob)
          b : free space (blob)
        */
        strcpy(p, ",iiiibb");
        p += 8;

        /* unused */
        *((int32_t *)p) = 0;
        p += 4; 

        /* status */
        *((int32_t *)p) = 0;
        p += 4; 

        /* offset of data section relative to start of bundle */
        *((int32_t *)p) = ose_htonl(o + OSE_CONTEXT_BUNDLE_OFFSET);
        p += 4; 

        /* total number of bytes */
        *((int32_t *)p) = ose_htonl(freespace
                                    + OSE_BUNDLE_HEADER_LEN);
        p += 4; 

        /* bundle */
        *((int32_t *)p) = ose_htonl(OSE_BUNDLE_HEADER_LEN);
        p += 4;
        memcpy(p, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
        p += OSE_BUNDLE_HEADER_LEN;

        *((int32_t *)p) = ose_htonl(freespace);

        return freespace;
    }
}

int32_t ose_init(ose_bundle bundle,
                 const int32_t size,
                 const char * const address)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(address);
    ose_assert(strlen(address) == 3);
    ose_assert(size >= OSE_CONTEXT_MESSAGE_OVERHEAD);
    ose_assert(size % 4 == 0);
    {
        const int32_t fs = writeContextMessage(bundle, size, address);
        ose_writeInt32_outOfBounds(bundle, -4,
                                   ose_readInt32_outOfBounds(bundle, -4)
                                   + size);
        return fs;
    }
}

int32_t ose_pushContextMessage(ose_bundle bundle,
                               const int32_t size,
                               const char * const address)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(address);
    ose_assert(strlen(address) == 3);
    ose_assert(size >= OSE_CONTEXT_MESSAGE_OVERHEAD);
    ose_assert(size % 4 == 0);
    {
        int32_t bs1 = ose_readSize(bundle);
        int32_t bs2 = ose_readInt32_outOfBounds(bundle, bs1);
        ose_assert(size <= bs2);
        {
            const int32_t fs = writeContextMessage(bundle, size, address);
            ose_writeInt32_outOfBounds(bundle, -4,
                                       ose_readInt32_outOfBounds(bundle, -4)
                                       + size);
            bs1 = ose_readSize(bundle);
            bs2 -= size;
            ose_writeInt32_outOfBounds(bundle, bs1, bs2);
            return fs;
        }
    }
}

void ose_dropContextMessage(ose_bundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(ose_readSize(bundle) > OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t bs = ose_readSize(bundle);
        int32_t o = OSE_BUNDLE_HEADER_LEN;
        int32_t s = ose_readInt32(bundle, o);
        ose_assert(s >= OSE_CONTEXT_MESSAGE_OVERHEAD);
        while(o < bs)
        {
            if(o + s + 4 >= bs)
            {
                break;
            }
            o += s + 4;
            s = ose_readInt32(bundle, o);
        }
        ose_assert(o < bs);
        ose_assert(o + s + 4 == bs);
        memset(ose_getBundlePtr(bundle) + o, 0, s + 4);
        ose_decSize(bundle, s + 4);
    }
}

int32_t ose_spaceAvailable(ose_constbundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    {
        const int32_t ts =
            ose_readInt32_outOfBounds(bundle,
                                      OSE_CONTEXT_TOTAL_SIZE_OFFSET);
        const int32_t s = ose_readSize(bundle);
        ose_assert(ts > 0);
        ose_assert(s >= 0);
        return ts - s;
    }
}

ose_bundle ose_enter(ose_bundle bundle,
                     const char * const address)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(address);
    ose_assert(strlen(address) < 4);
    {
        const char * const b = ose_getBundlePtr(bundle);
        int32_t o = OSE_BUNDLE_HEADER_LEN;
        const int32_t s = ose_readSize(bundle);
        ose_assert(s > o);
        while(o < s)
        {
            const int32_t ss = ose_readInt32(bundle, o);
            ose_assert(ss > 0);
            if(b[4 + o] == address[0]
               && b[4 + o + 1] == address[1]
               && b[4 + o + 2] == address[2]
               && b[4 + o + 3] == address[3])
            {
                const int32_t oo = o + OSE_CONTEXT_BUNDLE_OFFSET;
                ose_bundle bb = ose_makeBundle((char *)b + oo);
                ose_assert(oo < s);
                return bb;
            }
            o += ss + 4;
        }
        ose_assert(0 && "address must exist");
        return bundle;
    }
}

ose_bundle ose_exit(ose_bundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    {
        const int32_t o =
            ose_readInt32_outOfBounds(bundle,
                                      OSE_CONTEXT_PARENT_BUNDLE_OFFSET_OFFSET);
        ose_assert(o > 0);
        return ose_makeBundle(ose_getBundlePtr(bundle) - o);
    }
}

#ifdef OSE_DEBUG
int32_t ose_readSize(ose_constbundle bundle)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    {
        const char * const b = ose_getBundlePtr(bundle);
        return ose_ntohl(*((int32_t *)(b + OSE_CONTEXT_BUNDLE_SIZE_OFFSET)));
    }
}
#endif

void ose_addToSize(ose_bundle bundle, const int32_t amt)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(ose_readSize(bundle) >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(ose_readSize(bundle) + amt >= OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t os = ose_readSize(bundle);
        const int32_t ns1 = os + amt;
        const int32_t ns2 =
            ose_readInt32_outOfBounds(bundle,
                                      OSE_CONTEXT_TOTAL_SIZE_OFFSET) - ns1;
        ose_assert(ns2 >= 0);
        if(amt < 0)
        {
            ose_writeInt32_outOfBounds(bundle, os, 0);
        }
        ose_writeInt32_outOfBounds(bundle, -4, ns1);
        ose_writeInt32_outOfBounds(bundle, ns1, ns2);
        ose_assert(ose_readSize(bundle) >= OSE_BUNDLE_HEADER_LEN);
        ose_assert(ose_readInt32_outOfBounds(bundle, ose_readSize(bundle)) >= 0);
    }
}

void ose_incSize(ose_bundle bundle, const int32_t amt)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(ose_readSize(bundle) >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(ose_readSize(bundle) + amt >= OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t os = ose_readSize(bundle);
        const int32_t ns1 = os + amt;
        const int32_t ns2 =
            ose_readInt32_outOfBounds(bundle,
                                      OSE_CONTEXT_TOTAL_SIZE_OFFSET) - ns1;
        ose_assert(ns2 >= 0);
        ose_writeInt32_outOfBounds(bundle, -4, ns1);
        ose_writeInt32_outOfBounds(bundle, ns1, ns2);
        ose_assert(ose_readSize(bundle) >= OSE_BUNDLE_HEADER_LEN);
        ose_assert(ose_readInt32_outOfBounds(bundle, ose_readSize(bundle)) >= 0);
    }
}

void ose_decSize(ose_bundle bundle, const int32_t amt)
{
    ose_assert(ose_getBundlePtr(bundle));
    ose_assert(ose_isBundle(bundle));
    ose_assert(ose_readSize(bundle) >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(ose_readSize(bundle) - amt >= OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t os = ose_readSize(bundle);
        const int32_t ns1 = os - amt;
        const int32_t ns2 =
            ose_readInt32_outOfBounds(bundle,
                                      OSE_CONTEXT_TOTAL_SIZE_OFFSET) - ns1;
        ose_assert(ns2 >= 0);
        ose_writeInt32_outOfBounds(bundle, os, 0);
        ose_writeInt32_outOfBounds(bundle, -4, ns1);
        ose_writeInt32_outOfBounds(bundle, ns1, ns2);
        ose_assert(ose_readSize(bundle) >= OSE_BUNDLE_HEADER_LEN);
        ose_assert(ose_readInt32_outOfBounds(bundle, ose_readSize(bundle)) >= 0);
    }
}

void ose_copyElemAtOffset(const int32_t srcoffset,
                          ose_constbundle src,
                          ose_bundle dest)
{
    ose_assert(ose_getBundlePtr(src));
    ose_assert(ose_isBundle(src));
    ose_assert(ose_getBundlePtr(dest));
    ose_assert(ose_isBundle(dest));
    ose_assert(ose_bundleHasAtLeastNElems(src, 1));
    {
        const char * const srcp = ose_getBundlePtr(src);
        char *destp = ose_getBundlePtr(dest);
        const int32_t src_elem_size = ose_readInt32(src, srcoffset) + 4;
        const int32_t dest_size = ose_readSize(dest);
        ose_assert(src_elem_size > 0);
        ose_incSize(dest, src_elem_size);
        memcpy(destp + dest_size,
               srcp + srcoffset,
               src_elem_size);
    }
}

void ose_copyBundle(ose_constbundle src, ose_bundle dest)
{
    ose_assert(ose_getBundlePtr(src));
    ose_assert(ose_isBundle(src));
    ose_assert(ose_readSize(src) >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(ose_getBundlePtr(dest));
    ose_assert(ose_isBundle(dest));
    ose_assert(ose_readSize(dest) >= OSE_BUNDLE_HEADER_LEN);
    {
        const int32_t ds = ose_readSize(dest);
        const int32_t ss = ose_readSize(src);
        ose_incSize(dest, ss + 4);
        memcpy(ose_getBundlePtr(dest) + ds,
               ose_getBundlePtr(src) - 4,
               ss + 4);
    }
}

int32_t ose_routeElemAtOffset(const int32_t srcoffset,
                              ose_constbundle src,
                              const int32_t prefixlen,
                              ose_bundle dest)
{
    ose_assert(ose_getBundlePtr(src));
    ose_assert(ose_isBundle(src));
    ose_assert(ose_bundleHasAtLeastNElems(src, 1));
    ose_assert(ose_getBundlePtr(dest));
    ose_assert(ose_isBundle(dest));
    ose_assert(srcoffset >= OSE_BUNDLE_HEADER_LEN);
    ose_assert(srcoffset < ose_readSize(src));
    ose_assert(ose_readInt32(src, srcoffset) > prefixlen);
    ose_assert(ose_readInt32(src, srcoffset) + 4 + OSE_BUNDLE_HEADER_LEN <= ose_readSize(src));
    ose_assert(prefixlen >= 0);
    ose_assert(prefixlen <= strlen(ose_getBundlePtr(src) + srcoffset + 4));
    {
        const char * const sb = ose_getBundlePtr(src);
        char *db = ose_getBundlePtr(dest);        
        const int32_t so = srcoffset;
        const int32_t ss = ose_readInt32(src, so);
        const int32_t d_o = ose_readSize(dest);
        
        const int32_t addrlen = (int32_t)strlen(sb + srcoffset + 4);
        const int32_t addrdiff = addrlen - prefixlen;
        const int32_t newaddrlen = (addrdiff
                                    ? addrdiff
                                    : OSE_ADDRESS_ANONVAL_LEN);
        const int32_t newaddrsize = ose_pnbytes(newaddrlen);
        const int32_t newsize = ((ss - ose_pnbytes(addrlen))
                                 + newaddrsize);
        
        int32_t i = so + 4 + prefixlen;
        int32_t c = d_o + 4;
        
        ose_addToSize(dest, newsize + 4);
        ose_writeInt32(dest, d_o, newsize);
        
        if(addrdiff)
        {
            memcpy(db + c, sb + i, addrdiff);
        }
        else
        {
            memcpy(db + c, OSE_ADDRESS_ANONVAL,
                   OSE_ADDRESS_ANONVAL_SIZE);
        }
        i += addrdiff;
        i++;
        while(i % 4)
        {
            i++;
        }
        c += newaddrsize;
        memcpy(db + c, sb + i, ss - ose_pnbytes(addrlen));
        return newsize;
    }
}

void ose_appendBundle(ose_bundle src, ose_bundle dest)
{
    ose_assert(ose_getBundlePtr(src));
    ose_assert(ose_isBundle(src));
    ose_assert(ose_getBundlePtr(dest));
    ose_assert(ose_isBundle(dest));
    if(ose_readSize(src) > OSE_BUNDLE_HEADER_LEN)
    {
        char *sp = ose_getBundlePtr(src);
        char *dp = ose_getBundlePtr(dest);
        const int32_t so = ose_getLastBundleElemOffset(src);
        const int32_t ss = ose_readInt32(src, so);
        const int32_t ds = ose_readSize(dest);
        ose_assert(ss > 0);
        ose_assert(ds >= OSE_BUNDLE_HEADER_LEN);
        /* if(so >= OSE_BUNDLE_HEADER_LEN) */
        /* { */
        if(!strncmp(sp + so + 4, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN))
        {
            ose_addToSize(dest, ss - OSE_BUNDLE_HEADER_LEN);
            memcpy(dp + ds,
                   sp + so + 4 + OSE_BUNDLE_HEADER_LEN,
                   ss - OSE_BUNDLE_HEADER_LEN);
        }
        else
        {
            ose_addToSize(dest, ss + 4);
            memcpy(dp + ds,
                   sp + so,
                   ss + 4);
        }
        {
            /* drop */
            memset(sp + so, 0, ss + 4);
            ose_decSize(src, ss + 4);
        }
    }
    else
    {
        /* if the source is empty, do nothing */
        ;
    }
}

void ose_replaceBundle(ose_bundle src, ose_bundle dest)
{
    ose_assert(ose_getBundlePtr(src));
    ose_assert(ose_isBundle(src));
    ose_assert(ose_getBundlePtr(dest));
    ose_assert(ose_isBundle(dest));
    ose_assert(ose_readSize(dest) >= OSE_BUNDLE_HEADER_LEN);
    {
        /* clear */
        int32_t ds = ose_readSize(dest);
        memset(ose_getBundlePtr(dest) + OSE_BUNDLE_HEADER_LEN,
               0, ds - OSE_BUNDLE_HEADER_LEN);
        ose_decSize(dest, (ds - OSE_BUNDLE_HEADER_LEN));
    }
    ose_appendBundle(src, dest);
}

ose_bundle ose_newBundleFromCBytes(int32_t nbytes, char *bytes)
{
    char *p = bytes;
    ose_assert(p);
    ose_assert(nbytes >= OSE_CONTEXT_MAX_OVERHEAD);
    ose_assert(OSE_CONTEXT_ALIGNMENT == 4);
    while((uintptr_t)p % OSE_CONTEXT_ALIGNMENT)
    {
        p++;
        nbytes--;
    }
    while(nbytes % OSE_CONTEXT_ALIGNMENT)
    {
        nbytes--;
    }
    memset(p, 0, nbytes);
    *((int32_t *)p) = ose_htonl(OSE_BUNDLE_HEADER_LEN);
    p += sizeof(int32_t);
    memcpy(p, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
    {
        ose_bundle bundle = ose_makeBundle(p);
        int32_t n = ose_init(bundle,
                             OSE_CONTEXT_MESSAGE_OVERHEAD
                             + OSE_CONTEXT_STATUS_MESSAGE_SIZE,
                             "/sx");
        ose_assert(n == OSE_CONTEXT_STATUS_MESSAGE_SIZE);
        n = ose_init(bundle,
                     nbytes - (4 + OSE_BUNDLE_HEADER_LEN
                               + OSE_CONTEXT_MESSAGE_OVERHEAD
                               + OSE_CONTEXT_STATUS_MESSAGE_SIZE),
                     "/cx");
        ose_assert(n >= 0);
        return ose_enter(bundle, "/cx");
    }
}
