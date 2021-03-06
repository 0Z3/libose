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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_symtab.h"
#include "ose_builtins.h"
#include "ose_vm.h"
#include "ose_errno.h"

#if defined(OSE_ENDIAN)

#if OSE_ENDIAN == OSE_LITTLE_ENDIAN

#define OSEVM_INTCMP_MASK_1     0x000000ff
#define OSEVM_INTCMP_MASK_2     0x0000ffff
#define OSEVM_INTCMP_MASK_3     0x00ffffff

#define OSEVM_TOK_AT            0x002f402f
#define OSEVM_TOK_QUOTE         0x002f272f
#define OSEVM_TOK_BANG          0x002f212f
#define OSEVM_TOK_DOLLAR        0x002f242f
#define OSEVM_TOK_GT            0x002f3e2f
#define OSEVM_TOK_LTLT          0x2f3c3c2f
#define OSEVM_TOK_LT            0x002f3c2f
#define OSEVM_TOK_DASH          0x002f2d2f
#define OSEVM_TOK_DOT           0x002f2e2f
#define OSEVM_TOK_COLON         0x002f3a2f
#define OSEVM_TOK_SCOLON        0x002f3b2f
#define OSEVM_TOK_PIPE          0x002f7c2f
#define OSEVM_TOK_OPAR          0x002f282f
#define OSEVM_TOK_CPAR          0x002f292f
#define OSEVM_TOK_i             0x002f692f
#define OSEVM_TOK_f             0x002f662f
#define OSEVM_TOK_s             0x002f732f
#define OSEVM_TOK_b             0x002f622f
#define OSEVM_TOK_AMP           0x002f262f
#define OSEVM_TOK_HASH          0x002f232f

#define route_init(address, varname)            \
    struct { int32_t i2, i3, i4; } varname;     \
    do                                          \
    {                                           \
        varname.i4 = *((int32_t *)address);     \
        varname.i3 = varname.i4 & 0x00ffffff;   \
        varname.i2 = varname.i3 & 0x0000ffff;   \
    } while(0);

#elif OSE_ENDIAN == OSE_BIG_ENDIAN

#define OSEVM_INTCMP_MASK_1     0xff000000
#define OSEVM_INTCMP_MASK_2     0xffff0000
#define OSEVM_INTCMP_MASK_3     0xffffff00

#define OSEVM_TOK_AT            0x2f402f00
#define OSEVM_TOK_QUOTE         0x2f272f00
#define OSEVM_TOK_BANG          0x2f212f00
#define OSEVM_TOK_DOLLAR        0x2f242f00
#define OSEVM_TOK_GT            0x2f3e2f00
#define OSEVM_TOK_LTLT          0x2f3c3c2f
#define OSEVM_TOK_LT            0x2f3c2f00
#define OSEVM_TOK_DASH          0x2f2d2f00
#define OSEVM_TOK_DOT           0x2f2e2f00
#define OSEVM_TOK_COLON         0x2f3a2f00
#define OSEVM_TOK_SCOLON        0x2f3b2f00
#define OSEVM_TOK_PIPE          0x2f7c2f00
#define OSEVM_TOK_OPAR          0x2f282f00
#define OSEVM_TOK_CPAR          0x2f292f00
#define OSEVM_TOK_i             0x2f692f00
#define OSEVM_TOK_f             0x2f662f00
#define OSEVM_TOK_s             0x2f732f00
#define OSEVM_TOK_b             0x2f622f00
#define OSEVM_TOK_AMP           0x2f262f00
#define OSEVM_TOK_HASH          0x2f232f00

#define route_init(address, varname)            \
    struct { int32_t i2, i3, i4; } varname;     \
    do                                          \
    {                                           \
        varname.i4 = *((int32_t *)address);     \
        varname.i3 = varname.i4 & 0xffffff00;   \
        varname.i2 = varname.i3 & 0xffff0000;   \
    } while(0);
#endif

#define route_pfx_2(var, sym)                   \
    ((sym) == (var.i4))
#define route_pfx_1(var, sym)                   \
    ((sym & OSEVM_INTCMP_MASK_3) == (var.i3))
#define route_pfx(var, sym, n)                  \
    (route_pfx_##n((var), (sym)))

#define route_mthd_2(var, sym)                  \
    ((sym & OSEVM_INTCMP_MASK_3) == (var.i4))
#define route_mthd_1(var, sym)                  \
    ((sym & OSEVM_INTCMP_MASK_2) == (var.i4))
#define route_mthd(var, sym, n)                 \
    (route_mthd_##n((var), (sym)))

#else

#define route_init(address, varname)            \
    const char * const varname = address

#define route_pfx(var, sym, n)                  \
    (strncmp(var, sym, n + 2) == 0)
#define route_mthd(var, sym, n)                 \
    (strncmp(var, sym, n + 1) == 0)

#define OSEVM_TOK_AT        "/@/\0"
#define OSEVM_TOK_QUOTE     "/'/\0"
#define OSEVM_TOK_BANG      "/!/\0"
#define OSEVM_TOK_DOLLAR    "/$/\0"
#define OSEVM_TOK_GT        "/>/\0"
#define OSEVM_TOK_LTLT      "/<</"
#define OSEVM_TOK_LT        "/</\0"
#define OSEVM_TOK_DASH      "/-/\0"
#define OSEVM_TOK_DOT       "/./\0"
#define OSEVM_TOK_COLON     "/:/\0"
#define OSEVM_TOK_SCOLON    "/;/\0"
#define OSEVM_TOK_PIPE      "/|/\0"
#define OSEVM_TOK_OPAR      "/(/\0"
#define OSEVM_TOK_CPAR      "/)/\0"
#define OSEVM_TOK_i         "/i/\0"
#define OSEVM_TOK_f         "/f/\0"
#define OSEVM_TOK_s         "/s/\0"
#define OSEVM_TOK_b         "/b/\0"
#define OSEVM_TOK_AMP       "/&/\0"
#define OSEVM_TOK_HASH      "/#/\0"

#endif

#define route(var, sym, n)                              \
    (route_mthd(var, sym, n) || route_pfx(var, sym, n))

/* static void convertKnownStringAddressToAddress(ose_bundle vm_c); */

/* static void popControlToStack(ose_bundle vm_c, ose_bundle vm_s) */
/* { */
/*     ose_copyElem(vm_c, vm_s); */
/* } */

void osevm_popInputToControl(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_moveElem(vm_i, vm_c);
}

/**
 * /.
 */
void osevm_exec(ose_bundle osevm){}
/* void osevm_exec(ose_bundle osevm, char *address) */
/* { */
/*     ose_bundle vm_s = OSEVM_STACK(osevm); */
/*     ose_bundle vm_c = OSEVM_CONTROL(osevm); */
/*     const int32_t addresslen = strlen(address); */
/*     if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE) */
/*     { */
/*         popControlToStack(vm_c, vm_s); */
/*         return; */
/*     } */
/*     if(address[2] == '/') */
/*     { */
/*         int i = 2; */
/*         char sep = 0; */
/*         char char_before_sep = 0; */
/*         while(i < addresslen) */
/*         { */
/*             if(address[i] == '.' */
/*                || address[i] == ':' */
/*                || address[i] == ';' */
/*                || address[i] == '|') */
/*             { */
/*                 sep = address[i]; */
/*                 char_before_sep = address[i - 1]; */
/*                 address[i - 1] = 0; */
/*                 break; */
/*             } */
/*             i++; */
/*         } */
/*         if(address[3] == '.' */
/*            || address[3] == ':' */
/*            || address[3] == ';' */
/*            || address[3] == '|') */
/*         { */
/*             ose_pushBundle(vm_s); */
/*             ose_pushString(vm_s, address + 2); */
/*             ose_push(vm_s); */
/*             ose_pushMessage(vm_s, "/!/exec", 7, 0); */
/*             ose_moveElem(vm_s, vm_c); */
/*             ose_swap(vm_c); */
/*             ose_moveElem(vm_s, vm_c); */
/*             ose_swap(vm_c); */
/*         } */
/*         else */
/*         { */
/*             if(char_before_sep) */
/*             { */
/*                 ose_pushBundle(vm_s); */
/*                 ose_pushString(vm_s, address + 2); */
/*                 ose_push(vm_s); */
/*                 address[i - 1] = char_before_sep; */
/*                 ose_pushString(vm_s, address + (i - 1)); */
/*                 ose_push(vm_s); */
/*             } */
/*             else */
/*             { */
/*                 ose_pushBundle(vm_s); */
/*                 ose_pushString(vm_s, address + 2); */
/*                 ose_push(vm_s); */
/*             } */
/*         } */
/*         ose_pushMessage(vm_c, "/!/exec", 7, 0); */
/*         ose_swap(vm_c); */
/*     } */
/*     else */
/*     { */
/*         ose_pushMessage(vm_c, "/!/exec", 7, 0); */
/*         ose_swap(vm_c); */
/*     }     */
/* } */

/**
 * /|
 */
void osevm_execInCurrentContext(ose_bundle osevm){}
/* void osevm_execInCurrentContext(ose_bundle osevm, char *address) */
/* { */
/*     ose_bundle vm_s = OSEVM_STACK(osevm); */
/*     ose_bundle vm_c = OSEVM_CONTROL(osevm); */
/*     const int32_t addresslen = strlen(address); */
/*     if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE) */
/*     { */
/*         popControlToStack(vm_c, vm_s); */
/*         return; */
/*     } */
/*     if(address[2] == '/') */
/*     { */
/*         int i = 2; */
/*         char sep = 0; */
/*         char char_before_sep = 0; */
/*         while(i < addresslen) */
/*         { */
/*             if(address[i] == '.' */
/*                || address[i] == ':' */
/*                || address[i] == ';' */
/*                || address[i] == '|') */
/*             { */
/*                 sep = address[i]; */
/*                 char_before_sep = address[i - 1]; */
/*                 address[i - 1] = 0; */
/*                 break; */
/*             } */
/*             i++; */
/*         } */
/*         if(address[3] == '.' */
/*            || address[3] == ':' */
/*            || address[3] == ';' */
/*            || address[3] == '|') */
/*         { */
/*             address[i - 1] = char_before_sep; */
/*             ose_pushMessage(vm_s, */
/*                             address + 2, */
/*                             strlen(address + 2), */
/*                             0); */
/*             ose_moveElem(vm_s, vm_c); */
/*             ose_swap(vm_c); */
/*             ose_moveElem(vm_s, vm_c); */
/*             convertKnownStringAddressToAddress(vm_c); */
/*             ose_swap(vm_c); */
/*         } */
/*         else */
/*         { */
/*             if(char_before_sep) */
/*             { */
/*                 ose_pushMessage(vm_s, */
/*                                 address + 2, */
/*                                 strlen(address + 2), */
/*                                 0); */
/*                 address[i - 1] = char_before_sep; */
/*                 ose_pushMessage(vm_s, */
/*                                 address + (i - 1), */
/*                                 strlen(address + (i - 1)), */
/*                                 0); */
/*                 ose_moveElem(vm_s, vm_c); */
/*                 ose_swap(vm_c); */
/*                 ose_moveElem(vm_s, vm_c); */
/*                 ose_swap(vm_c); */
/*             } */
/*             else */
/*             { */
/*                 ose_pushMessage(vm_c, */
/*                                 address + 2, */
/*                                 strlen(address + 2), */
/*                                 0); */
/*                 ose_swap(vm_c); */
/*             } */
/*         } */
/*     } */
/*     else */
/*     { */
/*         ose_moveElem(vm_s, vm_c); */
/*         convertKnownStringAddressToAddress(vm_c); */
/*         ose_swap(vm_c); */
/*     }     */
/* } */

/**
 * /:
 */
void osevm_apply(ose_bundle osevm){}
/* void osevm_apply(ose_bundle osevm, char *address) */
/* { */
/*     ose_bundle vm_s = OSEVM_STACK(osevm); */
/*     ose_bundle vm_c = OSEVM_CONTROL(osevm); */
/*     const int32_t addresslen = strlen(address); */
/*     if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE) */
/*     { */
/*         popControlToStack(vm_c, vm_s); */
/*         return; */
/*     } */
/*     if(address[2] == '/') */
/*     { */
/*         int i = 2; */
/*         char sep = 0; */
/*         char char_before_sep = 0; */
/*         while(i < addresslen) */
/*         { */
/*             if(address[i] == '.' */
/*                || address[i] == ':' */
/*                || address[i] == ';' */
/*                || address[i] == '|') */
/*             { */
/*                 sep = address[i]; */
/*                 char_before_sep = address[i - 1]; */
/*                 address[i - 1] = 0; */
/*                 break; */
/*             } */
/*             i++; */
/*         } */
/*         if(address[3] == '.' */
/*            || address[3] == ':' */
/*            || address[3] == ';' */
/*            || address[3] == '|') */
/*         { */
/*             address[i - 1] = char_before_sep; */
/*             ose_pushBundle(vm_s); */
/*             ose_pushString(vm_s, address + 2); */
/*             ose_push(vm_s); */
/*             ose_pushMessage(vm_s, "/!/apply", 8, 0); */
/*             ose_moveElem(vm_s, vm_c); */
/*             ose_swap(vm_c); */
/*             ose_moveElem(vm_s, vm_c); */
/*             ose_swap(vm_c); */
/*         } */
/*         else */
/*         { */
/*             if(char_before_sep) */
/*             { */
/*                 ose_pushBundle(vm_s); */
/*                 ose_pushString(vm_s, address + 2); */
/*                 ose_push(vm_s); */
/*                 address[i - 1] = char_before_sep; */
/*                 ose_pushString(vm_s, address + (i - 1)); */
/*                 ose_push(vm_s); */
/*             } */
/*             else */
/*             { */
/*                 ose_pushBundle(vm_s); */
/*                 ose_pushString(vm_s, address + 2); */
/*                 ose_push(vm_s); */
/*             } */
/*         } */
/*         ose_pushMessage(vm_c, "/!/apply", 8, 0); */
/*         ose_swap(vm_c); */
/*     } */
/*     else */
/*     { */
/*         ose_pushMessage(vm_c, "/!/apply", 8, 0); */
/*         ose_swap(vm_c); */
/*     } */
/* } */

/**
 * /;
 */
void osevm_return(ose_bundle osevm){}
/* void osevm_return(ose_bundle osevm, char *address) */
/* { */
/*     ose_bundle vm_i = OSEVM_INPUT(osevm); */
/*     ose_bundle vm_s = OSEVM_STACK(osevm); */
/*     ose_bundle vm_c = OSEVM_CONTROL(osevm); */
/*     const int32_t addresslen = strlen(address); */
/*     if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE) */
/*     { */
/*         popControlToStack(vm_c, vm_s); */
/*         return; */
/*     } */
/*     if(address[2] == '/') */
/*     { */
/*         int i = 2; */
/*         char sep = 0; */
/*         char char_before_sep = 0; */
/*         while(i < addresslen) */
/*         { */
/*             if(address[i] == '.' */
/*                || address[i] == ':' */
/*                || address[i] == ';' */
/*                || address[i] == '|') */
/*             { */
/*                 sep = address[i]; */
/*                 char_before_sep = address[i - 1]; */
/*                 address[i - 1] = 0; */
/*                 break; */
/*             } */
/*             i++; */
/*         } */
/*         if(address[3] == '.' */
/*            || address[3] == ':' */
/*            || address[3] == ';' */
/*            || address[3] == '|') */
/*         { */
/*             /\* /;/./~~~ or /;/:/~~~ means return and execute *\/ */
/*             /\* address + 2 in the new context *\/ */
                
/*             address[i - 1] = char_before_sep; */
/*             ose_pushString(vm_s, address + (i - 1)); */
/*             ose_builtin_return(osevm); */
/*             ose_moveElem(vm_s, vm_i); */
/*             ose_pushMessage(vm_c, */
/*                             OSE_ADDRESS_ANONVAL, */
/*                             OSE_ADDRESS_ANONVAL_LEN, */
/*                             0); */
/*         } */
/*         else */
/*         { */
/*             /\* /;/~~~ means execute address + 2 in this *\/ */
/*             /\* context and then return *\/ */
                
/*             if(char_before_sep) */
/*             { */
/*                 /\* /;/~~~/./~~~ or /;/~~~/:/~~~ *\/ */
/*                 /\* means execute address + 2 in this *\/ */
/*                 /\* context, then return and continue *\/ */

/*                 address[i - 1] = char_before_sep; */
/*                 for(int j = 0; j < i - 3; j++){ */
/*                     address[j] = address[j + 2]; */
/*                 } */
/*                 address[i - 3] = 0; */
/*                 address[i - 2] = ';'; */
/*                 ose_pushMessage(vm_s, */
/*                                 address, */
/*                                 strlen(address), */
/*                                 0); */
/*                 address[i - 3] = '/'; */
/*                 ose_pushMessage(vm_s, */
/*                                 address + (i - 3), */
/*                                 strlen(address + (i - 3)), */
/*                                 0); */
/*                 ose_moveElem(vm_s, vm_c); */
/*                 ose_swap(vm_c); */
/*                 ose_moveElem(vm_s, vm_c); */
/*                 ose_swap(vm_c); */
/*             } */
/*             else */
/*             { */
/*                 /\* /;/~~~ means execute address + 2 *\/ */
/*                 /\* in this context and then return *\/ */

/*                 for(int j = 0; j < i - 2; j++) */
/*                 { */
/*                     address[j] = address[j + 2]; */
/*                 } */
/*                 address[i - 2] = 0; */
/*                 address[i - 1] = ';'; */
/*                 ose_pushMessage(vm_s, */
/*                                 address, */
/*                                 strlen(address), */
/*                                 0); */
/*                 address[i - 2] = '/'; */
/*                 ose_pushMessage(vm_s, */
/*                                 address + (i - 2), */
/*                                 strlen(address + (i - 2)), */
/*                                 0); */
/*                 ose_moveElem(vm_s, vm_c); */
/*                 ose_swap(vm_c); */
/*                 ose_moveElem(vm_s, vm_c); */
/*                 ose_swap(vm_c); */
/*             } */
/*         } */
/*     } */
/*     else */
/*     { */
/*         ose_builtin_return(osevm); */
/*         ose_pushMessage(vm_c, */
/*                         OSE_ADDRESS_ANONVAL, */
/*                         OSE_ADDRESS_ANONVAL_LEN, */
/*                         0); */
/*     } */
/* } */

/**
 * /(
 */
void osevm_defun(ose_bundle osevm){}
/* void osevm_defun(ose_bundle osevm, char *address) */
/* { */
/*     ose_bundle vm_s = OSEVM_STACK(osevm); */
/*     ose_bundle vm_d = OSEVM_DUMP(osevm); */
    
/*     int32_t flags = OSEVM_GET_FLAGS(osevm); */
/*     OSEVM_SET_FLAGS(osevm, flags | OSEVM_FLAG_COMPILE); */
/*     /\* ose_copyBundle(vm_i, vm_d); *\/ */
/*     /\* ose_copyBundle(vm_e, vm_d); *\/ */
/*     ose_copyBundle(vm_s, vm_d); */
/*     ose_clear(vm_s); */
/*     /\* ose_copyBundle(vm_c, vm_d); *\/ */
    
/*     if(address[2] == '/') */
/*     { */
/*         ose_pushMessage(vm_s, */
/*                         address + 2, */
/*                         strlen(address + 2), */
/*                         0); */
/*     } */
/*     else */
/*     { */
/*         ose_pushMessage(vm_s, */
/*                         OSE_ADDRESS_ANONVAL, */
/*                         OSE_ADDRESS_ANONVAL_LEN, */
/*                         0); */
/*     } */
/* } */

/**
 * /)
 */
void osevm_endDefun(ose_bundle osevm){}
/* void osevm_endDefun(ose_bundle osevm, char *address) */
/* { */
/*     ose_bundle vm_s = OSEVM_STACK(osevm); */
/*     ose_bundle vm_d = OSEVM_DUMP(osevm); */
/*     int32_t flags = OSEVM_GET_FLAGS(osevm); */
/*     OSEVM_SET_FLAGS(osevm, flags ^ OSEVM_FLAG_COMPILE); */
/*     ose_rollBottom(vm_s); */
/*     ose_bundleAll(vm_s); */
/*     ose_pop(vm_s); */
/*     ose_swap(vm_s); */
/*     ose_push(vm_s); */
/*     /\* ose_builtin_return(osevm); *\/ */
/*     ose_moveElem(vm_d, vm_s); */
/*     ose_unpackDrop(vm_s); */
/*     ose_rollBottom(vm_s); */
/* } */

void osevm_respondToString(ose_bundle osevm)
{
}

#ifdef OSEVM_HAVE_SIZES
ose_bundle osevm_init(ose_bundle bundle)
#else
    ose_bundle osevm_init(ose_bundle bundle,
                          int32_t input_size,
                          int32_t stack_size,
                          int32_t env_size,
                          int32_t control_size,
                          int32_t dump_size,
                          int32_t output_size)
#endif
{
#ifdef OSEVM_HAVE_SIZES
    const int32_t input_size = OSEVM_INPUT_SIZE
        + OSE_CONTEXT_MESSAGE_OVERHEAD;
    const int32_t stack_size = OSEVM_STACK_SIZE
        + OSE_CONTEXT_MESSAGE_OVERHEAD;
    const int32_t env_size = OSEVM_ENV_SIZE
        + OSE_CONTEXT_MESSAGE_OVERHEAD;
    const int32_t control_size = OSEVM_CONTROL_SIZE
        + OSE_CONTEXT_MESSAGE_OVERHEAD;
    const int32_t dump_size = OSEVM_DUMP_SIZE
        + OSE_CONTEXT_MESSAGE_OVERHEAD;
    const int32_t output_size = OSEVM_OUTPUT_SIZE
        + OSE_CONTEXT_MESSAGE_OVERHEAD;
#endif
    /* cache */
    ose_pushContextMessage(bundle,
                           OSEVM_CACHE_MSG_SIZE,
                           OSEVM_ADDR_CACHE);

    /* input from the world */
    ose_pushContextMessage(bundle,
                           input_size,
                           OSEVM_ADDR_INPUT);
    /* stack */
    ose_pushContextMessage(bundle,
                           stack_size,
                           OSEVM_ADDR_STACK);
    /* environment */
    ose_pushContextMessage(bundle,
                           env_size,
                           OSEVM_ADDR_ENV);
    /* control */
    ose_pushContextMessage(bundle,
                           control_size,
                           OSEVM_ADDR_CONTROL);
    /* dump */
    ose_pushContextMessage(bundle,
                           dump_size,
                           OSEVM_ADDR_DUMP);
    /* output to the world */
    ose_pushContextMessage(bundle,
                           output_size,
                           OSEVM_ADDR_OUTPUT);

    ose_bundle vm_cache = ose_enter(bundle, OSEVM_ADDR_CACHE);
    ose_bundle vm_i = ose_enter(bundle, OSEVM_ADDR_INPUT);
    ose_bundle vm_s = ose_enter(bundle, OSEVM_ADDR_STACK);
    ose_bundle vm_e = ose_enter(bundle, OSEVM_ADDR_ENV);
    ose_bundle vm_c = ose_enter(bundle, OSEVM_ADDR_CONTROL);
    ose_bundle vm_d = ose_enter(bundle, OSEVM_ADDR_DUMP);
    ose_bundle vm_o = ose_enter(bundle, OSEVM_ADDR_OUTPUT);
    ose_pushMessage(vm_cache,
                    OSE_ADDRESS_ANONVAL,
                    OSE_ADDRESS_ANONVAL_LEN,
                    30,
                    OSETT_INT32,
                    7,
                    OSETT_INT32,
                    0,
                    OSETT_INT32,
                    ose_getBundlePtr(vm_i) - ose_getBundlePtr(bundle),
                    OSETT_INT32,
                    ose_getBundlePtr(vm_s) - ose_getBundlePtr(bundle),
                    OSETT_INT32,
                    ose_getBundlePtr(vm_e) - ose_getBundlePtr(bundle),
                    OSETT_INT32,
                    ose_getBundlePtr(vm_c) - ose_getBundlePtr(bundle),
                    OSETT_INT32,
                    ose_getBundlePtr(vm_d) - ose_getBundlePtr(bundle),
                    OSETT_INT32,
                    ose_getBundlePtr(vm_o) - ose_getBundlePtr(bundle),
                    OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
                    OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
                    OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
                    OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
                    OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
                    OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
                    OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
                    OSETT_INT32, 0);
    return bundle;
}

static void applyControl(ose_bundle osevm, char *address)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);

    if(ose_peekType(vm_c) == OSETT_MESSAGE)
    {
        char t = ose_peekMessageArgType(vm_c);
        if(!ose_isStringType(t))
        {
            ose_copyElem(vm_c, vm_s);
            return;
        }
    }
    else
    {
        ose_copyElem(vm_c, vm_s);
        return;
    }

    const char * const str = ose_peekString(vm_c);
    route_init(str, a);

    if(route_pfx(a, OSEVM_TOK_AT, 1))
    {
        if(str[3])
        {
            ose_pushString(vm_s, str + 2);
        }
        else
        {
            ose_pushString(vm_s, OSE_ADDRESS_ANONVAL);
        }
        OSEVM_ASSIGN(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_QUOTE, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_QUOTE(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_BANG, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_FUNCALL(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_DOLLAR, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_LOOKUP(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_GT, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_COPYCONTEXTBUNDLE(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_LTLT, 2))
    {
        ose_pushString(vm_s, str + 3);
        OSEVM_APPENDTOCONTEXTBUNDLE(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_LT, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_REPLACECONTEXTBUNDLE(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_DASH, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_MOVEELEMTOCONTEXTBUNDLE(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_i, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_TOINT32(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_f, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_TOFLOAT(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_s, 1))
    {
        ose_pushString(vm_s, str + 3);
        OSEVM_TOSTRING(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_b, 1))
    {
        ose_pushString(vm_s, str + 2);
        OSEVM_TOBLOB(osevm);
    }
    else if(route_pfx(a, OSEVM_TOK_HASH, 1))
    {
        ;
    }
    else
    {
        ose_pushString(vm_s, str);
        OSEVM_RESPONDTOSTRING(osevm);
    }
}

static void popAllControl(ose_bundle osevm)
{
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    const char * const address = ose_peekAddress(vm_c);
    if(!strncmp(address, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN))
    {
        return;
    }
    else if(strncmp(address,
                    OSE_ADDRESS_ANONVAL,
                    OSE_ADDRESS_ANONVAL_SIZE))
    {
        ose_pushString(vm_c, OSE_ADDRESS_ANONVAL);
        ose_push(vm_c);
        ose_swapStringToAddress(vm_c);
    }
    ose_countItems(vm_c);
    int32_t n = ose_popInt32(vm_c);
    for(int i = 0; i < n; i++)
    {
        ose_pop(vm_c);
        ose_swap(vm_c);
    }
    ose_drop(vm_c);
}

void osevm_preInput(ose_bundle osevm)
{
}

void osevm_postInput(ose_bundle osevm)
{
}

void osevm_postControl(ose_bundle osevm)
{
}

char osevm_step(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_d = OSEVM_DUMP(osevm);
    if(!ose_bundleIsEmpty(vm_c))
    {
        applyControl(osevm, ose_peekAddress(vm_c));
        if(ose_bundleIsEmpty(vm_c))
        {
            OSEVM_POSTCONTROL(osevm);
        }
    }
    else if(!ose_bundleIsEmpty(vm_i))
    {
        OSEVM_POPINPUTTOCONTROL(osevm);
        if(!ose_bundleIsEmpty(vm_c))
        {
            popAllControl(osevm);
        }
    }
    else if(!ose_bundleIsEmpty(vm_d)
            && !(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE))
    {
        ose_builtin_return(osevm);
    }
    if(!ose_bundleIsEmpty(vm_i)
       || !ose_bundleIsEmpty(vm_c))
    {
        return OSETT_TRUE;
    }
    else if(!ose_bundleIsEmpty(vm_d))
    {
        if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE)
        {
            return OSETT_FALSE;
        }
        else
        {
            return OSETT_TRUE;
        }
    }
    else
    {
        return OSETT_FALSE;
    }
}

void osevm_run(ose_bundle osevm)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_bundle vm_s = OSEVM_STACK(osevm);
    ose_bundle vm_c = OSEVM_CONTROL(osevm);
    ose_bundle vm_d = OSEVM_DUMP(osevm);
    int32_t n = ose_getBundleElemCount(vm_d);
    OSEVM_PREINPUT(osevm);
    while(1)
    {
        while(1)
        {
            if(ose_bundleIsEmpty(vm_c))
            {
                if(ose_bundleIsEmpty(vm_i))
                {
                    break;
                }
                OSEVM_POPINPUTTOCONTROL(osevm);
                if(ose_bundleIsEmpty(vm_c))
                {
                    continue;
                }
                popAllControl(osevm);
            }
            while(1)
            {
                if(ose_bundleIsEmpty(vm_c))
                {
                    break;
                }
                applyControl(osevm, ose_peekAddress(vm_c));
                /* check status and drop into */
                /* debugger if necessary */
                enum ose_errno e = ose_errno_get(osevm);
                if(e)
                {
                    ose_errno_set(osevm, OSE_ERR_NONE);
                    ose_pushInt32(vm_s, e);
                    ose_pushString(vm_c, "/!/exception");
                    ose_pushString(vm_c, "");
                }
                if(ose_bundleHasAtLeastNElems(vm_c, 1))
                {
                    ose_drop(vm_c);
                }
            }
            OSEVM_POSTCONTROL(osevm);
        }
        if(!ose_bundleIsEmpty(vm_d)
           && ose_getBundleElemCount(vm_d) > n
           && !(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE))
        {
            ose_builtin_return(osevm);
        }
        else
        {
            break;
        }
    }
    OSEVM_POSTINPUT(osevm);
}

void osevm_inputMessages(ose_bundle osevm,
                         int32_t size, const char * const bundle)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_pushMessage(vm_i, OSE_ADDRESS_ANONVAL,
                    OSE_ADDRESS_ANONVAL_LEN, 
                    1, OSETT_BLOB, size, bundle);
    ose_blobToElem(vm_i);
    ose_popAllDrop(vm_i);
}

void osevm_inputMessage(ose_bundle osevm,
                        int32_t size, const char * const message)
{
    ose_bundle vm_i = OSEVM_INPUT(osevm);
    ose_pushMessage(vm_i, OSE_ADDRESS_ANONVAL,
                    OSE_ADDRESS_ANONVAL_LEN, 
                    1, OSETT_BLOB, size, message);
    ose_blobToElem(vm_i);
    //ose_popAllDrop(vm_i);
}

#ifdef OSEVM_HAVE_SIZES
int32_t osevm_computeSizeReqs(int n, ...)
{
    va_list ap;
    va_start(ap, n);
    int32_t s = OSE_CONTEXT_MAX_OVERHEAD + OSEVM_CACHE_MSG_SIZE
        + OSEVM_INPUT_SIZE + OSE_CONTEXT_MESSAGE_OVERHEAD
        + OSEVM_STACK_SIZE + OSE_CONTEXT_MESSAGE_OVERHEAD
        + OSEVM_ENV_SIZE + OSE_CONTEXT_MESSAGE_OVERHEAD
        + OSEVM_CONTROL_SIZE + OSE_CONTEXT_MESSAGE_OVERHEAD
        + OSEVM_DUMP_SIZE + OSE_CONTEXT_MESSAGE_OVERHEAD
        + OSEVM_OUTPUT_SIZE + OSE_CONTEXT_MESSAGE_OVERHEAD;
    for(int i = 0; i < n; i++)
    {
        int32_t nn = va_arg(ap, int32_t);
        s += nn;
    }
    va_end(ap);
    return s;
}
#else
int32_t osevm_computeSizeReqs(int32_t input_size,
                              int32_t stack_size,
                              int32_t env_size,
                              int32_t control_size,
                              int32_t dump_size,
                              int32_t output_size,
                              int n, ...)
{
    va_list ap;
    va_start(ap, n);
    int32_t s = OSE_CONTEXT_MAX_OVERHEAD + OSEVM_CACHE_MSG_SIZE
        + input_size + stack_size + env_size
        + control_size + dump_size + output_size;
    for(int i = 0; i < n; i++)
    {
        int32_t nn = va_arg(ap, int32_t);
        s += nn;
    }
    va_end(ap);
    return s;
}
#endif
