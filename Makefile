CC=clang

MODULES= \
modules/lined/ose_lined.so \
modules/lang/oscbn/ose_oscbn.so \
modules/net/udp/ose_udp.so

all: release
all-debug: debug

.PHONY: release 
release: TARGET=release
release: ose $(MODULES)

.PHONY: debug
debug: TARGET=debug
debug: ose $(MODULES)

hosts/repl/ose:
	cd hosts/repl && $(MAKE) $(TARGET)

ose: hosts/repl/ose ose_symtab.c
	mv hosts/repl/ose .

.FORCE:
$(MODULES): %.so: .FORCE
	cd $(dir $@) && $(MAKE) $(TARGET)

############################################################
# Derived files
############################################################
sys/ose_endianchk: CC=clang
sys/ose_endianchk: sys/ose_endianchk.c
	$(CC) -o sys/ose_endianchk sys/ose_endianchk.c

sys/ose_endian.h: sys/ose_endianchk
	sys/ose_endianchk > sys/ose_endian.h

ose_symtab.c: ose_symtab.gperf
	gperf ose_symtab.gperf > ose_symtab.c

ose_version.h:
	echo "#define OSE_VERSION \""`git describe --always --dirty --tags`"\"\n" > ose_version.h
	echo "#define OSE_DATE_COMPILED \""`date`"\"\n" >> ose_version.h


############################################################
# JS
############################################################
JS_CFILES=$(CORE_CFILES) $(VM_CFILES) $(LANG_CFILES) js/osejs.c
JS_HFILES=$(CORE_HFILES) $(VM_HFILES) $(LANG_HFILES)
EMSCRIPTEN_EXPORTED_FUNCTIONS=$(shell cat js/osejs_export.mk)
js/libose.js: CC=emcc
js/libose.js: CFLAGS=-I. -O3 -s MALLOC="emmalloc" -s EXPORTED_FUNCTIONS=$(EMSCRIPTEN_EXPORTED_FUNCTIONS) -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -g0 -s 'EXPORT_NAME="libose"'
js/libose.js: $(JS_CFILES) $(JS_HFILES) js/osejs_export.mk js/ose.js js/osevm.js
	$(CC) $(CFLAGS) -o js/libose.js \
	$(JS_CFILES)

.PHONY: js
js: js/libose.js

############################################################
# Tests
############################################################
TEST_CFILES=ut_ose_util.c ut_ose_stackops.c
TEST_HFILES=ut_ose_util.h ut_ose_stackops.h

TESTDIR=./test
UNITTESTS=$(TESTDIR)/ut_ose_util $(TESTDIR)/ut_ose_stackops
TESTS=$(UNITTESTS)

$(TESTDIR)/%: CFLAGS=$(CFLAGS_DEBUG)
$(TESTDIR)/%: $(CORE_CFILES) $(CORE_HFILES) $(TESTDIR)/%.c $(TESTDIR)/common.h
	clang $(CFLAGS) -o $@ \
	-DOSE_CONF_PROVIDE_TYPE_SYMBOL \
	-DOSE_CONF_PROVIDE_TYPE_DOUBLE \
	-DOSE_CONF_PROVIDE_TYPE_INT8 \
	-DOSE_CONF_PROVIDE_TYPE_UINT8 \
	-DOSE_CONF_PROVIDE_TYPE_UINT32 \
	-DOSE_CONF_PROVIDE_TYPE_INT64 \
	-DOSE_CONF_PROVIDE_TYPE_UINT64 \
	-DOSE_CONF_PROVIDE_TYPE_TIMETAG \
	-DOSE_CONF_PROVIDE_TYPE_TRUE \
	-DOSE_CONF_PROVIDE_TYPE_FALSE \
	-DOSE_CONF_PROVIDE_TYPE_NULL \
	-DOSE_CONF_PROVIDE_TYPE_INFINITUM \
	$(CORE_CFILES) ose_print.c $@.c

.PHONY: test
test: $(TESTS) test/common.h test/ut_common.h

############################################################
# Clean
############################################################
.PHONY: clean
clean:
	rm -rf ose *.dSYM $(TESTDIR)/*.dSYM $(TESTS) docs js/libose.js js/libose.wasm osec/osec sys/ose_endianchk sys/ose_endian.h *.o sys/*.o *.a ose_version.h
	$(foreach m,$(MODULES),$(shell cd $(dir $(m)) && $(MAKE) clean))
