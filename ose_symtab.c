/* C code produced by gperf version 3.0.3 */
/* Command-line: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/gperf ose_symtab.gperf  */
/* Computed positions: -k'2-3,8,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "ose_symtab.gperf"

/*
Copyright (c) 2019-21 John MacCallum Permission is hereby
granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without
limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject
to the following conditions:

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

#include <stdlib.h>
#include <string.h>
#include "ose.h"
#include "ose_stackops.h"
#include "ose_builtins.h"
#include "ose_vm.h"

#ifdef OSE_SYMTAB_FNSYMS
#define OSE_SYMTAB_VALUE(s) s, #s
#else
#define OSE_SYMTAB_VALUE(s) s
#endif

#line 46 "ose_symtab.gperf"
struct _ose_symtab_rec {
	char *name;
	void (*f)(ose_bundle);
#ifdef OSE_SYMTAB_FNSYMS
	char *fnsym;
#endif
};

#define TOTAL_KEYWORDS 142
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 24
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 348
/* maximum key range = 347, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
_ose_symtab_hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned short asso_values[] =
    {
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349,  45,  45, 349,  20,  15,  15,  15,
       10, 349, 349, 349, 349,  10,   5,  35, 349, 349,
        5,  50,  20, 349, 349, 349,  15, 349, 349, 349,
        0,  40,   5,   5,   5,   0, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349,  45,  15, 155,
        5,   0,  35,  10,  65,  65, 135,   0,   5,  30,
       90,   5,  25,  15,  40,   0,   0, 110, 120,  60,
        5,   0, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349, 349, 349, 349,
      349, 349, 349, 349, 349, 349, 349
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
      case 6:
      case 5:
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]+1];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static const struct _ose_symtab_rec _ose_symtab_wordlist[] =
  {
    {""}, {""},
#line 217 "ose_symtab.gperf"
    {"/s", OSE_SYMTAB_VALUE(OSEVM_TOSTRING)},
    {""}, {""}, {""}, {""},
#line 207 "ose_symtab.gperf"
    {"/@", OSE_SYMTAB_VALUE(OSEVM_ASSIGN)},
#line 140 "ose_symtab.gperf"
    {"/replace", OSE_SYMTAB_VALUE(ose_builtin_replace)},
#line 72 "ose_symtab.gperf"
    {"/rot", OSE_SYMTAB_VALUE(ose_builtin_rot)},
    {""},
#line 143 "ose_symtab.gperf"
    {"/route", OSE_SYMTAB_VALUE(ose_builtin_route)},
#line 211 "ose_symtab.gperf"
    {"/>", OSE_SYMTAB_VALUE(OSEVM_COPYCONTEXTBUNDLE)},
#line 184 "ose_symtab.gperf"
    {"/dotimes", OSE_SYMTAB_VALUE(ose_builtin_dotimes)},
    {""},
#line 187 "ose_symtab.gperf"
    {"/replace/bundle", OSE_SYMTAB_VALUE(ose_builtin_replaceBundle)},
    {""},
#line 214 "ose_symtab.gperf"
    {"/-", OSE_SYMTAB_VALUE(OSEVM_MOVEELEMTOCONTEXTBUNDLE)},
#line 129 "ose_symtab.gperf"
    {"/join/strings", OSE_SYMTAB_VALUE(ose_builtin_joinStrings)},
#line 161 "ose_symtab.gperf"
    {"/neg", OSE_SYMTAB_VALUE(ose_builtin_neg)},
    {""},
#line 200 "ose_symtab.gperf"
    {"/replacecontextbundle", OSE_SYMTAB_VALUE(ose_builtin_replaceContextBundle)},
#line 98 "ose_symtab.gperf"
    {"/count/elems", OSE_SYMTAB_VALUE(ose_builtin_countElems)},
#line 126 "ose_symtab.gperf"
    {"/decat/string/fromstart", OSE_SYMTAB_VALUE(ose_builtin_decatenateStringFromStart)},
#line 163 "ose_symtab.gperf"
    {"/neq", OSE_SYMTAB_VALUE(ose_builtin_neq)},
    {""},
#line 125 "ose_symtab.gperf"
    {"/decat/string/fromend", OSE_SYMTAB_VALUE(ose_builtin_decatenateStringFromEnd)},
#line 210 "ose_symtab.gperf"
    {"/'", OSE_SYMTAB_VALUE(OSEVM_QUOTE)},
#line 119 "ose_symtab.gperf"
    {"/address", OSE_SYMTAB_VALUE(ose_builtin_copyAddressToString)},
#line 155 "ose_symtab.gperf"
    {"/add", OSE_SYMTAB_VALUE(ose_builtin_add)},
#line 111 "ose_symtab.gperf"
    {"/addresses", OSE_SYMTAB_VALUE(ose_builtin_getAddresses)},
#line 90 "ose_symtab.gperf"
    {"/split", OSE_SYMTAB_VALUE(ose_builtin_split)},
#line 219 "ose_symtab.gperf"
    {"/&", OSE_SYMTAB_VALUE(OSEVM_APPENDBYTE)},
#line 165 "ose_symtab.gperf"
    {"/lt", OSE_SYMTAB_VALUE(ose_builtin_lt)},
#line 164 "ose_symtab.gperf"
    {"/lte", OSE_SYMTAB_VALUE(ose_builtin_lte)},
    {""},
#line 124 "ose_symtab.gperf"
    {"/decat/blob/fromstart", OSE_SYMTAB_VALUE(ose_builtin_decatenateBlobFromStart)},
#line 208 "ose_symtab.gperf"
    {"/$", OSE_SYMTAB_VALUE(OSEVM_LOOKUP)},
#line 85 "ose_symtab.gperf"
    {"/pop/all", OSE_SYMTAB_VALUE(ose_builtin_popAll)},
#line 123 "ose_symtab.gperf"
    {"/decat/blob/fromend", OSE_SYMTAB_VALUE(ose_builtin_decatenateBlobFromEnd)},
#line 87 "ose_symtab.gperf"
    {"/pop/all/bundle", OSE_SYMTAB_VALUE(ose_builtin_popAllBundle)},
    {""},
#line 213 "ose_symtab.gperf"
    {"/<", OSE_SYMTAB_VALUE(OSEVM_REPLACECONTEXTBUNDLE)},
#line 212 "ose_symtab.gperf"
    {"/<<", OSE_SYMTAB_VALUE(OSEVM_APPENDTOCONTEXTBUNDLE)},
#line 103 "ose_symtab.gperf"
    {"/lengths/items", OSE_SYMTAB_VALUE(ose_builtin_lengthsItems)},
#line 88 "ose_symtab.gperf"
    {"/pop/all/drop/bundle", OSE_SYMTAB_VALUE(ose_builtin_popAllDropBundle)},
#line 190 "ose_symtab.gperf"
    {"/apply", OSE_SYMTAB_VALUE(ose_builtin_apply)},
#line 216 "ose_symtab.gperf"
    {"/f", OSE_SYMTAB_VALUE(OSEVM_TOFLOAT)},
#line 132 "ose_symtab.gperf"
    {"/split/string/fromstart", OSE_SYMTAB_VALUE(ose_builtin_splitStringFromStart)},
#line 84 "ose_symtab.gperf"
    {"/pop", OSE_SYMTAB_VALUE(ose_builtin_pop)},
#line 64 "ose_symtab.gperf"
    {"/-rot", OSE_SYMTAB_VALUE(ose_builtin_notrot)},
#line 131 "ose_symtab.gperf"
    {"/split/string/fromend", OSE_SYMTAB_VALUE(ose_builtin_splitStringFromEnd)},
#line 70 "ose_symtab.gperf"
    {"/roll/bottom", OSE_SYMTAB_VALUE(ose_builtin_rollBottom)},
    {""},
#line 82 "ose_symtab.gperf"
    {"/clear/payload", OSE_SYMTAB_VALUE(ose_builtin_clearPayload)},
#line 189 "ose_symtab.gperf"
    {"/copy/elem", OSE_SYMTAB_VALUE(ose_builtin_copyElem)},
#line 81 "ose_symtab.gperf"
    {"/clear", OSE_SYMTAB_VALUE(ose_builtin_clear)},
#line 130 "ose_symtab.gperf"
    {"/string/toaddress/move", OSE_SYMTAB_VALUE(ose_builtin_moveStringToAddress)},
#line 117 "ose_symtab.gperf"
    {"/concat/blobs", OSE_SYMTAB_VALUE(ose_builtin_concatenateBlobs)},
#line 162 "ose_symtab.gperf"
    {"/eql", OSE_SYMTAB_VALUE(ose_builtin_eql)},
#line 118 "ose_symtab.gperf"
    {"/concat/strings", OSE_SYMTAB_VALUE(ose_builtin_concatenateStrings)},
    {""},
#line 199 "ose_symtab.gperf"
    {"/appendtocontextbundle", OSE_SYMTAB_VALUE(ose_builtin_appendToContextBundle)},
#line 86 "ose_symtab.gperf"
    {"/pop/all/drop", OSE_SYMTAB_VALUE(ose_builtin_popAllDrop)},
    {""},
#line 144 "ose_symtab.gperf"
    {"/route/all", OSE_SYMTAB_VALUE(ose_builtin_routeWithDelegation)},
#line 206 "ose_symtab.gperf"
    {"/appendbyte", OSE_SYMTAB_VALUE(ose_builtin_appendByte)},
#line 142 "ose_symtab.gperf"
    {"/lookup", OSE_SYMTAB_VALUE(ose_builtin_lookup)},
    {""}, {""},
#line 61 "ose_symtab.gperf"
    {"/drop", OSE_SYMTAB_VALUE(ose_builtin_drop)},
    {""},
#line 127 "ose_symtab.gperf"
    {"/elem/toblob", OSE_SYMTAB_VALUE(ose_builtin_elemToBlob)},
#line 110 "ose_symtab.gperf"
    {"/size/tt", OSE_SYMTAB_VALUE(ose_builtin_sizeTT)},
#line 146 "ose_symtab.gperf"
    {"/nth", OSE_SYMTAB_VALUE(ose_builtin_nth)},
#line 101 "ose_symtab.gperf"
    {"/length/tt", OSE_SYMTAB_VALUE(ose_builtin_lengthTT)},
    {""},
#line 108 "ose_symtab.gperf"
    {"/sizes/elems", OSE_SYMTAB_VALUE(ose_builtin_sizesElems)},
#line 120 "ose_symtab.gperf"
    {"/payload", OSE_SYMTAB_VALUE(ose_builtin_copyPayloadToBlob)},
#line 69 "ose_symtab.gperf"
    {"/roll/jth", OSE_SYMTAB_VALUE(ose_builtin_roll)},
#line 100 "ose_symtab.gperf"
    {"/length/address", OSE_SYMTAB_VALUE(ose_builtin_lengthAddress)},
#line 60 "ose_symtab.gperf"
    {"/2swap", OSE_SYMTAB_VALUE(ose_builtin_2swap)},
#line 121 "ose_symtab.gperf"
    {"/string/toaddress/swap", OSE_SYMTAB_VALUE(ose_builtin_swapStringToAddress)},
#line 104 "ose_symtab.gperf"
    {"/size/address", OSE_SYMTAB_VALUE(ose_builtin_sizeAddress)},
#line 160 "ose_symtab.gperf"
    {"/pow", OSE_SYMTAB_VALUE(ose_builtin_pow)},
#line 58 "ose_symtab.gperf"
    {"/2dup", OSE_SYMTAB_VALUE(ose_builtin_2dup)},
#line 57 "ose_symtab.gperf"
    {"/2drop", OSE_SYMTAB_VALUE(ose_builtin_2drop)},
#line 99 "ose_symtab.gperf"
    {"/count/items", OSE_SYMTAB_VALUE(ose_builtin_countItems)},
#line 134 "ose_symtab.gperf"
    {"/swap/bytes/8", OSE_SYMTAB_VALUE(ose_builtin_swap8Bytes)},
#line 186 "ose_symtab.gperf"
    {"/append/bundle", OSE_SYMTAB_VALUE(ose_builtin_appendBundle)},
#line 73 "ose_symtab.gperf"
    {"/swap", OSE_SYMTAB_VALUE(ose_builtin_swap)},
    {""},
#line 209 "ose_symtab.gperf"
    {"/!", OSE_SYMTAB_VALUE(OSEVM_FUNCALL)},
#line 133 "ose_symtab.gperf"
    {"/swap/bytes/4", OSE_SYMTAB_VALUE(ose_builtin_swap4Bytes)},
    {""}, {""}, {""},
#line 192 "ose_symtab.gperf"
    {"/return", OSE_SYMTAB_VALUE(ose_builtin_return)},
    {""},
#line 63 "ose_symtab.gperf"
    {"/nip", OSE_SYMTAB_VALUE(ose_builtin_nip)},
#line 83 "ose_symtab.gperf"
    {"/join", OSE_SYMTAB_VALUE(ose_builtin_join)},
#line 59 "ose_symtab.gperf"
    {"/2over", OSE_SYMTAB_VALUE(ose_builtin_2over)},
    {""}, {""},
#line 159 "ose_symtab.gperf"
    {"/mod", OSE_SYMTAB_VALUE(ose_builtin_mod)},
#line 106 "ose_symtab.gperf"
    {"/size/item", OSE_SYMTAB_VALUE(ose_builtin_sizeItem)},
    {""},
#line 102 "ose_symtab.gperf"
    {"/length/item", OSE_SYMTAB_VALUE(ose_builtin_lengthItem)},
#line 167 "ose_symtab.gperf"
    {"/or", OSE_SYMTAB_VALUE(ose_builtin_or)},
    {""},
#line 105 "ose_symtab.gperf"
    {"/size/elem", OSE_SYMTAB_VALUE(ose_builtin_sizeElem)},
    {""},
#line 141 "ose_symtab.gperf"
    {"/assign", OSE_SYMTAB_VALUE(ose_builtin_assign)},
#line 122 "ose_symtab.gperf"
    {"/tt", OSE_SYMTAB_VALUE(ose_builtin_copyTTToBlob)},
#line 166 "ose_symtab.gperf"
    {"/and", OSE_SYMTAB_VALUE(ose_builtin_and)},
    {""}, {""},
#line 139 "ose_symtab.gperf"
    {"/pmatch", OSE_SYMTAB_VALUE(ose_builtin_pmatch)},
#line 198 "ose_symtab.gperf"
    {"/copycontextbundle", OSE_SYMTAB_VALUE(ose_builtin_copyContextBundle)},
#line 201 "ose_symtab.gperf"
    {"/moveelemtocontextbundle", OSE_SYMTAB_VALUE(ose_builtin_moveElemToContextBundle)},
    {""}, {""}, {""},
#line 203 "ose_symtab.gperf"
    {"/tofloat", OSE_SYMTAB_VALUE(ose_builtin_toFloat)},
    {""}, {""},
#line 71 "ose_symtab.gperf"
    {"/roll/match", OSE_SYMTAB_VALUE(ose_builtin_rollMatch)},
#line 67 "ose_symtab.gperf"
    {"/pick/bottom", OSE_SYMTAB_VALUE(ose_builtin_pickBottom)},
#line 107 "ose_symtab.gperf"
    {"/size/payload", OSE_SYMTAB_VALUE(ose_builtin_sizePayload)},
#line 156 "ose_symtab.gperf"
    {"/sub", OSE_SYMTAB_VALUE(ose_builtin_sub)},
    {""}, {""},
#line 185 "ose_symtab.gperf"
    {"/copy/bundle", OSE_SYMTAB_VALUE(ose_builtin_copyBundle)},
#line 202 "ose_symtab.gperf"
    {"/toint32", OSE_SYMTAB_VALUE(ose_builtin_toInt32)},
    {""}, {""}, {""},
#line 205 "ose_symtab.gperf"
    {"/toblob", OSE_SYMTAB_VALUE(ose_builtin_toBlob)},
#line 196 "ose_symtab.gperf"
    {"/funcall", OSE_SYMTAB_VALUE(ose_builtin_funcall)},
#line 62 "ose_symtab.gperf"
    {"/dup", OSE_SYMTAB_VALUE(ose_builtin_dup)},
#line 188 "ose_symtab.gperf"
    {"/move/elem", OSE_SYMTAB_VALUE(ose_builtin_moveElem)},
    {""},
#line 109 "ose_symtab.gperf"
    {"/sizes/items", OSE_SYMTAB_VALUE(ose_builtin_sizesItems)},
    {""}, {""}, {""}, {""},
#line 174 "ose_symtab.gperf"
    {"/is/type/int", OSE_SYMTAB_VALUE(ose_builtin_isIntegerType)},
#line 177 "ose_symtab.gperf"
    {"/is/type/unit", OSE_SYMTAB_VALUE(ose_builtin_isUnitType)},
#line 175 "ose_symtab.gperf"
    {"/is/type/float", OSE_SYMTAB_VALUE(ose_builtin_isFloatType)},
    {""}, {""},
#line 194 "ose_symtab.gperf"
    {"/assignstacktoenv", OSE_SYMTAB_VALUE(ose_builtin_assignStackToEnv)},
#line 178 "ose_symtab.gperf"
    {"/is/type/bool", OSE_SYMTAB_VALUE(ose_builtin_isBoolType)},
#line 66 "ose_symtab.gperf"
    {"/pick/jth", OSE_SYMTAB_VALUE(ose_builtin_pick)},
#line 151 "ose_symtab.gperf"
    {"/push/blob", OSE_SYMTAB_VALUE(ose_builtin_makeBlob)},
#line 197 "ose_symtab.gperf"
    {"/quote", OSE_SYMTAB_VALUE(ose_builtin_quote)},
#line 145 "ose_symtab.gperf"
    {"/gather", OSE_SYMTAB_VALUE(ose_builtin_gather)},
    {""}, {""},
#line 173 "ose_symtab.gperf"
    {"/is/type/string", OSE_SYMTAB_VALUE(ose_builtin_isStringType)},
    {""}, {""},
#line 135 "ose_symtab.gperf"
    {"/swap/bytes/n", OSE_SYMTAB_VALUE(ose_builtin_swapNBytes)},
#line 191 "ose_symtab.gperf"
    {"/map", OSE_SYMTAB_VALUE(ose_builtin_map)},
    {""}, {""},
#line 128 "ose_symtab.gperf"
    {"/item/toblob", OSE_SYMTAB_VALUE(ose_builtin_itemToBlob)},
#line 137 "ose_symtab.gperf"
    {"/trim/string/start", OSE_SYMTAB_VALUE(ose_builtin_trimStringStart)},
    {""}, {""},
#line 136 "ose_symtab.gperf"
    {"/trim/string/end", OSE_SYMTAB_VALUE(ose_builtin_trimStringEnd)},
#line 218 "ose_symtab.gperf"
    {"/b", OSE_SYMTAB_VALUE(OSEVM_TOBLOB)},
    {""}, {""}, {""}, {""},
#line 116 "ose_symtab.gperf"
    {"/blob/totype", OSE_SYMTAB_VALUE(ose_builtin_blobToType)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""},
#line 158 "ose_symtab.gperf"
    {"/div", OSE_SYMTAB_VALUE(ose_builtin_div)},
#line 65 "ose_symtab.gperf"
    {"/over", OSE_SYMTAB_VALUE(ose_builtin_over)},
    {""}, {""}, {""}, {""},
#line 89 "ose_symtab.gperf"
    {"/push", OSE_SYMTAB_VALUE(ose_builtin_push)},
    {""}, {""}, {""}, {""},
#line 182 "ose_symtab.gperf"
    {"/exec", OSE_SYMTAB_VALUE(ose_builtin_exec)},
#line 68 "ose_symtab.gperf"
    {"/pick/match", OSE_SYMTAB_VALUE(ose_builtin_pickMatch)},
#line 215 "ose_symtab.gperf"
    {"/i", OSE_SYMTAB_VALUE(OSEVM_TOINT32)},
    {""}, {""}, {""},
#line 138 "ose_symtab.gperf"
    {"/match", OSE_SYMTAB_VALUE(ose_builtin_match)},
#line 115 "ose_symtab.gperf"
    {"/blob/toelem", OSE_SYMTAB_VALUE(ose_builtin_blobToElem)},
#line 183 "ose_symtab.gperf"
    {"/if", OSE_SYMTAB_VALUE(ose_builtin_if)},
#line 157 "ose_symtab.gperf"
    {"/mul", OSE_SYMTAB_VALUE(ose_builtin_mul)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 91 "ose_symtab.gperf"
    {"/unpack", OSE_SYMTAB_VALUE(ose_builtin_unpack)},
    {""}, {""}, {""}, {""}, {""}, {""},
#line 204 "ose_symtab.gperf"
    {"/tostring", OSE_SYMTAB_VALUE(ose_builtin_toString)},
#line 74 "ose_symtab.gperf"
    {"/tuck", OSE_SYMTAB_VALUE(ose_builtin_tuck)},
    {""}, {""}, {""}, {""},
#line 171 "ose_symtab.gperf"
    {"/is/addresschar", OSE_SYMTAB_VALUE(ose_builtin_isAddressChar)},
    {""},
#line 195 "ose_symtab.gperf"
    {"/lookupinenv", OSE_SYMTAB_VALUE(ose_builtin_lookupInEnv)},
    {""}, {""}, {""}, {""}, {""}, {""},
#line 172 "ose_symtab.gperf"
    {"/is/type/known", OSE_SYMTAB_VALUE(ose_builtin_isKnownTypetag)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 193 "ose_symtab.gperf"
    {"/version", OSE_SYMTAB_VALUE(ose_builtin_version)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 150 "ose_symtab.gperf"
    {"/make/bundle", OSE_SYMTAB_VALUE(ose_builtin_pushBundle)},
    {""},
#line 93 "ose_symtab.gperf"
    {"/unpack/bundle", OSE_SYMTAB_VALUE(ose_builtin_unpackBundle)},
    {""}, {""}, {""}, {""},
#line 94 "ose_symtab.gperf"
    {"/unpack/drop/bundle", OSE_SYMTAB_VALUE(ose_builtin_unpackDropBundle)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 92 "ose_symtab.gperf"
    {"/unpack/drop", OSE_SYMTAB_VALUE(ose_builtin_unpackDrop)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""},
#line 176 "ose_symtab.gperf"
    {"/is/type/numeric", OSE_SYMTAB_VALUE(ose_builtin_isNumericType)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 78 "ose_symtab.gperf"
    {"/bundle/all", OSE_SYMTAB_VALUE(ose_builtin_bundleAll)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""},
#line 80 "ose_symtab.gperf"
    {"/bundle/fromtop", OSE_SYMTAB_VALUE(ose_builtin_bundleFromTop)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 79 "ose_symtab.gperf"
    {"/bundle/frombottom", OSE_SYMTAB_VALUE(ose_builtin_bundleFromBottom)}
  };

const struct _ose_symtab_rec *
_ose_symtab_lookup (str, len)
     register const char *str;
     register unsigned int len;
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = _ose_symtab_hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = _ose_symtab_wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &_ose_symtab_wordlist[key];
        }
    }
  return 0;
}
#line 221 "ose_symtab.gperf"


void (*ose_symtab_lookup_fn(const char * const str))(ose_bundle)
{
	const struct _ose_symtab_rec *r = _ose_symtab_lookup(str, strlen(str));
	if(r){
		return r->f;
	}else{
		return NULL;
	}
}

#ifdef OSE_SYMTAB_FNSYMS
char *ose_symtab_lookup_fnsym(const char * const str)
{
	const struct _ose_symtab_rec *r = _ose_symtab_lookup(str, strlen(str));
	if(r){
		return r->fnsym;
	}else{
		return NULL;
	}
}
#endif

int ose_symtab_len(void)
{
	return TOTAL_KEYWORDS;
}

char *ose_symtab_getNthSym(int n)
{
	int c = 0, i = 0;
	while(c < TOTAL_KEYWORDS){
		char *name = _ose_symtab_wordlist[i++].name;
		if(strlen(name) >= MIN_WORD_LENGTH){
			if(c == n){
				return name;
			}
			c++;
		}
	}
	return NULL;
}