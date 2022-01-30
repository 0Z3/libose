/*
  Copyright (c) 2019-21 John MacCallum Permission is hereby granted,
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

#include <stdio.h>
#include <string.h>
#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_symtab.h"

#define OSE_BUILTIN_DEFN(name)                  \
    void ose_builtin_##name(ose_bundle bundle)	\
    {                                           \
        ose_##name(OSEVM_STACK(bundle));        \
    }

#define OSE_BUILTIN_DEFPRED(name)                           \
    void ose_builtin_##name(ose_bundle bundle)              \
    {                                                       \
        ose_bundle vm_s = OSEVM_STACK(bundle);              \
        /* this assertion is wrong, but will be replaced by a */    \
        /* version of ose_popInt32 that we can trust, and that  */  \
        /* sets the status on error */                              \
        /* ose_assert(ose_isIntegerType(ose_peekType(vm_s)));  \ */\
        int32_t i = ose_popInt32(vm_s);                     \
        bool r = ose_##name(i);                             \
        ose_pushInt32(vm_s, r == true ? 1 : 0);             \
    }

OSE_BUILTIN_DEFN(2drop)
OSE_BUILTIN_DEFN(2dup)
OSE_BUILTIN_DEFN(2over)
OSE_BUILTIN_DEFN(2swap)
OSE_BUILTIN_DEFN(drop)
OSE_BUILTIN_DEFN(dup)
OSE_BUILTIN_DEFN(nip)
OSE_BUILTIN_DEFN(notrot)
OSE_BUILTIN_DEFN(over)
OSE_BUILTIN_DEFN(pick)
OSE_BUILTIN_DEFN(pickBottom)
OSE_BUILTIN_DEFN(pickMatch)
OSE_BUILTIN_DEFN(roll)
OSE_BUILTIN_DEFN(rollBottom)
OSE_BUILTIN_DEFN(rollMatch)
OSE_BUILTIN_DEFN(rot)
OSE_BUILTIN_DEFN(swap)
OSE_BUILTIN_DEFN(tuck)

OSE_BUILTIN_DEFN(bundleAll)
OSE_BUILTIN_DEFN(bundleFromBottom)
OSE_BUILTIN_DEFN(bundleFromTop)
OSE_BUILTIN_DEFN(clear)
OSE_BUILTIN_DEFN(clearPayload)
OSE_BUILTIN_DEFN(join)
OSE_BUILTIN_DEFN(pop)
OSE_BUILTIN_DEFN(popAll)
OSE_BUILTIN_DEFN(popAllDrop)
OSE_BUILTIN_DEFN(popAllBundle)
OSE_BUILTIN_DEFN(popAllDropBundle)
OSE_BUILTIN_DEFN(push)
OSE_BUILTIN_DEFN(split)
OSE_BUILTIN_DEFN(unpack)
OSE_BUILTIN_DEFN(unpackDrop)
OSE_BUILTIN_DEFN(unpackBundle)
OSE_BUILTIN_DEFN(unpackDropBundle)

OSE_BUILTIN_DEFN(countElems)
OSE_BUILTIN_DEFN(countItems)
OSE_BUILTIN_DEFN(lengthAddress)
OSE_BUILTIN_DEFN(lengthTT)
OSE_BUILTIN_DEFN(lengthItem)
OSE_BUILTIN_DEFN(lengthsItems)
OSE_BUILTIN_DEFN(sizeAddress)
OSE_BUILTIN_DEFN(sizeElem)
OSE_BUILTIN_DEFN(sizeItem)
OSE_BUILTIN_DEFN(sizePayload)
OSE_BUILTIN_DEFN(sizesElems)
OSE_BUILTIN_DEFN(sizesItems)
OSE_BUILTIN_DEFN(sizeTT)
OSE_BUILTIN_DEFN(getAddresses)

OSE_BUILTIN_DEFN(blobToElem)
OSE_BUILTIN_DEFN(blobToType)
OSE_BUILTIN_DEFN(concatenateBlobs)
OSE_BUILTIN_DEFN(concatenateStrings)
OSE_BUILTIN_DEFN(copyAddressToString)
OSE_BUILTIN_DEFN(copyPayloadToBlob)
OSE_BUILTIN_DEFN(swapStringToAddress)
OSE_BUILTIN_DEFN(copyTTToBlob)
OSE_BUILTIN_DEFN(decatenateBlobFromEnd)
OSE_BUILTIN_DEFN(decatenateBlobFromStart)
OSE_BUILTIN_DEFN(decatenateStringFromEnd)
OSE_BUILTIN_DEFN(decatenateStringFromStart)
OSE_BUILTIN_DEFN(elemToBlob)
OSE_BUILTIN_DEFN(itemToBlob)
OSE_BUILTIN_DEFN(joinStrings)
OSE_BUILTIN_DEFN(moveStringToAddress)
OSE_BUILTIN_DEFN(splitStringFromEnd)
OSE_BUILTIN_DEFN(splitStringFromStart)
OSE_BUILTIN_DEFN(swap4Bytes)
OSE_BUILTIN_DEFN(swap8Bytes)
OSE_BUILTIN_DEFN(swapNBytes)
OSE_BUILTIN_DEFN(trimStringEnd)
OSE_BUILTIN_DEFN(trimStringStart)
OSE_BUILTIN_DEFN(match)
OSE_BUILTIN_DEFN(pmatch)
OSE_BUILTIN_DEFN(replace)
OSE_BUILTIN_DEFN(assign)
OSE_BUILTIN_DEFN(lookup)
OSE_BUILTIN_DEFN(route)
OSE_BUILTIN_DEFN(routeWithDelegation)
OSE_BUILTIN_DEFN(gather)
OSE_BUILTIN_DEFN(nth)

OSE_BUILTIN_DEFN(makeBlob)
OSE_BUILTIN_DEFN(pushBundle)

OSE_BUILTIN_DEFN(add)
OSE_BUILTIN_DEFN(sub)
OSE_BUILTIN_DEFN(mul)
OSE_BUILTIN_DEFN(div)
OSE_BUILTIN_DEFN(mod)
OSE_BUILTIN_DEFN(pow)
OSE_BUILTIN_DEFN(neg)
OSE_BUILTIN_DEFN(eql)
OSE_BUILTIN_DEFN(neq)
OSE_BUILTIN_DEFN(lte)
OSE_BUILTIN_DEFN(lt)
OSE_BUILTIN_DEFN(and)
OSE_BUILTIN_DEFN(or)

OSE_BUILTIN_DEFPRED(isAddressChar);
OSE_BUILTIN_DEFPRED(isKnownTypetag);
OSE_BUILTIN_DEFPRED(isStringType);
OSE_BUILTIN_DEFPRED(isIntegerType);
OSE_BUILTIN_DEFPRED(isFloatType);
OSE_BUILTIN_DEFPRED(isNumericType);
OSE_BUILTIN_DEFPRED(isUnitType);
OSE_BUILTIN_DEFPRED(isBoolType);

void ose_builtin_exec1(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_d = OSEVM_DUMP(osevm);

    /* move input to dump */
    ose_copyBundle(vm_i, vm_d);
    ose_clear(vm_i);

    /* copy env to dump  */
    ose_copyBundle(vm_e, vm_d);
    /* ose_replaceBundle(vm_s, vm_e); */

    /* put topmost stack element into input */
    ose_moveElem(vm_s, vm_i);
    if(ose_peekType(vm_i) == OSETT_BUNDLE)
    {
    	ose_popAllDrop(vm_i);
    }

    /* move topmost bundle to env */
    /* ose_replaceBundle(vm_s, vm_e); */

    /* move control to dump */
    ose_drop(vm_c);             /* drop our own exec command */
    ose_copyBundle(vm_c, vm_d);
    ose_clear(vm_c);
}

void ose_builtin_exec2(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_d = OSEVM_DUMP(osevm);

    /* move input to dump */
    ose_copyBundle(vm_i, vm_d);
    ose_clear(vm_i);

    /* move env to dump  */
    ose_copyBundle(vm_e, vm_d);
    /* ose_replaceBundle(vm_s, vm_e); */

    /* put topmost stack element into input */
    ose_moveElem(vm_s, vm_i);
    if(ose_peekType(vm_i) == OSETT_BUNDLE)
    {
    	ose_popAllDrop(vm_i);
    }

    /* move topmost bundle to env */
    ose_replaceBundle(vm_s, vm_e);

    /* move control to dump */
    ose_drop(vm_c);             /* drop our own exec command */
    ose_copyBundle(vm_c, vm_d);
    ose_clear(vm_c);
}

void ose_builtin_exec3(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_d = OSEVM_DUMP(osevm);

    /* move input to dump */
    ose_copyBundle(vm_i, vm_d);
    ose_clear(vm_i);

    /* move env to dump  */
    ose_copyBundle(vm_e, vm_d);
    /* ose_replaceBundle(vm_s, vm_e); */

    /* put topmost stack element into input */
    ose_moveElem(vm_s, vm_i);
    if(ose_peekType(vm_i) == OSETT_BUNDLE)
    {
    	ose_popAllDrop(vm_i);
    }

    /* move topmost bundle to env */
    ose_replaceBundle(vm_s, vm_e);

    /* unpack topmost bundle onto stack */
    ose_unpackDrop(vm_s);

    /* move control to dump */
    ose_drop(vm_c);             /* drop our own exec command */
    ose_copyBundle(vm_c, vm_d);
    ose_clear(vm_c);
}

void ose_builtin_exec(ose_bundle osevm)
{
    ose_builtin_exec2(osevm);    
}

void ose_builtin_if(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_pushInt32(vm_s, 0);
    ose_neq(vm_s);
    ose_roll(vm_s);
    ose_drop(vm_s);
    ose_copyBundle(vm_e, vm_s);
    ose_swap(vm_s);
    ose_pushString(vm_c, "/!/exec");
    ose_swap(vm_c);
}

void ose_builtin_dotimes(ose_bundle osevm)
{
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    int32_t n = ose_popInt32(vm_s);
    if(n > 0)
    {
        ose_pushInt32(vm_c, n - 1);
        ose_copyElem(vm_s, vm_c);
        ose_pushString(vm_c, "/!/drop");
        ose_pushString(vm_c, "/!/exec1");
        ose_pushString(vm_c, "/!/dotimes");
    }
    else
    {
        ose_drop(vm_s);
    }
}

void ose_builtin_copyBundle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_constbundle src = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_copyBundle(src, dest);
}

void ose_builtin_appendBundle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle src = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_appendBundle(src, dest);
}

void ose_builtin_replaceBundle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle src = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_replaceBundle(src, dest);
}

void ose_builtin_moveElem(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle src = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_moveElem(src, dest);
}

void ose_builtin_copyElem(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_constbundle src = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
    ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)), 1);
    ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
    ose_drop(vm_s);

    ose_copyElem(src, dest);
}

void ose_builtin_apply(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_d = OSEVM_DUMP(osevm);

    ose_rassert(ose_bundleHasAtLeastNElems(vm_s, 1), 1);
    while(1)
    {
        char elemtype = ose_peekType(vm_s);
        if(elemtype == OSETT_BUNDLE)
        {
            /* move input to dump */
            ose_copyBundle(vm_i, vm_d);
            ose_clear(vm_i);
            {
                /* move the contents of the bundle on the stack to
                   the input, and unpack it in reverse order */
                char *sp = ose_getBundlePtr(vm_s);
                char *ip = ose_getBundlePtr(vm_i);
                int32_t stackoffset = OSE_BUNDLE_HEADER_LEN;
                int32_t stacksize = ose_readInt32(vm_s, -4);
                ose_assert(stackoffset < stacksize);
                int32_t s = ose_readInt32(vm_s, stackoffset);
                while(stackoffset + s + 4 < stacksize)
                {
                    stackoffset += s + 4;
                    s = ose_readInt32(vm_s, stackoffset);
                }

                int32_t o1, o2;
                o1 = stackoffset + 4 + OSE_BUNDLE_HEADER_LEN;
                ose_incSize(vm_i, s - OSE_BUNDLE_HEADER_LEN);
                o2 = ose_readInt32(vm_i, -4);
                int32_t end = o1 + s - OSE_BUNDLE_HEADER_LEN;;
                while(o1 < end)
                {
                    int32_t ss = ose_readInt32(vm_s, o1);
                    o2 -= ss + 4;
                    memcpy(ip + o2, sp + o1, ss + 4);
                    o1 += ss + 4;
                }
                ose_dropAtOffset(vm_s, stackoffset);
            }

            /* copy environment to dump */
            ose_copyBundle(vm_e, vm_d);

            /* move stack to dump */
            /* ose_pushBundle(vm_d); */

            /* move control to dump */
            ose_drop(vm_c);
            /* ose_builtin_return leaves the env on the stack */
            /* ose_pushString(vm_c, "/!/drop"); */
            ose_pushString(vm_c, "/</_e");
            ose_copyBundle(vm_c, vm_d);
            ose_clear(vm_c);
            break;
        }
        else if(elemtype == OSETT_MESSAGE)
        {
            int32_t o = ose_getLastBundleElemOffset(vm_s);
            int32_t to, ntt, lto, po, lpo;
            ose_getNthPayloadItem(vm_s, 1, o,
                                  &to, &ntt, &lto, &po, &lpo);
            /*char itemtype = ose_peekMessageArgType(vm_s); */
            char itemtype = ose_getBundlePtr(vm_s)[lto];
            const char * const p = ose_getBundlePtr(vm_s) + lpo;
            /* if(ose_isStringType(itemtype) == OSETT_TRUE) */
            if(0)
            {
                /* int32_t mo = ose_getFirstOffsetForPMatch(vm_e, p); */
                /* if(mo >= OSE_BUNDLE_HEADER_LEN){ */
                /* 	ose_pushString(vm_c, p); */
                /* 	ose_push(vm_c); */
                /* 	ose_dropAtOffset(vm_s, o); */
                /* 	ose_copyElemAtOffset(mo, vm_e, vm_s); */
                /* 	continue; */
                /* } */
                /* /\* if it wasn't present in env, lookup in symtab *\/ */
                /* else{ */
                /* 	ose_fn f = ose_symtab_lookup_fn(p); */
                /* 	if(f){ */
                /* 		ose_dropAtOffset(vm_s, o); */
                /* 		//f(osevm); */
                /*         ose_pushAlignedPtr(vm_s, f); */
                /*         continue; */
                /* 		//break; */
                /* 	}else{ */
                /* 		break; */
                /* 	} */
                /* } */
                OSEVM_LOOKUP(osevm);
                if(ose_peekType(vm_s) == OSETT_MESSAGE
                   && ose_peekMessageArgType(vm_s) == OSETT_STRING)
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
            else if(itemtype == OSETT_BLOB)
            {
                int32_t len = ose_ntohl(*((int32_t *)p));
                if(len >= OSE_BUNDLE_HEADER_LEN
                   && !strncmp(OSE_BUNDLE_ID,
                               p + 4,
                               OSE_BUNDLE_ID_LEN))
                {
                    /* blob is a bundle */
                    ose_blobToElem(vm_s);
                    continue;
                }
                else
                {
                    /* blob is not a bundle */
                    int32_t o = p - ose_getBundlePtr(vm_s);
                    ose_alignPtr(vm_s, o + 4);
                    ose_fn f = (ose_fn)ose_readAlignedPtr(vm_s, o + 4);
                    if(f)
                    {
                        ose_drop(vm_s);
                        f(osevm);
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                /* 
                   If there's a message that contains something
                   other than a blob (which might contain a bundle
                   or a function pointer), we just leave it on the
                   stack. Effectively, this means that "application"
                   of a string, int, float, etc. means that the
                   object takes no arguments and returns itself.
                */
                break;
            }
        }
        else
        {
            ose_assert(0 && "encountered unknown element type!");
            break;
        }
    }
}

void ose_builtin_map(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_rassert(ose_bundleHasAtLeastNElems(vm_s, 2), 1);
    ose_swap(vm_s);
    char t = ose_peekType(vm_s);
    if(t == OSETT_BUNDLE)
    {
        ose_countItems(vm_s);
        int32_t n = ose_popInt32(vm_s);
        ose_popAll(vm_s);
        int i = 0, j = -1;
        ose_pushBundle(vm_s);
        /* ENm1 ... E0 B B - */
        /* B1 => items */
        /* B0 => elems */
        for(i = 0; i < n; i++)
        {
            /* ENm1 ... E0 B1 B0 - */
            ose_rot(vm_s);
            /* ENm1 ... B1 B0 E0 - */
            ose_countItems(vm_s);
            int jj = ose_popInt32(vm_s);
            if(jj == 0)
            {
                for(i = 0; i < n + 3; i++)
                {
                    ose_drop(vm_s);
                }
                return;
            }
            else if(j > 0 && jj != j)
            {
                ose_rassert(0 && "arguments to map must be the same length", 1);
                break;
            }
            else
            {
                ose_pop(vm_s);  /* ENm1 ... B1 B0 E0 I0 - */
                ose_notrot(vm_s); /* ENm1 ... B1 I0 B0 E0 - */
                ose_push(vm_s); /* ENm1 ... B1 I0 B0 - */
                ose_notrot(vm_s); /* ENm1 ... B0 B1 I0 - */
                ose_push(vm_s); /* ENm1 ... B0 B1 - */
                ose_swap(vm_s); /* ENm1 ... B1 B0 - */
            }
        }
        /* B1 B0 - */
        /* if(!strcmp(ose_peekAddress(vm_c), "/!")) */
        /* { */
        /*     ose_drop(vm_c); */
        /*     ose_pushMessage(vm_c, "/!/map", 6, 0); */
        /* } */
        ose_copyElem(vm_s, vm_c);
        ose_drop(vm_s);
        ose_swap(vm_s);
        ose_copyElem(vm_s, vm_c);
        ose_swap(vm_c);
        ose_pushString(vm_c, "/!/apply");
        ose_pushString(vm_c, "/!/map");
        ose_push(vm_s);
        ose_unpackDrop(vm_s);
    }
    else
    {
        ose_countItems(vm_s);
        if(ose_popInt32(vm_s) > 0)
        {
            ose_swap(vm_s);
            ose_copyElem(vm_s, vm_c);
            ose_swap(vm_s);
            ose_pop(vm_s);
            ose_swap(vm_s);
            ose_copyElem(vm_s, vm_c);
            ose_drop(vm_s);
            ose_swap(vm_s);
            ose_pushString(vm_c, "/!/apply");
            ose_pushString(vm_c, "/!/map");
        }
        else
        {
            ;
        }
    }
}

void ose_builtin_return(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_d = OSEVM_DUMP(osevm);

    /* #define OSE_USE_OPTIMIZED_CODE */
#ifdef OSE_USE_OPTIMIZED_CODE
    char *dp = ose_getBundlePtr(vm_d);
    char *cp = ose_getBundlePtr(vm_c);
    char *sp = ose_getBundlePtr(vm_s);
    char *ep = ose_getBundlePtr(vm_e);
    char *ip = ose_getBundlePtr(vm_i);

    int32_t onm3, onm2, onm1, on, snm3, snm2, snm1, sn;
    extern void be4(ose_bundle bundle,
                    int32_t *onm3,
                    int32_t *snm3,
                    int32_t *onm2,
                    int32_t *snm2,
                    int32_t *onm1,
                    int32_t *snm1,
                    int32_t *on,
                    int32_t *sn);
    be4(vm_d, &onm3, &snm3, &onm2, &snm2, &onm1, &snm1, &on, &sn);
	
    ose_clear(vm_c);
    ose_incSize(vm_c, sn - OSE_BUNDLE_HEADER_LEN);
    memcpy(cp + OSE_BUNDLE_HEADER_LEN,
           dp + on + 4 + OSE_BUNDLE_HEADER_LEN,
           sn - OSE_BUNDLE_HEADER_LEN);

    ose_incSize(vm_s, snm1 - OSE_BUNDLE_HEADER_LEN);
    memmove(sp + OSE_BUNDLE_HEADER_LEN + (snm1 - OSE_BUNDLE_HEADER_LEN),
            sp + OSE_BUNDLE_HEADER_LEN,
            ose_readInt32(vm_s, -4) - OSE_BUNDLE_HEADER_LEN);
    memcpy(dp + onm1 + 4 + OSE_BUNDLE_HEADER_LEN,
           sp + OSE_BUNDLE_HEADER_LEN,
           snm1 - OSE_BUNDLE_HEADER_LEN);

    ose_clear(vm_e);
    ose_incSize(vm_e, snm2 - OSE_BUNDLE_HEADER_LEN);
    memcpy(ep + OSE_BUNDLE_HEADER_LEN,
           dp + onm2 + 4 + OSE_BUNDLE_HEADER_LEN,
           snm2 - OSE_BUNDLE_HEADER_LEN);

    ose_clear(vm_i);
    ose_incSize(vm_i, snm3 - OSE_BUNDLE_HEADER_LEN);
    memcpy(ip + OSE_BUNDLE_HEADER_LEN,
           dp + onm3 + 4 + OSE_BUNDLE_HEADER_LEN,
           snm3 - OSE_BUNDLE_HEADER_LEN);

    int32_t s = snm3 + snm2 + snm1 + sn + 16;
    memset(dp + onm3, 0, s);
    ose_decSize(vm_d, s);
#else
    /* restore control */
    ose_replaceBundle(vm_d, vm_c);

    /* put the env on the stack */
    ose_copyBundle(vm_e, vm_s);

    /* restore env */
    /*ose_copyBundle(vm_e, vm_s); */
    ose_replaceBundle(vm_d, vm_e);

    /* restore input */
    ose_replaceBundle(vm_d, vm_i);
#endif
}

void ose_builtin_version(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_pushString(vm_s, ose_version);
#ifdef OSE_DEBUG
    ose_pushString(vm_s, ose_debug);
#endif
    ose_pushString(vm_s, ose_date_compiled);
}

void ose_builtin_assignStackToEnv(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);

    const char * const str = ose_peekString(vm_s);
    if(!strncmp(str, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN)
       && ose_readInt32(vm_e, -4) == OSE_BUNDLE_HEADER_LEN)
    {
        /* if there's nothing in the env, and this is the empty
           string, rollMatch_impl will crash, because the string
           will match itself */
    }
    else
    {
        ose_pushString(vm_e, str);
        while(ose_rollMatch_impl(vm_e))
        {
            ose_drop(vm_e);
            ose_pushString(vm_e, ose_peekString(vm_s));
        }
        ose_drop(vm_e);
    }
    while(1)
    {
        int32_t n = ose_getBundleElemCount(vm_s);
        if(n == 1)
        {
            break;
        }
        ose_swap(vm_s);
        if(ose_peekType(vm_s) == OSETT_BUNDLE)
        {
            ose_elemToBlob(vm_s);
        }
        ose_swap(vm_s);
        ose_push(vm_s);
    }
    ose_moveStringToAddress(vm_s);
    ose_moveElem(vm_s, vm_e);
    ose_clear(vm_s);
}

void ose_builtin_lookupInEnv(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_e = OSEVM_ENV(osevm);

    const char * const address = ose_peekString(vm_s);
    int32_t mo = 0;
    {
        mo = ose_getFirstOffsetForMatch(vm_e, address);
    }
    if(mo >= OSE_BUNDLE_HEADER_LEN)
    {
        ose_drop(vm_s);
        ose_copyElemAtOffset(mo, vm_e, vm_s);
    }
    /* if it wasn't present in env, lookup in symtab */
    else
    {
        const ose_fn f = ose_symtab_lookup_fn(address);
        if(f)
        {
            ose_drop(vm_s);               
            ose_pushAlignedPtr(vm_s, (void *)f);
        }
        else
        {
            ;
        }
    }
}

void ose_builtin_funcall(ose_bundle osevm)
{
    OSEVM_LOOKUP(osevm);
    ose_builtin_apply(osevm);
}

void ose_builtin_quote(ose_bundle osevm)
{
    ;
}

void ose_builtin_copyContextBundle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    const char * const str = ose_peekString(vm_s);
    ose_bundle src = ose_enter(osevm, str);
    ose_bundle dest = vm_s;
    ose_drop(vm_s);
    ose_copyBundle(src, dest);
}

void ose_builtin_appendToContextBundle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    const char * const str = ose_peekString(vm_s);
    ose_bundle src = vm_s;
    ose_bundle dest = ose_enter(osevm, str);
    ose_drop(vm_s);
    ose_appendBundle(src, dest);	
}

void ose_builtin_replaceContextBundle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    const char * const str = ose_peekString(vm_s);
    ose_bundle src = vm_s;
    ose_bundle dest = ose_enter(osevm, str);
    ose_drop(vm_s);
    ose_replaceBundle(src, dest);
}

void ose_builtin_moveElemToContextBundle(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    const char * const str = ose_peekString(vm_s);
    ose_bundle src = vm_s;
    ose_bundle dest = ose_enter(osevm, str);
    ose_drop(vm_s);
    ose_moveElem(src, dest);
}

void ose_builtin_toInt32(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    {
        const char t = ose_peekMessageArgType(vm_s);
        switch(t)
        {
        case OSETT_INT32:
        {
            ;
        }
        break;
        case OSETT_FLOAT:
        {
            const float f = ose_popFloat(vm_s);
            ose_pushInt32(vm_s, (int32_t)f);
        }
        break;
        case OSETT_STRING:
        {
            const char * const s = ose_peekString(vm_s);
            int32_t l;
            if(*s == '/')
            {
                l = strtol(s + 1, NULL, 10);
            }
            else
            {
                l = strtol(s, NULL, 10);
            }
            ose_drop(vm_s);
            ose_pushInt32(vm_s, l);
        }
        break;
        case OSETT_BLOB:
        {
            const char * const b = ose_peekBlob(vm_s);
            if(ose_ntohl(*((int32_t *)b)) == 4)
            {
                int32_t i = ose_ntohl(*((int32_t *)(b + 4)));
                ose_drop(vm_s);
                ose_pushInt32(vm_s, i);
            }
            else
            {
                /* popControlToStack(vm_c, vm_s); */
            }
        }
        break;
        default: ;
            /* popControlToStack(vm_c, vm_s); */
        }
    }
}

/**
 * /f
 */
void ose_builtin_toFloat(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    {
        const char t = ose_peekMessageArgType(vm_s);
        switch(t)
        {
        case OSETT_INT32:
        {
            const int32_t i = ose_popInt32(vm_s);
            ose_pushFloat(vm_s, (float)i);
        }
        break;
        case OSETT_FLOAT:
        {
            ;
        }
        break;
        case OSETT_STRING:
        {
            const char * const s = ose_peekString(vm_s);
            float f;
            if(*s == '/')
            {
                f = strtof(s + 1, NULL);
            }
            else
            {
                f = strtof(s, NULL);
            }
            ose_drop(vm_s);
            ose_pushFloat(vm_s, f);
        }
        break;
        case OSETT_BLOB:
        {
            const char * const b = ose_peekBlob(vm_s);
            if(ose_ntohl(*((int32_t *)b)) == 4)
            {
                int32_t i = ose_ntohl(*((int32_t *)(b + 4)));
                float f = *((float *)&i);
                ose_drop(vm_s);
                ose_pushFloat(vm_s, f);
            }
            else
            {
                /* popControlToStack(vm_c, vm_s); */
            }
        }
        break;
        default: ;
            /* popControlToStack(vm_c, vm_s); */
        }
    }
}

/**
 * /s
 */
void ose_builtin_toString(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    {
        const char t = ose_peekMessageArgType(vm_s);
        switch(t){
        case OSETT_INT32:
        {
            const int32_t i = ose_popInt32(vm_s);
            const int32_t n = snprintf(NULL, 0, "%d", i);
            ose_pushBlob(vm_s, n - 3 >= 0 ? n - 3 : 0, NULL);
            char *p = ose_peekBlob(vm_s);
            snprintf(p, n + 1, "%d", i);
            p--;
            while(*p != OSETT_BLOB)
            {
                p--;
            }
            *p = OSETT_STRING;
        }
        break;
        case OSETT_FLOAT:
        {
            const float f = ose_popFloat(vm_s);
            const int32_t n = snprintf(NULL, 0, "%f", f);
            ose_pushBlob(vm_s, n - 3 >= 0 ? n - 3 : 0, NULL);
            char *p = ose_peekBlob(vm_s);
            snprintf(p, n + 1, "%f", f);
            p--;
            while(*p != OSETT_BLOB)
            {
                p--;
            }
            *p = OSETT_STRING;
        }
        break;
        case OSETT_STRING:
        {
            ;
        }
        break;
        case OSETT_BLOB:
        {
            ose_pushInt32(vm_s, OSETT_STRING);
            ose_blobToType(vm_s);
        }
        break;
        default: ;
            /* popControlToStack(vm_c, vm_s); */
        }
    }
}

/**
 * /b
 */
void ose_builtin_toBlob(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_itemToBlob(vm_s);
}

void ose_builtin_appendByte(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);

#define SLIP_END 0300
#define SLIP_ESC 0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

    const char * const str = ose_peekString(vm_s);
    unsigned char c = 0;
    c = (unsigned char)strtol(str + 1, NULL, 10);
    const int32_t n = ose_getBundleElemCount(vm_s);
    if(n == 0)
    {
        ose_pushMessage(vm_s,
                        OSE_ADDRESS_ANONVAL,
                        OSE_ADDRESS_ANONVAL_LEN,
                        2,
                        OSETT_BLOB, 0, NULL,
                        OSETT_INT32, 1);
    }
    else
    {
        const char elemtype = ose_peekType(vm_s);
        if(elemtype == OSETT_BUNDLE)
        {
            ose_pushMessage(vm_s,
                            OSE_ADDRESS_ANONVAL,
                            OSE_ADDRESS_ANONVAL_LEN,
                            2,
                            OSETT_BLOB, 0, NULL,
                            OSETT_INT32, 1);
        }
        else if(elemtype == OSETT_MESSAGE)
        {
            if(ose_peekMessageArgType(vm_s) == OSETT_INT32)
            {
                ose_pop(vm_s);
                int32_t state = ose_popInt32(vm_s);
                if(ose_peekMessageArgType(vm_s)
                   == OSETT_BLOB)
                {
                    switch(state)
                    {
                    case 0:
                        ose_pushInt32(vm_s, 1);
                        ose_push(vm_s);
                        break;
                    case 1:
                        switch(c)
                        {
                        case SLIP_END:
                            /* done */
                            break;
                        case SLIP_ESC:
                            ose_pushInt32(vm_s, 2);
                            ose_push(vm_s);
                            break;
                        default:
                            ose_pushBlob(vm_s,
                                         1,
                                         (char *)&c);
                            ose_push(vm_s);
                            ose_concatenateBlobs(vm_s);
                            ose_pushInt32(vm_s, 1);
                            ose_push(vm_s);
                            break;
                        }
                        break;
                    case 2:
                        switch(c)
                        {
                        case SLIP_ESC_END:
                        {
                            const char cc = SLIP_END;	
                            ose_pushBlob(vm_s,
                                         1,
                                         &cc);
                            ose_push(vm_s);
                            ose_concatenateBlobs(vm_s);
                            ose_pushInt32(vm_s, 1);
                            ose_push(vm_s);
                        }
                        break;
                        case SLIP_ESC_ESC:
                        {
                            const char cc = SLIP_ESC;	
                            ose_pushBlob(vm_s,
                                         1,
                                         &cc);
                            ose_push(vm_s);
                            ose_concatenateBlobs(vm_s);
                            ose_pushInt32(vm_s, 1);
                            ose_push(vm_s);
                        }
                        break;
                        default:
                            ose_assert(0
                                       && "SLIP ESC not followed by ESC_END or ESC_ESC.");
                        }
                        break;
                    default:
                        ose_pushInt32(vm_s, state);
                        ose_push(vm_s);
                        ose_pushMessage(vm_s,
                                        OSE_ADDRESS_ANONVAL,
                                        OSE_ADDRESS_ANONVAL_LEN,
                                        2,
                                        OSETT_BLOB, 0, NULL,
                                        OSETT_INT32, 1);
                        break;
                    }
                }
                else
                {
                    ose_pushInt32(vm_s, state);
                    ose_push(vm_s);
                    ose_pushMessage(vm_s,
                                    OSE_ADDRESS_ANONVAL,
                                    OSE_ADDRESS_ANONVAL_LEN,
                                    2,
                                    OSETT_BLOB, 0, NULL,
                                    OSETT_INT32, 1);
                }
            }
            else
            {
                ose_pushMessage(vm_s,
                                OSE_ADDRESS_ANONVAL,
                                OSE_ADDRESS_ANONVAL_LEN,
                                2,
                                OSETT_BLOB, 0, NULL,
                                OSETT_INT32, 1);
            }
        }
        else
        {
            ose_assert(0 && "found something other than "
                       "a bundle or message");
        }
    }
}
