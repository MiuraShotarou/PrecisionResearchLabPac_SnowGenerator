// macro.h

#ifndef MACRO_H
#define MACRO_H

#define NDARRAY_MIN(a, b) ((a) < (b) ? (a) : (b))
#define NDARRAY_MAX(a, b) ((a) > (b) ? (a) : (b))

#define NPY_MAY_SHARE_BOUNDS 0
#define NPY_NO_OVERLAP       1
#define NPY_SHARE_BOUNDS     2

#define NDARRAY_MIN_ND		   1
#define NDARRAY_MIN_DIMENSIONS 1
#define NDARRAY_MIN_INDICES	   1
#define NDARRAY_MAX_ND         64
#define NDARRAY_MAX_DIMENSIONS 64
#define NDARRAY_MAX_INDICES	   64

#define INDICES_MAX_CAPACITY(obj) (obj->count)

#define NDARRAY_FLAG_C_CONTIGUOUS    0x0001  // C言語順でメモリが連続しているかどうか（strides の値がndarray_createで利用されている計算と正しいか）
#define NDARRAY_FLAG_F_CONTIGUOUS    0x0002  // Fortran順でメモリが連続しているかどうか
#define NDARRAY_FLAG_OWNDATA         0x0004  // 生データを所有している（書き込み可能である）
#define NDARRAY_FLAG_ISINDICES       0x0008

#define AXIS_NONE INT32_MAX

/* np_where */
#define INDICES_DEFAULT_ND 2

#endif