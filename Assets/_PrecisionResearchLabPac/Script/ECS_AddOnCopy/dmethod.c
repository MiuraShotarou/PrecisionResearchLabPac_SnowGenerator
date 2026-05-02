#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#define _MULTIARRAYMODULE

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include "numpy/arrayobject.h"
#include "numpy/arrayscalars.h"

#include "arrayobject.h"
#include "arrayfunction_override.h"
#include "npy_argparse.h"
#include "npy_config.h"
#include "npy_pycompat.h"
#include "npy_import.h"
#include "ufunc_override.h"
#include "array_coercion.h"
#include "common.h"
#include "templ_common.h" /* for npy_mul_sizes_with_overflow */
#include "ctors.h"
#include "calculation.h"
#include "convert_datatype.h"
#include "descriptor.h"
#include "dtypemeta.h"
#include "item_selection.h"
#include "conversion_utils.h"
#include "shape.h"
#include "strfuncs.h"
#include "array_assign.h"
#include "npy_dlpack.h"
#include "npy_static_data.h"
#include "multiarraymodule.h"

#include "methods.h"
#include "alloc.h"
#include "array_api_standard.h"

#include <stdarg.h>

arange
choice
np.full
copy
//typedef struct {
//    char    *data;          // 実データへのポインタ
//    int      nd;            // 次元数
//    int64_t *dimensions;   // 各次元のサイズ
//    int64_t *strides;      // 各次元でステップする際ののバイト数 → 転置ができる
//    int   itemsize;      // 1要素のバイト数 → 実質データ型でありstridesでもある
//} NdArray;

static NdArray *
np_d_arange(double start, double end, int itemsize, char order){
	
}