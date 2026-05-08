#ifndef MACRO_H
#define MACRO_H

#define NDARRAY_MIN(a, b) ((a) < (b) ? (a) : (b))
#define NDARRAY_MAX(a, b) ((a) > (b) ? (a) : (b))

#define NPY_MAY_SHARE_BOUNDS 0
#define NPY_NO_OVERLAP       1
#define NPY_SHARE_BOUNDS     2

#endif