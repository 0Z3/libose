/*
  Copyright (c) 2019-21 John MacCallum Permission is hereby granted,
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_symtab.h"
#include "ose_print.h"
#include "ose_load.h"

void ose_loadLib(ose_bundle osevm, const char * const name)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    void *h = dlopen(name, RTLD_LAZY);
    ose_rassert(h != NULL, 1);
    void (*ose_main)(ose_bundle) = NULL;
    *(void**)(&ose_main) = dlsym(h, "ose_main");
    ose_rassert(ose_main != NULL, 1);
    ose_drop(vm_s);
    ose_main(osevm);
}

void ose_readFileLines(ose_bundle bundle, const char * const name)
{
    FILE *fp = fopen(name, "r");
    ose_rassert(fp, 1);
    const int maxlen = 256;
    char buf[maxlen];
    ose_pushBundle(bundle);
    while(fgets(buf, maxlen, fp)){
        int len = strlen(buf);
        if(len == 1 && buf[len - 1] == '\n')
        {
            ;
        }
        else
        {
            if(buf[len - 1] == '\n'){
                buf[len - 1] = 0;
            }
            ose_pushString(bundle, buf);
            ose_push(bundle);
        }
    }
    fclose(fp);
}

void ose_readFile(ose_bundle bundle, const char * const name)
{
    FILE *fp = fopen(name, "r");
    ose_rassert(fp, 1);
    int32_t s = ftell(fp);
    fseek(fp, 0, SEEK_END);
    int32_t e = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int32_t n = e - s;
    ose_pushBlob(bundle, n, NULL);
    char *p = ose_peekBlob(bundle) + 4;
    int32_t i;
    for(i = 0; i < n; i++)
    {
        *p++ = fgetc(fp);
    }
    fclose(fp);
}
