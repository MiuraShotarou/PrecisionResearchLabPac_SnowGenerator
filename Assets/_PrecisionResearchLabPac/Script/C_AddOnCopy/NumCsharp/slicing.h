#ifndef SLICING_H
#define SLICING_H

#include <stdint.h>
#include <stdbool.h>
#include "arrayobject.h"
#include "error.h"
#include "static.h"

typedef struct {
    int start;
    int stop;
    int step;
    int sign;
} SliceStruct;

NdArray* get_ndarray_slicing(NdArray *src, SliceStruct **slices, int slice_nd);
void     set_ndarray_slicing(NdArray *out_src, SliceStruct **slices, int slice_nd, NdArray *value);

static NdArray* slicingndarray_create                      (SliceStruct **slices, int slice_nd, NdArray *src);
static void     assign_ndarray_slicing                     (NdArray *src, SliceStruct **slices, int slice_nd, NdArray *out_res);
static bool     check_sliceconditions_and_assignstepsign   (SliceStruct **out_slice, int slice_nd);

#endif /* SLICING_H */