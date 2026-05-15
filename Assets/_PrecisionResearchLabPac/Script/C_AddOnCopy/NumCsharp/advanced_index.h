#ifndef ADVANCED_INDEX_H
#define ADVANCED_INDEX_H

#include <stdbool.h>
#include "arrayobject.h"
#include "enum.h"
#include "error.h"

// get
NdArray* get_ndarray_advancedindexing(NdArray *src, NdArray *mask);

// set
void set_ndarray_advancedindexing(NdArray *out_src, NdArray *mask, NdArray *value);

// internal
static NdArray* boolindexingndarray_create   (NdArray *src, NdArray *mask);
static NdArray* fancyindexingndarray_create  (NdArray *src, NdArray *mask);
static void     assign_ndarray_boolindexing  (NdArray *src, NdArray *mask, NdArray *out_res);
static void     assign_ndarray_fancyindexing (NdArray *src, NdArray *mask, NdArray *out_res);

#endif /* ADVANCED_INDEX_H */