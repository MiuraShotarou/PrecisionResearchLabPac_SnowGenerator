#ifndef ARRAY_CAST_H
#define ARRAY_CAST_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "enum.h"

// SDType 指定で下記の(配列同士キャスト関数)を呼び出すことができるメソッド → 破壊的操作で変更する形に


void *bool_array_cast_by_sdtype   (bool     *src, SDType sdtype, size_t n); //NdArray以外でも可能
void *sbyte_array_cast_by_sdtype  (int8_t   *src, SDType sdtype, size_t n);
void *byte_array_cast_by_sdtype   (uint8_t  *src, SDType sdtype, size_t n);
void *short_array_cast_by_sdtype  (int16_t  *src, SDType sdtype, size_t n);
void *ushort_array_cast_by_sdtype (uint16_t *src, SDType sdtype, size_t n);
void *int_array_cast_by_sdtype    (int32_t  *src, SDType sdtype, size_t n);
void *uint_array_cast_by_sdtype   (uint32_t *src, SDType sdtype, size_t n);
void *long_array_cast_by_sdtype   (int64_t  *src, SDType sdtype, size_t n);
void *ulong_array_cast_by_sdtype  (uint64_t *src, SDType sdtype, size_t n);
void *float_array_cast_by_sdtype  (float    *src, SDType sdtype, size_t n);
void *double_array_cast_by_sdtype (double   *src, SDType sdtype, size_t n);
/* bool to all */
void bool_array_to_sbyte_array  (bool *src, int8_t   *dest, size_t n); //内部呼び出し用。第二引数に渡したポインタの生データに直接書き込む設計。破壊的操作
void bool_array_to_byte_array   (bool *src, uint8_t  *dest, size_t n);
void bool_array_to_short_array  (bool *src, int16_t  *dest, size_t n);
void bool_array_to_ushort_array (bool *src, uint16_t *dest, size_t n);
void bool_array_to_int_array    (bool *src, int32_t  *dest, size_t n);
void bool_array_to_uint_array   (bool *src, uint32_t *dest, size_t n);
void bool_array_to_long_array   (bool *src, int64_t  *dest, size_t n);
void bool_array_to_ulong_array  (bool *src, uint64_t *dest, size_t n);
void bool_array_to_float_array  (bool *src, float    *dest, size_t n);
void bool_array_to_double_array (bool *src, double   *dest, size_t n);
/* sbyte to all */
void sbyte_array_to_bool_array   (int8_t *src, bool     *dest, size_t n);
void sbyte_array_to_byte_array   (int8_t *src, uint8_t  *dest, size_t n);
void sbyte_array_to_short_array  (int8_t *src, int16_t  *dest, size_t n);
void sbyte_array_to_ushort_array (int8_t *src, uint16_t *dest, size_t n);
void sbyte_array_to_int_array    (int8_t *src, int32_t  *dest, size_t n);
void sbyte_array_to_uint_array   (int8_t *src, uint32_t *dest, size_t n);
void sbyte_array_to_long_array   (int8_t *src, int64_t  *dest, size_t n);
void sbyte_array_to_ulong_array  (int8_t *src, uint64_t *dest, size_t n);
void sbyte_array_to_float_array  (int8_t *src, float    *dest, size_t n);
void sbyte_array_to_double_array (int8_t *src, double   *dest, size_t n);
/* byte to all */
void byte_array_to_bool_array   (uint8_t *src, bool     *dest, size_t n);
void byte_array_to_sbyte_array  (uint8_t *src, int8_t   *dest, size_t n);
void byte_array_to_short_array  (uint8_t *src, int16_t  *dest, size_t n);
void byte_array_to_ushort_array (uint8_t *src, uint16_t *dest, size_t n);
void byte_array_to_int_array    (uint8_t *src, int32_t  *dest, size_t n);
void byte_array_to_uint_array   (uint8_t *src, uint32_t *dest, size_t n);
void byte_array_to_long_array   (uint8_t *src, int64_t  *dest, size_t n);
void byte_array_to_ulong_array  (uint8_t *src, uint64_t *dest, size_t n);
void byte_array_to_float_array  (uint8_t *src, float    *dest, size_t n);
void byte_array_to_double_array (uint8_t *src, double   *dest, size_t n);
/* short to all */
void short_array_to_bool_array   (int16_t *src, bool     *dest, size_t n);
void short_array_to_sbyte_array  (int16_t *src, int8_t   *dest, size_t n);
void short_array_to_byte_array   (int16_t *src, uint8_t  *dest, size_t n);
void short_array_to_ushort_array (int16_t *src, uint16_t *dest, size_t n);
void short_array_to_int_array    (int16_t *src, int32_t  *dest, size_t n);
void short_array_to_uint_array   (int16_t *src, uint32_t *dest, size_t n);
void short_array_to_long_array   (int16_t *src, int64_t  *dest, size_t n);
void short_array_to_ulong_array  (int16_t *src, uint64_t *dest, size_t n);
void short_array_to_float_array  (int16_t *src, float    *dest, size_t n);
void short_array_to_double_array (int16_t *src, double   *dest, size_t n);
/* ushort to all */
void ushort_array_to_bool_array   (uint16_t *src, bool     *dest, size_t n);
void ushort_array_to_sbyte_array  (uint16_t *src, int8_t   *dest, size_t n);
void ushort_array_to_byte_array   (uint16_t *src, uint8_t  *dest, size_t n);
void ushort_array_to_short_array  (uint16_t *src, int16_t  *dest, size_t n);
void ushort_array_to_int_array    (uint16_t *src, int32_t  *dest, size_t n);
void ushort_array_to_uint_array   (uint16_t *src, uint32_t *dest, size_t n);
void ushort_array_to_long_array   (uint16_t *src, int64_t  *dest, size_t n);
void ushort_array_to_ulong_array  (uint16_t *src, uint64_t *dest, size_t n);
void ushort_array_to_float_array  (uint16_t *src, float    *dest, size_t n);
void ushort_array_to_double_array (uint16_t *src, double   *dest, size_t n);
/* int to all */
void int_array_to_bool_array   (int32_t *src, bool     *dest, size_t n);
void int_array_to_sbyte_array  (int32_t *src, int8_t   *dest, size_t n);
void int_array_to_byte_array   (int32_t *src, uint8_t  *dest, size_t n);
void int_array_to_short_array  (int32_t *src, int16_t  *dest, size_t n);
void int_array_to_ushort_array (int32_t *src, uint16_t *dest, size_t n);
void int_array_to_uint_array   (int32_t *src, uint32_t *dest, size_t n);
void int_array_to_long_array   (int32_t *src, int64_t  *dest, size_t n);
void int_array_to_ulong_array  (int32_t *src, uint64_t *dest, size_t n);
void int_array_to_float_array  (int32_t *src, float    *dest, size_t n);
void int_array_to_double_array (int32_t *src, double   *dest, size_t n);
/* uint to all */
void uint_array_to_bool_array   (uint32_t *src, bool     *dest, size_t n);
void uint_array_to_sbyte_array  (uint32_t *src, int8_t   *dest, size_t n);
void uint_array_to_byte_array   (uint32_t *src, uint8_t  *dest, size_t n);
void uint_array_to_short_array  (uint32_t *src, int16_t  *dest, size_t n);
void uint_array_to_ushort_array (uint32_t *src, uint16_t *dest, size_t n);
void uint_array_to_int_array    (uint32_t *src, int32_t  *dest, size_t n);
void uint_array_to_long_array   (uint32_t *src, int64_t  *dest, size_t n);
void uint_array_to_ulong_array  (uint32_t *src, uint64_t *dest, size_t n);
void uint_array_to_float_array  (uint32_t *src, float    *dest, size_t n);
void uint_array_to_double_array (uint32_t *src, double   *dest, size_t n);
/* long to all */
void long_array_to_bool_array   (int64_t *src, bool     *dest, size_t n);
void long_array_to_sbyte_array  (int64_t *src, int8_t   *dest, size_t n);
void long_array_to_byte_array   (int64_t *src, uint8_t  *dest, size_t n);
void long_array_to_short_array  (int64_t *src, int16_t  *dest, size_t n);
void long_array_to_ushort_array (int64_t *src, uint16_t *dest, size_t n);
void long_array_to_int_array    (int64_t *src, int32_t  *dest, size_t n);
void long_array_to_uint_array   (int64_t *src, uint32_t *dest, size_t n);
void long_array_to_ulong_array  (int64_t *src, uint64_t *dest, size_t n);
void long_array_to_float_array  (int64_t *src, float    *dest, size_t n);
void long_array_to_double_array (int64_t *src, double   *dest, size_t n);
/* ulong to all */
void ulong_array_to_bool_array   (uint64_t *src, bool     *dest, size_t n);
void ulong_array_to_sbyte_array  (uint64_t *src, int8_t   *dest, size_t n);
void ulong_array_to_byte_array   (uint64_t *src, uint8_t  *dest, size_t n);
void ulong_array_to_short_array  (uint64_t *src, int16_t  *dest, size_t n);
void ulong_array_to_ushort_array (uint64_t *src, uint16_t *dest, size_t n);
void ulong_array_to_int_array    (uint64_t *src, int32_t  *dest, size_t n);
void ulong_array_to_uint_array   (uint64_t *src, uint32_t *dest, size_t n);
void ulong_array_to_long_array   (uint64_t *src, int64_t  *dest, size_t n);
void ulong_array_to_float_array  (uint64_t *src, float    *dest, size_t n);
void ulong_array_to_double_array (uint64_t *src, double   *dest, size_t n);
/* float to all */
void float_array_to_bool_array   (float *src, bool     *dest, size_t n);
void float_array_to_sbyte_array  (float *src, int8_t   *dest, size_t n);
void float_array_to_byte_array   (float *src, uint8_t  *dest, size_t n);
void float_array_to_short_array  (float *src, int16_t  *dest, size_t n);
void float_array_to_ushort_array (float *src, uint16_t *dest, size_t n);
void float_array_to_int_array    (float *src, int32_t  *dest, size_t n);
void float_array_to_uint_array   (float *src, uint32_t *dest, size_t n);
void float_array_to_long_array   (float *src, int64_t  *dest, size_t n);
void float_array_to_ulong_array  (float *src, uint64_t *dest, size_t n);
void float_array_to_double_array (float *src, double   *dest, size_t n);
/* double to all */
void double_array_to_bool_array   (double *src, bool     *dest, size_t n);
void double_array_to_sbyte_array  (double *src, int8_t   *dest, size_t n);
void double_array_to_byte_array   (double *src, uint8_t  *dest, size_t n);
void double_array_to_short_array  (double *src, int16_t  *dest, size_t n);
void double_array_to_ushort_array (double *src, uint16_t *dest, size_t n);
void double_array_to_int_array    (double *src, int32_t  *dest, size_t n);
void double_array_to_uint_array   (double *src, uint32_t *dest, size_t n);
void double_array_to_long_array   (double *src, int64_t  *dest, size_t n);
void double_array_to_ulong_array  (double *src, uint64_t *dest, size_t n);
void double_array_to_float_array  (double *src, float    *dest, size_t n);

#endif