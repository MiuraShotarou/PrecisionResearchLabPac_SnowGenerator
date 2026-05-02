#ifndef ARRAY_CAST_H
#define ARRAY_CAST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "enum.h"

// SDType 指定で下記の(配列同士キャスト関数)を呼び出すことができるメソッド → 破壊的操作で変更する形に → 内部でsrcarray_to_resarray_tableを呼び出す
NdArray *np_ndarray_cast (void *src, SDType srctype, SDType restype); //NdArray.dataに反映させれば良い
/* typedefinition */
typedef void (*ArrayCast)(void *src, void *res, size_t n);
// ----------------------------------------------------------------
// SDType をindexとして利用する２次元関数ポインタテーブル [src][dest]
// ----------------------------------------------------------------
static ArrayCast srcarray_to_resarray_table[11][11] = {
    /* Bool   */ { NULL,                      boolarray_to_sbytearray,   boolarray_to_bytearray,   boolarray_to_shortarray,   boolarray_to_ushortarray,  boolarray_to_intarray,    boolarray_to_uintarray,   boolarray_to_longarray,   boolarray_to_ulongarray,  boolarray_to_floatarray,   boolarray_to_doublearray  },
    /* SByte  */ { sbytearray_to_boolarray,   NULL,                      sbytearray_to_bytearray,  sbytearray_to_shortarray,  sbytearray_to_ushortarray, sbytearray_to_intarray,   sbytearray_to_uintarray,  sbytearray_to_longarray,  sbytearray_to_ulongarray, sbytearray_to_floatarray,  sbytearray_to_doublearray },
    /* Byte   */ { bytearray_to_boolarray,    bytearray_to_sbytearray,   NULL,                     bytearray_to_shortarray,   bytearray_to_ushortarray,  bytearray_to_intarray,    bytearray_to_uintarray,   bytearray_to_longarray,   bytearray_to_ulongarray,  bytearray_to_floatarray,   bytearray_to_doublearray  },
    /* Short  */ { shortarray_to_boolarray,   shortarray_to_sbytearray,  shortarray_to_bytearray,  NULL,                      shortarray_to_ushortarray, shortarray_to_intarray,   shortarray_to_uintarray,  shortarray_to_longarray,  shortarray_to_ulongarray, shortarray_to_floatarray,  shortarray_to_doublearray },
    /* UShort */ { ushortarray_to_boolarray,  ushortarray_to_sbytearray, ushortarray_to_bytearray, ushortarray_to_shortarray, NULL,                      ushortarray_to_intarray,  ushortarray_to_uintarray, ushortarray_to_longarray, ushortarray_to_ulongarray,ushortarray_to_floatarray, ushortarray_to_doublearray},
    /* Int    */ { intarray_to_boolarray,     intarray_to_sbytearray,    intarray_to_bytearray,    intarray_to_shortarray,    intarray_to_ushortarray,   NULL,                     intarray_to_uintarray,    intarray_to_longarray,    intarray_to_ulongarray,   intarray_to_floatarray,    intarray_to_doublearray   },
    /* UInt   */ { uintarray_to_boolarray,    uintarray_to_sbytearray,   uintarray_to_bytearray,   uintarray_to_shortarray,   uintarray_to_ushortarray,  uintarray_to_intarray,    NULL,                     uintarray_to_longarray,   uintarray_to_ulongarray,  uintarray_to_floatarray,   uintarray_to_doublearray  },
    /* Long   */ { longarray_to_boolarray,    longarray_to_sbytearray,   longarray_to_bytearray,   longarray_to_shortarray,   longarray_to_ushortarray,  longarray_to_intarray,    longarray_to_uintarray,   NULL,                     longarray_to_ulongarray,  longarray_to_floatarray,   longarray_to_doublearray  },
    /* ULong  */ { ulongarray_to_boolarray,   ulongarray_to_sbytearray,  ulongarray_to_bytearray,  ulongarray_to_shortarray,  ulongarray_to_ushortarray, ulongarray_to_intarray,   ulongarray_to_uintarray,  ulongarray_to_longarray,  NULL,                     ulongarray_to_floatarray,  ulongarray_to_doublearray },
    /* Float  */ { floatarray_to_boolarray,   floatarray_to_sbytearray,  floatarray_to_bytearray,  floatarray_to_shortarray,  floatarray_to_ushortarray, floatarray_to_intarray,   floatarray_to_uintarray,  floatarray_to_longarray,  floatarray_to_ulongarray, NULL,                      floatarray_to_doublearray },
    /* Double */ { doublearray_to_boolarray,  doublearray_to_sbytearray, doublearray_to_bytearray, doublearray_to_shortarray, doublearray_to_ushortarray,doublearray_to_intarray,  doublearray_to_uintarray, doublearray_to_longarray, doublearray_to_ulongarray,doublearray_to_floatarray, NULL                      },
};
// ----------------------------------------------------------------
// bool → all
// ----------------------------------------------------------------
static void boolarray_to_sbytearray  (void *src, void *res, size_t n) { bool *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void boolarray_to_bytearray   (void *src, void *res, size_t n) { bool *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void boolarray_to_shortarray  (void *src, void *res, size_t n) { bool *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void boolarray_to_ushortarray (void *src, void *res, size_t n) { bool *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void boolarray_to_intarray    (void *src, void *res, size_t n) { bool *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void boolarray_to_uintarray   (void *src, void *res, size_t n) { bool *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void boolarray_to_longarray   (void *src, void *res, size_t n) { bool *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void boolarray_to_ulongarray  (void *src, void *res, size_t n) { bool *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void boolarray_to_floatarray  (void *src, void *res, size_t n) { bool *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void boolarray_to_doublearray (void *src, void *res, size_t n) { bool *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// sbyte → all
// ----------------------------------------------------------------
static void sbytearray_to_boolarray   (void *src, void *res, size_t n) { int8_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void sbytearray_to_bytearray   (void *src, void *res, size_t n) { int8_t *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void sbytearray_to_shortarray  (void *src, void *res, size_t n) { int8_t *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void sbytearray_to_ushortarray (void *src, void *res, size_t n) { int8_t *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void sbytearray_to_intarray    (void *src, void *res, size_t n) { int8_t *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void sbytearray_to_uintarray   (void *src, void *res, size_t n) { int8_t *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void sbytearray_to_longarray   (void *src, void *res, size_t n) { int8_t *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void sbytearray_to_ulongarray  (void *src, void *res, size_t n) { int8_t *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void sbytearray_to_floatarray  (void *src, void *res, size_t n) { int8_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void sbytearray_to_doublearray (void *src, void *res, size_t n) { int8_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// byte → all
// ----------------------------------------------------------------
static void bytearray_to_boolarray   (void *src, void *res, size_t n) { uint8_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void bytearray_to_sbytearray  (void *src, void *res, size_t n) { uint8_t *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void bytearray_to_shortarray  (void *src, void *res, size_t n) { uint8_t *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void bytearray_to_ushortarray (void *src, void *res, size_t n) { uint8_t *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void bytearray_to_intarray    (void *src, void *res, size_t n) { uint8_t *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void bytearray_to_uintarray   (void *src, void *res, size_t n) { uint8_t *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void bytearray_to_longarray   (void *src, void *res, size_t n) { uint8_t *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void bytearray_to_ulongarray  (void *src, void *res, size_t n) { uint8_t *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void bytearray_to_floatarray  (void *src, void *res, size_t n) { uint8_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void bytearray_to_doublearray (void *src, void *res, size_t n) { uint8_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// short → all
// ----------------------------------------------------------------
static void shortarray_to_boolarray   (void *src, void *res, size_t n) { int16_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void shortarray_to_sbytearray  (void *src, void *res, size_t n) { int16_t *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void shortarray_to_bytearray   (void *src, void *res, size_t n) { int16_t *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void shortarray_to_ushortarray (void *src, void *res, size_t n) { int16_t *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void shortarray_to_intarray    (void *src, void *res, size_t n) { int16_t *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void shortarray_to_uintarray   (void *src, void *res, size_t n) { int16_t *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void shortarray_to_longarray   (void *src, void *res, size_t n) { int16_t *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void shortarray_to_ulongarray  (void *src, void *res, size_t n) { int16_t *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void shortarray_to_floatarray  (void *src, void *res, size_t n) { int16_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void shortarray_to_doublearray (void *src, void *res, size_t n) { int16_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// ushort → all
// ----------------------------------------------------------------
static void ushortarray_to_boolarray   (void *src, void *res, size_t n) { uint16_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void ushortarray_to_sbytearray  (void *src, void *res, size_t n) { uint16_t *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void ushortarray_to_bytearray   (void *src, void *res, size_t n) { uint16_t *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void ushortarray_to_shortarray  (void *src, void *res, size_t n) { uint16_t *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void ushortarray_to_intarray    (void *src, void *res, size_t n) { uint16_t *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void ushortarray_to_uintarray   (void *src, void *res, size_t n) { uint16_t *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void ushortarray_to_longarray   (void *src, void *res, size_t n) { uint16_t *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void ushortarray_to_ulongarray  (void *src, void *res, size_t n) { uint16_t *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void ushortarray_to_floatarray  (void *src, void *res, size_t n) { uint16_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void ushortarray_to_doublearray (void *src, void *res, size_t n) { uint16_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// int → all
// ----------------------------------------------------------------
static void intarray_to_boolarray   (void *src, void *res, size_t n) { int32_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void intarray_to_sbytearray  (void *src, void *res, size_t n) { int32_t *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void intarray_to_bytearray   (void *src, void *res, size_t n) { int32_t *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void intarray_to_shortarray  (void *src, void *res, size_t n) { int32_t *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void intarray_to_ushortarray (void *src, void *res, size_t n) { int32_t *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void intarray_to_uintarray   (void *src, void *res, size_t n) { int32_t *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void intarray_to_longarray   (void *src, void *res, size_t n) { int32_t *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void intarray_to_ulongarray  (void *src, void *res, size_t n) { int32_t *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void intarray_to_floatarray  (void *src, void *res, size_t n) { int32_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void intarray_to_doublearray (void *src, void *res, size_t n) { int32_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// uint → all
// ----------------------------------------------------------------
static void uintarray_to_boolarray   (void *src, void *res, size_t n) { uint32_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void uintarray_to_sbytearray  (void *src, void *res, size_t n) { uint32_t *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void uintarray_to_bytearray   (void *src, void *res, size_t n) { uint32_t *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void uintarray_to_shortarray  (void *src, void *res, size_t n) { uint32_t *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void uintarray_to_ushortarray (void *src, void *res, size_t n) { uint32_t *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void uintarray_to_intarray    (void *src, void *res, size_t n) { uint32_t *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void uintarray_to_longarray   (void *src, void *res, size_t n) { uint32_t *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void uintarray_to_ulongarray  (void *src, void *res, size_t n) { uint32_t *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void uintarray_to_floatarray  (void *src, void *res, size_t n) { uint32_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void uintarray_to_doublearray (void *src, void *res, size_t n) { uint32_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// long → all
// ----------------------------------------------------------------
static void longarray_to_boolarray   (void *src, void *res, size_t n) { int64_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void longarray_to_sbytearray  (void *src, void *res, size_t n) { int64_t *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void longarray_to_bytearray   (void *src, void *res, size_t n) { int64_t *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void longarray_to_shortarray  (void *src, void *res, size_t n) { int64_t *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void longarray_to_ushortarray (void *src, void *res, size_t n) { int64_t *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void longarray_to_intarray    (void *src, void *res, size_t n) { int64_t *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void longarray_to_uintarray   (void *src, void *res, size_t n) { int64_t *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void longarray_to_ulongarray  (void *src, void *res, size_t n) { int64_t *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void longarray_to_floatarray  (void *src, void *res, size_t n) { int64_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void longarray_to_doublearray (void *src, void *res, size_t n) { int64_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// ulong → all
// ----------------------------------------------------------------
static void ulongarray_to_boolarray   (void *src, void *res, size_t n) { uint64_t *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void ulongarray_to_sbytearray  (void *src, void *res, size_t n) { uint64_t *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void ulongarray_to_bytearray   (void *src, void *res, size_t n) { uint64_t *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void ulongarray_to_shortarray  (void *src, void *res, size_t n) { uint64_t *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void ulongarray_to_ushortarray (void *src, void *res, size_t n) { uint64_t *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void ulongarray_to_intarray    (void *src, void *res, size_t n) { uint64_t *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void ulongarray_to_uintarray   (void *src, void *res, size_t n) { uint64_t *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void ulongarray_to_longarray   (void *src, void *res, size_t n) { uint64_t *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void ulongarray_to_floatarray  (void *src, void *res, size_t n) { uint64_t *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
static void ulongarray_to_doublearray (void *src, void *res, size_t n) { uint64_t *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// float → all
// ----------------------------------------------------------------
static void floatarray_to_boolarray   (void *src, void *res, size_t n) { float *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void floatarray_to_sbytearray  (void *src, void *res, size_t n) { float *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void floatarray_to_bytearray   (void *src, void *res, size_t n) { float *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void floatarray_to_shortarray  (void *src, void *res, size_t n) { float *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void floatarray_to_ushortarray (void *src, void *res, size_t n) { float *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void floatarray_to_intarray    (void *src, void *res, size_t n) { float *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void floatarray_to_uintarray   (void *src, void *res, size_t n) { float *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void floatarray_to_longarray   (void *src, void *res, size_t n) { float *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void floatarray_to_ulongarray  (void *src, void *res, size_t n) { float *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void floatarray_to_doublearray (void *src, void *res, size_t n) { float *s = src; double   *d = res; while (n--) *d++ = (double)  *s++; }
// ----------------------------------------------------------------
// double → all
// ----------------------------------------------------------------
static void doublearray_to_boolarray   (void *src, void *res, size_t n) { double *s = src; bool     *d = res; while (n--) *d++ = (bool)    *s++; }
static void doublearray_to_sbytearray  (void *src, void *res, size_t n) { double *s = src; int8_t   *d = res; while (n--) *d++ = (int8_t)  *s++; }
static void doublearray_to_bytearray   (void *src, void *res, size_t n) { double *s = src; uint8_t  *d = res; while (n--) *d++ = (uint8_t) *s++; }
static void doublearray_to_shortarray  (void *src, void *res, size_t n) { double *s = src; int16_t  *d = res; while (n--) *d++ = (int16_t) *s++; }
static void doublearray_to_ushortarray (void *src, void *res, size_t n) { double *s = src; uint16_t *d = res; while (n--) *d++ = (uint16_t)*s++; }
static void doublearray_to_intarray    (void *src, void *res, size_t n) { double *s = src; int32_t  *d = res; while (n--) *d++ = (int32_t) *s++; }
static void doublearray_to_uintarray   (void *src, void *res, size_t n) { double *s = src; uint32_t *d = res; while (n--) *d++ = (uint32_t)*s++; }
static void doublearray_to_longarray   (void *src, void *res, size_t n) { double *s = src; int64_t  *d = res; while (n--) *d++ = (int64_t) *s++; }
static void doublearray_to_ulongarray  (void *src, void *res, size_t n) { double *s = src; uint64_t *d = res; while (n--) *d++ = (uint64_t)*s++; }
static void doublearray_to_floatarray  (void *src, void *res, size_t n) { double *s = src; float    *d = res; while (n--) *d++ = (float)   *s++; }
#endif