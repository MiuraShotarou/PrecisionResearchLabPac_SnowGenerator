// cast.h

#ifndef ARRAY_CAST_H
#define ARRAY_CAST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "enum.h"

///* mulitdimsarray to ndarray */ //→srcは解放しない方針で 
//static NdArray*
//multiarray_to_ndarray(char* src, int nd, int64_t dimensions, SDType sdtype) //nd と dimensionsが結局必要
//{
	// 多次元配列の一次元化を行う
//	int64_t totalelments = 

//	①nd, ②dims, ③itemsize
//}

/* SDType指定で、キャスト用メソッドを呼び出す配列群 */

// ulongスカラーのキャスト用メソッド。型落ち範囲内の型まで実装する → 現状は未実装で良い
typedef void (*UInt64ScalarCast)(char *dest, uint64_t value); // 符号付き整数の中で最大の型
extern UInt64ScalarCast ulongcalar_cast_by_sdtype[];
// SDTypeごとに long value を適性な型にキャストするメソッドの宣言
void uint64_to_bool   (char *dest, uint64_t value);
void uint64_to_sbyte  (char *dest, uint64_t value);
void uint64_to_byte   (char *dest, uint64_t value);
void uint64_to_short  (char *dest, uint64_t value);
void uint64_to_ushort (char *dest, uint64_t value);
void uint64_to_int    (char *dest, uint64_t value);
void uint64_to_uint   (char *dest, uint64_t value);
void uint64_to_long   (char *dest, uint64_t value);
void uint64_to_ulong  (char *dest, uint64_t value);
void uint64_to_float  (char *dest, uint64_t value);
void uint64_to_double (char *dest, uint64_t value);

// longスカラーのキャスト用メソッド。型落ち範囲内の型まで実装する
typedef void (*Int64ScalarCast)(char *dest, int64_t value); // 符号付き整数の中で最大の型
extern Int64ScalarCast int64calar_cast_by_sdtype[];
// SDTypeごとに long value を適性な型にキャストするメソッドの宣言
void int64_to_bool   (char *dest, int64_t value);
void int64_to_sbyte  (char *dest, int64_t value);
void int64_to_byte   (char *dest, int64_t value);
void int64_to_short  (char *dest, int64_t value);
void int64_to_ushort (char *dest, int64_t value);
void int64_to_int    (char *dest, int64_t value);
void int64_to_uint   (char *dest, int64_t value);
void int64_to_long   (char *dest, int64_t value);
void int64_to_ulong  (char *dest, int64_t value);
void int64_to_float  (char *dest, int64_t value);
void int64_to_double (char *dest, int64_t value);

// doubleスカラーのキャスト用メソッド。型落ち範囲内の型まで実装する
typedef void (*DoubleScalarCast)(char *dest, double value); // 浮動小数点の中で最大の型
extern DoubleScalarCast doublescalar_cast_by_sdtype[];
// SDTypeごとに double value を適性な型にキャストするメソッドの宣言
void double_to_bool   (char *dest, double value);
void double_to_sbyte  (char *dest, double value);
void double_to_byte   (char *dest, double value);
void double_to_short  (char *dest, double value);
void double_to_ushort (char *dest, double value);
void double_to_int    (char *dest, double value);
void double_to_uint   (char *dest, double value);
void double_to_long   (char *dest, double value);
void double_to_ulong  (char *dest, double value);
void double_to_float  (char *dest, double value);
void double_to_double (char *dest, double value);

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