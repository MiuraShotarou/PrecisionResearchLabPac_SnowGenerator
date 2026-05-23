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

/* np_where */
#define INDICES_DEFAULT_ND 2

#endif