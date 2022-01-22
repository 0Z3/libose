ifeq ($(OS),Windows_NT)
	OSNAME:=$(OS)
else
	OSNAME:=$(shell uname -s)
endif

CFILES=\
ose.c\
ose_assert.c\
ose_builtins.c\
ose_context.c\
ose_match.c\
ose_print.c\
ose_stackops.c\
ose_symtab.c\
ose_util.c\
ose_vm.c

HFILES=$(CFILES:.c=.h) ose_conf.h sys/ose_endian.h ose_version.h

OFILES=$(CFILES:.c=.o)

DEFINES+=\
	-DHAVE_OSE_ENDIAN_H \
	-DHAVE_OSE_VERSION_H

ifeq ($(OSNAME),Windows_NT)
	CC=x86_64-w64-mingw32-gcc

	STATIC_TARGET:=libose.a
	STATIC_TARGET_CMD=x86_64-w64-mingw32-gcc-ar cru $(STATIC_TARGET) $(OFILES)

	DYNAMIC_TARGET:=libose.dll
	DYNAMIC_TARGET_CMD=$(CC) $(LDFLAGS) $(OFILES) -shared -o $(DYNAMIC_TARGET)
else
	CC=clang

	STATIC_TARGET:=libose.a

	ifeq ($(OSNAME),Darwin)
		STATIC_TARGET_CMD=libtool -static -o $(STATIC_TARGET) $(OFILES)
	else
		STATIC_TARGET_CMD=ar rcs $(STATIC_TARGET) $(OFILES)
	endif

	DYNAMIC_TARGET:=libose.so
	DYNAMIC_TARGET_CMD=$(CC) $(LDFLAGS) $(OFILES) -shared -o $(DYNAMIC_TARGET)
endif

.PHONY: all release debug
all: release

INCLUDES=-I.

release: CFLAGS+=$(DEFINES) -Wall -O3 -c
release: LDFLAGS+=
release: $(STATIC_TARGET) $(DYNAMIC_TARGET)

debug: CFLAGS+=$(DEFINES) -Wall -DOSE_CONF_DEBUG -O0 -glldb -gmodules -c
debug: LDFLAGS+=
debug: $(STATIC_TARGET) $(DYNAMIC_TARGET)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(STATIC_TARGET): $(HFILES) $(OFILES) $(CFILES) 
	$(STATIC_TARGET_CMD)

$(DYNAMIC_TARGET): $(OFILES) $(CFILES) $(HFILES)
	$(DYNAMIC_TARGET_CMD)

############################################################
# Derived files
############################################################
#sys/ose_endianchk: CC=clang
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
# Clean
############################################################
.PHONY: clean
clean:
	rm -rf $(STATIC_TARGET) $(DYNAMIC_TARGET) *.dSYM sys/ose_endianchk sys/ose_endian.h *.o sys/*.o ose_version.h
