/*
  Copyright (c) 2019-20 John MacCallum
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#include "ose_conf.h"
#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_symtab.h"
#include "ose_vm.h"

static void popControlToStack(ose_bundle vm_c, ose_bundle vm_s)
{
	ose_copyBundleElemToDest(vm_c, vm_s);
}

static void popInputToControl(ose_bundle vm_i, ose_bundle vm_c)
{
	ose_moveBundleElemToDest(vm_i, vm_c);
}

static void popStackToEnv(ose_bundle vm_s,
			  ose_bundle vm_e)
{
	//ose_bundleAll(vm_e);
	ose_replaceBundleElemInDest(vm_s, vm_e);
	ose_drop(vm_s);
	//ose_swap(vm_e);
	//ose_unpackDrop(vm_e);
}

static void doAssignment(ose_bundle vm_s, ose_bundle vm_e)
{
	ose_countElems(vm_s);
	int32_t n = ose_popInt32(vm_s);
	switch(n){
	case 0:
		return;
	case 1:
		ose_moveStringToAddress(vm_s);
		return;
	default:
		ose_moveStringToAddress(vm_s);
		
		if(ose_peekAddress(vm_s)[1] == ';'){
			return;
		}
		ose_moveBundleElemToDest(vm_s, vm_e);
		for(int i = 0; i < n - 2; i++){
			ose_swap(vm_s);
			if(ose_peekAddress(vm_s)[1] == ';'){
				ose_drop(vm_s);
				break;
			}
			ose_swap(vm_s);
			ose_push(vm_s);
		}
		ose_moveBundleElemToDest(vm_e, vm_s);
		ose_swap(vm_s);
		ose_push(vm_s);
		ose_replaceBundleElemInDest(vm_s, vm_e);
	}
}

static void lookupStackItemInEnv(ose_bundle vm_s,
				 ose_bundle vm_e,
				 ose_bundle vm_d)
{
	ose_countElems(vm_e);
	ose_moveBundleElemToDest(vm_e, vm_d);
	ose_pickMatch(vm_e);
	ose_countElems(vm_e);
	ose_moveBundleElemToDest(vm_d, vm_e);
	ose_eql(vm_e);
	
	if(ose_popInt32(vm_e) == 0){
		// success
		ose_swap(vm_e);
		ose_drop(vm_e);
	}else{
		// failure
		ose_copyAddressToString(vm_e);
		ose_swap(vm_e);
		ose_push(vm_e);
		ose_concatenateStrings(vm_e);
	}
	ose_moveBundleElemToDest(vm_e, vm_s);
}

ose_bundle osevm_init(ose_bundle bundle)
{
	{
		// ose_pushContextMessage(bundle,
		// 		       OSE_CONTEXT_MESSAGE_OVERHEAD
		// 		       + OSE_VM_STATUS_SIZE,
		// 		       "/st");
		// ose_pushContextMessage(bundle,
		// 		       ose_spaceAvailable(bundle),
		// 		       "/vm");
		// bundle = ose_enter(bundle, "/vm");
	}

	{
		// init vm
#ifdef OSE_VM_SIZE
		const int32_t s = OSE_VM_STACK_SIZE;
#else
		int32_t s = ose_spaceAvailable(bundle);

		int32_t cachesize = OSE_CONTEXT_MESSAGE_OVERHEAD
			+ 4 + 4 + 8 + (6 * 4);
		s -= cachesize;
		ose_pushContextMessage(bundle,
				       cachesize,
				       "/co");
		s /= 6;
#endif		

		// input from the world
		ose_pushContextMessage(bundle,
				       s,
				       "/ii");
		// stack
		ose_pushContextMessage(bundle,
				       s,
				       "/ss");
		// environment
		ose_pushContextMessage(bundle,
				       s,
				       "/ee");
		// control
		ose_pushContextMessage(bundle,
				       s,
				       "/cc");
		// dump
		ose_pushContextMessage(bundle,
				       s,
				       "/dd");
		// output to the world
		ose_pushContextMessage(bundle,
				       s,
				       "/oo");

#ifndef OSE_VM_SIZE
		ose_bundle vm_cache = ose_enter(bundle, "/co");
		ose_bundle vm_i = ose_enter(bundle, "/ii");
		ose_bundle vm_s = ose_enter(bundle, "/ss");
		ose_bundle vm_e = ose_enter(bundle, "/ee");
		ose_bundle vm_c = ose_enter(bundle, "/cc");
		ose_bundle vm_d = ose_enter(bundle, "/dd");
		ose_bundle vm_o = ose_enter(bundle, "/oo");
		ose_pushMessage(vm_cache,
				OSE_ADDRESS_ANONVAL,
				OSE_ADDRESS_ANONVAL_LEN,
				6,
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
				ose_getBundlePtr(vm_o) - ose_getBundlePtr(bundle));

		char *b = ose_getBundlePtr(bundle);
		char *c = ose_getBundlePtr(vm_cache);
		int32_t oi = (c + OSE_BUNDLE_HEADER_LEN + 16) - b;
#endif
	}
	return bundle;
}

static void applyControl(ose_bundle osevm, char *address)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);

	// pop control, push to stack, and evaluate
	int32_t addresslen = strlen(address);

	if(!strncmp(address, "/@", 2)){
		if(!strncmp(address, "/@/", 3)){
			ose_pushString(vm_s, address + 2);
		}
		if(ose_bundleIsEmpty(vm_s) == OSETT_FALSE){
			doAssignment(vm_s, vm_e);
		}
	}else if(!strncmp(address, "/$", 2)){
		if(!strncmp(address, "/$/", 3)){
			ose_pushString(vm_e,
				       address + 2);
		}else{
			popStackToEnv(vm_s, vm_e);
		}
		lookupStackItemInEnv(vm_s, vm_e, vm_d);
	}else if(!strncmp(address, "/!", 2)){
		if(!strncmp(address, "/!/", 3)){
			// void (*f)(ose_bundle) =
			// 	ose_symtab_lookup(address + 2);
			void (*f)(ose_bundle) = NULL;
			f = ose_symtab_lookup(address + 2);
			if(!f){
				int32_t o = ose_getFirstOffsetForMatch(vm_e,
								       address + 2);
				if(o){
					// assert stuff about o
				        int32_t to, ntt, lto, po, lpo;
					ose_getNthPayloadItem(vm_e,
							      1,
							      o,
							      &to,
							      &ntt,
							      &lto,
							      &po,
							      &lpo);
					if(ose_readByte(vm_e, to + 1)
					   == OSETT_BLOB){
						char *p = ose_readBlobPayload(vm_e,
									    po);
						while((uintptr_t)p % sizeof(intptr_t)){
							p++;
						}
						intptr_t i = 0;
						i = *((intptr_t *)p);
						f = ((void (*)(ose_bundle))i);
					}
				}
			}
			if(f){
				f(osevm);
			}else{
				ose_copyAddressToString(vm_c);
				ose_moveBundleElemToDest(vm_c, vm_s);
			}
		}else{
			if(ose_peekMessageArgType(vm_s) == OSETT_BLOB){
				ose_blobToElem(vm_s);
			}
			if(ose_peekType(vm_s) == OSETT_BUNDLE){
				ose_bundleAll(vm_i);
				ose_moveBundleElemToDest(vm_i, vm_d);
				ose_pushBundle(vm_d); // stack
				ose_bundleAll(vm_e);
				ose_copyBundleElemToDest(vm_e, vm_d);
				ose_unpackDrop(vm_e);
				ose_bundleAll(vm_c);
				ose_moveBundleElemToDest(vm_c, vm_d);
				
				ose_pop(vm_s);
				ose_countItems(vm_s);
				int32_t n = ose_popInt32(vm_s);
				for(int32_t i = 0; i < n; i++){
					ose_pop(vm_s);
					ose_pushInt32(vm_s, 3);
					ose_roll(vm_s);
					ose_swap(vm_s);
					ose_push(vm_s);
					ose_moveStringToAddress(vm_s);
					ose_moveBundleElemToDest(vm_s, vm_e);
				}
				ose_drop(vm_s);
				ose_moveBundleElemToDest(vm_s, vm_i);
				ose_popAllDrop(vm_i);
			}else{
				void (*f)(ose_bundle) =
					ose_symtab_lookup(ose_peekString(vm_s));
				if(f){
					ose_drop(vm_s);
					f(osevm);
				}else{
					ose_copyAddressToString(vm_c);
					ose_moveBundleElemToDest(vm_c, vm_s);
				}
			}
		}
	}else if(!strcmp(address, "/(")){
		// // move input to dump
		// ose_bundleAll(vm_i);
		// ose_moveBundleElemToDest(vm_i, "/dd");
		// move stack to dump
		ose_bundleAll(vm_s);
		ose_moveBundleElemToDest(vm_s, vm_d);

		// copy env to dump
		ose_bundleAll(vm_e);
		ose_copyBundleElemToDest(vm_e, vm_d);
		ose_unpackDrop(vm_e);
		
		// // move control to dump
		// ose_bundleAll(vm_c);
		// ose_moveBundleElemToDest(vm_c, vm_d);
	}else if(!strcmp(address, "/)")){
		//xx ose_drop(vm_c);
		// env
		ose_clear(vm_e);
		ose_moveBundleElemToDest(vm_d, vm_e);
		ose_unpackDrop(vm_e);
		// ose_bundleAll(vm_e);
		// ose_moveBundleElemToDest(vm_e, "/ss");
		// ose_moveBundleElemToDest(vm_d, "/ee");
		// ose_unpackDrop(vm_e);

		// stack
		if(ose_getBundleElemCount(vm_s)){
			ose_bundleAll(vm_s);
		}else{
			ose_pushBundle(vm_s);
		}
		ose_moveBundleElemToDest(vm_d, vm_s);
		ose_unpackDrop(vm_s);
		ose_rollBottom(vm_s);
	}else if(!strncmp(address, "/&", 2)
		 ||!strncmp(address, "/i", 2)){
		if(address[2] == '/'){
			long l = strtol(address + 3,
					NULL, 10);
			ose_pushInt32(vm_s, l);
		}else{
			popControlToStack(vm_c, vm_s);
		}
	}else if(!strncmp(address, "/f", 2)){
		if(address[2] == '/'){
			float f = strtof(address + 3,
					NULL);
			ose_pushFloat(vm_s, f);
		}else{
			popControlToStack(vm_c, vm_s);
		}
	}else if(!strncmp(address, "/s", 2)){
		if(address[2] == '/'){
			ose_pushString(vm_s,
				       address + 3);
			ose_swap(vm_s);
			ose_push(vm_s);
			ose_concatenateStrings(vm_s);
		}else{
		}
	}else{
		popControlToStack(vm_c, vm_s);
	}
}

static void popAllControl(ose_bundle osevm)
{
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	// load an element from the input to
	// the control, and unpack it
	if(strcmp(ose_peekAddress(vm_c), OSE_ADDRESS_ANONVAL)){
		ose_pushString(vm_c, OSE_ADDRESS_ANONVAL);
		ose_push(vm_c);
		ose_swapStringToAddress(vm_c);
	}
	ose_countItems(vm_c);
	int32_t n = ose_popInt32(vm_c);
	for(int i = 0; i < n; i++){
		ose_pop(vm_c);
		if(ose_isStringType(ose_peekMessageArgType(vm_c))
		   == OSETT_TRUE){
			char *b = ose_getBundlePtr(vm_c);
			char *str = ose_peekString(vm_c);
			if(!strcmp(str, "/@")
			   || !strncmp(str, "/@/", 3)
			   || !strcmp(str, "/;")
			   || !strcmp(str, "/!")
			   || !strncmp(str, "/!/", 3)
			   || !strcmp(str, "/$")
			   || !strncmp(str, "/$/", 3)
			   || !strcmp(str, "/(")
			   || !strcmp(str, "/)")
			   || !strcmp(str, "/&")
			   || !strncmp(str, "/&/", 3)
			   || !strncmp(str, "/i/", 3)
			   || !strncmp(str, "/f/", 3)
			   || !strncmp(str, "/s/", 3)
			   || !strncmp(str, "/b/", 3)){
				ose_moveStringToAddress(vm_c);
			}
		}
		ose_swap(vm_c);
	}
	ose_drop(vm_c);
}

static void restoreDump(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	// output
	// input
	// stack
	// environment
	// control

	// restore control
	ose_moveBundleElemToDest(vm_d, vm_c);
	ose_unpackDrop(vm_c);
			
	// restore env
	ose_clear(vm_e);
	ose_moveBundleElemToDest(vm_d, vm_e);
	ose_unpackDrop(vm_e);

	// restore stack
	ose_bundleAll(vm_s);
	ose_moveBundleElemToDest(vm_d, vm_s);
	ose_unpackDrop(vm_s);
	ose_rollBottom(vm_s);
	ose_unpackDrop(vm_s);

	// restore input
	ose_moveBundleElemToDest(vm_d, vm_i);
	ose_unpackDrop(vm_i);

	// move output to stack and restore output
	// ose_bundleAll(vm_o);
	// //ose_moveBundleElemToDest(vm_o, "/s");
	// ose_moveBundleElemToDest(vm_d, "/o");
	// ose_unpackDrop(vm_o);
}

char osevm_step(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	if(ose_bundleIsEmpty(vm_c) == OSETT_FALSE){
		// extern jmp_buf ose_jmp_buf;
		// if(!setjmp(ose_jmp_buf)){
		// 	applyControl(osevm, ose_peekAddress(vm_c));
		// 	ose_drop(vm_c);
		// }else{
		// 	printf("drop into debugger\n");
		// 	abort();
		// }
		ose_try{
			applyControl(osevm, ose_peekAddress(vm_c));
		}ose_catch(1){
			// debug
		}ose_finally{
			ose_drop(vm_c);
		}ose_end_try;
		return OSETT_TRUE;
	}else if(ose_bundleIsEmpty(vm_i) == OSETT_FALSE){
		popInputToControl(vm_i, vm_c);
		popAllControl(osevm);		
		return OSETT_TRUE;
	}else if(ose_bundleIsEmpty(vm_d) == OSETT_FALSE){
		restoreDump(osevm);
		return OSETT_TRUE;
	}
	return OSETT_FALSE;
}

void osevm_run(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	int32_t n = ose_getBundleElemCount(vm_d);
	while(1){
		static int x1;
		while(1){
			static int x2;
			if(ose_bundleIsEmpty(vm_c) == OSETT_TRUE){
				if(ose_bundleIsEmpty(vm_i) == OSETT_TRUE){
					break;
				}
				popInputToControl(vm_i, vm_c);
				popAllControl(osevm);
			}
			while(1){
				static int x3;
				if(ose_bundleIsEmpty(vm_c) == OSETT_TRUE){
					break;
				}
				applyControl(osevm, ose_peekAddress(vm_c));
				// check status and drop into
				// debugger if necessary
				ose_drop(vm_c);
				x3++;
			}
			x2++;
		}
		if(ose_bundleIsEmpty(vm_d) == OSETT_FALSE
		   && ose_getBundleElemCount(vm_d) > n){
			restoreDump(osevm);
		}else{
			break;
		}
		x1++;
	}
}
