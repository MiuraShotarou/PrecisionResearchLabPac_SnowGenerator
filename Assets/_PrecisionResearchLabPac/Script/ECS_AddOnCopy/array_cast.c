#include "array_cast.h"

// ----------------------------------------------------------------
// bool配列 → 各型配列
// ----------------------------------------------------------------
void boolArray_to_sbyteArray  (bool *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void boolArray_to_byteArray   (bool *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void boolArray_to_shortArray  (bool *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void boolArray_to_ushortArray (bool *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void boolArray_to_intArray    (bool *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void boolArray_to_uintArray   (bool *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void boolArray_to_longArray   (bool *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void boolArray_to_ulongArray  (bool *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void boolArray_to_floatArray  (bool *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void boolArray_to_doubleArray (bool *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// sbyte配列 → 各型配列
// ----------------------------------------------------------------
void sbyteArray_to_boolArray   (int8_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void sbyteArray_to_byteArray   (int8_t *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void sbyteArray_to_shortArray  (int8_t *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void sbyteArray_to_ushortArray (int8_t *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void sbyteArray_to_intArray    (int8_t *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void sbyteArray_to_uintArray   (int8_t *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void sbyteArray_to_longArray   (int8_t *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void sbyteArray_to_ulongArray  (int8_t *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void sbyteArray_to_floatArray  (int8_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void sbyteArray_to_doubleArray (int8_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// byte配列 → 各型配列
// ----------------------------------------------------------------
void byteArray_to_boolArray   (uint8_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void byteArray_to_sbyteArray  (uint8_t *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void byteArray_to_shortArray  (uint8_t *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void byteArray_to_ushortArray (uint8_t *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void byteArray_to_intArray    (uint8_t *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void byteArray_to_uintArray   (uint8_t *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void byteArray_to_longArray   (uint8_t *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void byteArray_to_ulongArray  (uint8_t *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void byteArray_to_floatArray  (uint8_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void byteArray_to_doubleArray (uint8_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// short配列 → 各型配列
// ----------------------------------------------------------------
void shortArray_to_boolArray   (int16_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void shortArray_to_sbyteArray  (int16_t *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void shortArray_to_byteArray   (int16_t *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void shortArray_to_ushortArray (int16_t *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void shortArray_to_intArray    (int16_t *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void shortArray_to_uintArray   (int16_t *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void shortArray_to_longArray   (int16_t *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void shortArray_to_ulongArray  (int16_t *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void shortArray_to_floatArray  (int16_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void shortArray_to_doubleArray (int16_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// ushort配列 → 各型配列
// ----------------------------------------------------------------
void ushortArray_to_boolArray   (uint16_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void ushortArray_to_sbyteArray  (uint16_t *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void ushortArray_to_byteArray   (uint16_t *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void ushortArray_to_shortArray  (uint16_t *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void ushortArray_to_intArray    (uint16_t *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void ushortArray_to_uintArray   (uint16_t *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void ushortArray_to_longArray   (uint16_t *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void ushortArray_to_ulongArray  (uint16_t *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void ushortArray_to_floatArray  (uint16_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void ushortArray_to_doubleArray (uint16_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// int配列 → 各型配列
// ----------------------------------------------------------------
void intArray_to_boolArray   (int32_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void intArray_to_sbyteArray  (int32_t *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void intArray_to_byteArray   (int32_t *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void intArray_to_shortArray  (int32_t *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void intArray_to_ushortArray (int32_t *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void intArray_to_uintArray   (int32_t *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void intArray_to_longArray   (int32_t *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void intArray_to_ulongArray  (int32_t *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void intArray_to_floatArray  (int32_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void intArray_to_doubleArray (int32_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// uint配列 → 各型配列
// ----------------------------------------------------------------
void uintArray_to_boolArray   (uint32_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void uintArray_to_sbyteArray  (uint32_t *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void uintArray_to_byteArray   (uint32_t *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void uintArray_to_shortArray  (uint32_t *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void uintArray_to_ushortArray (uint32_t *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void uintArray_to_intArray    (uint32_t *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void uintArray_to_longArray   (uint32_t *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void uintArray_to_ulongArray  (uint32_t *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void uintArray_to_floatArray  (uint32_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void uintArray_to_doubleArray (uint32_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// long配列 → 各型配列
// ----------------------------------------------------------------
void longArray_to_boolArray   (int64_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void longArray_to_sbyteArray  (int64_t *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void longArray_to_byteArray   (int64_t *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void longArray_to_shortArray  (int64_t *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void longArray_to_ushortArray (int64_t *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void longArray_to_intArray    (int64_t *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void longArray_to_uintArray   (int64_t *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void longArray_to_ulongArray  (int64_t *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void longArray_to_floatArray  (int64_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void longArray_to_doubleArray (int64_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// ulong配列 → 各型配列
// ----------------------------------------------------------------
void ulongArray_to_boolArray   (uint64_t *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void ulongArray_to_sbyteArray  (uint64_t *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void ulongArray_to_byteArray   (uint64_t *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void ulongArray_to_shortArray  (uint64_t *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void ulongArray_to_ushortArray (uint64_t *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void ulongArray_to_intArray    (uint64_t *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void ulongArray_to_uintArray   (uint64_t *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void ulongArray_to_longArray   (uint64_t *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void ulongArray_to_floatArray  (uint64_t *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }
void ulongArray_to_doubleArray (uint64_t *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// float配列 → 各型配列
// ----------------------------------------------------------------
void floatArray_to_boolArray   (float *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void floatArray_to_sbyteArray  (float *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void floatArray_to_byteArray   (float *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void floatArray_to_shortArray  (float *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void floatArray_to_ushortArray (float *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void floatArray_to_intArray    (float *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void floatArray_to_uintArray   (float *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void floatArray_to_longArray   (float *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void floatArray_to_ulongArray  (float *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void floatArray_to_doubleArray (float *src, double   *dest, size_t n) { while (n--) *dest++ = (double)  *src++; }

// ----------------------------------------------------------------
// double配列 → 各型配列
// ----------------------------------------------------------------
void doubleArray_to_boolArray   (double *src, bool     *dest, size_t n) { while (n--) *dest++ = (bool)    *src++; }
void doubleArray_to_sbyteArray  (double *src, int8_t   *dest, size_t n) { while (n--) *dest++ = (int8_t)  *src++; }
void doubleArray_to_byteArray   (double *src, uint8_t  *dest, size_t n) { while (n--) *dest++ = (uint8_t) *src++; }
void doubleArray_to_shortArray  (double *src, int16_t  *dest, size_t n) { while (n--) *dest++ = (int16_t) *src++; }
void doubleArray_to_ushortArray (double *src, uint16_t *dest, size_t n) { while (n--) *dest++ = (uint16_t)*src++; }
void doubleArray_to_intArray    (double *src, int32_t  *dest, size_t n) { while (n--) *dest++ = (int32_t) *src++; }
void doubleArray_to_uintArray   (double *src, uint32_t *dest, size_t n) { while (n--) *dest++ = (uint32_t)*src++; }
void doubleArray_to_longArray   (double *src, int64_t  *dest, size_t n) { while (n--) *dest++ = (int64_t) *src++; }
void doubleArray_to_ulongArray  (double *src, uint64_t *dest, size_t n) { while (n--) *dest++ = (uint64_t)*src++; }
void doubleArray_to_floatArray  (double *src, float    *dest, size_t n) { while (n--) *dest++ = (float)   *src++; }