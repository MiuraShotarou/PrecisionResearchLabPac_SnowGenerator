method.c
#include <stdbool.h>
#include "error.h"

// pn_asarray() /* スカラー値からNdArrayに変換する関数 */ を廃止した
// ndarray_createの引数sdtypeを確認した
// staticの除外を行った
// get_totalelementsの引数順を修正した
// np_random_choiceをrandom.cに移した
// np_ndarray_cast → np_cast にリネームした
// np_sum_return_scalarの戻り値を、double → voidに。またその変更に伴って、第二引数 void *out_resultを追加した。
// np_sum_return_scalar → assign_np_sum_return_scalar にリネームした

/* np zeros */
NdArray*
np_zeros(int64_t *size, int size_nd, SDType sdtype)//order='C'C言語, 'F'Fotran 内部でfullを呼んでも良い
{
    NdArray *result = NULL;
    double value = 0.0;
    result = np_full(size, size_nd, value, sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_zeros: result is NULL.");
        goto fail;
    }
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}
/* np ones */
NdArray*
np_ones(int64_t *size, int size_nd, SDType sdtype)//order='C'C言語, 'F'Fotran
{
    NdArray *result = NULL;
    double value = 1.0;
    result = np_full(size, size_nd, value, sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_ones: result is NULL.");
        goto fail;
    }
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}

/* np full */
NdArray*
np_full(int64_t *size, int size_nd, void *value, SDType sdtype, char order) // lも実装する
{
    NdArray *result = NULL;
    int itemsize = itemsize_cast_by_sdtype(sdtype);
    result = ndarray_create(size_nd, size, itemsize, sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_full: Cannot create a new result.");
        goto fail;
    }
    SafeCastType *safe = get_safecasttype(sdtype);
    if (safe == NULL) {
        SET_ERROR_MESSAGE("np_full: safe is NULL.");
        goto fail;
    }
    int64_t total = get_totalelements(result->nd, result->dimensions);
    switch (safe->sdtype) {
        case Long: {
            LongScalarCast cast = longscalar_cast_by_sdtype[result->sdtype];
            int64_t v;
            memcpy(&v, value, sizeof(int64_t));
            for (int64_t f = 0; f < total; f++) {
                cast(result->data + f * itemsize, v);
            } break;
        }
        case ULong: {
            ULongScalarCast cast = ulongscalar_cast_by_sdtype[result->sdtype];
            uint64_t v;
            memcpy(&v, value, sizeof(uint64_t));
            for (int64_t f = 0; f < total; f++) {
                cast(result->data + f * itemsize, v);
            } break;
        }
        case Double: {
            DoubleScalarCast cast = doublescalar_cast_by_sdtype[result->sdtype];
            double v;
            memcpy(&v, value, sizeof(double));
            for (int64_t f = 0; f < total; f++) {
                cast(result->data + f * itemsize, v);
            } break;
        }
    }
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}

NdArray* 
np_cast(NdArray *src, SDType restype) {
    NdArray *result = NULL;
    /* check NULL */
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_cast: src is NULL.");
        goto fail;
    }
    /* create array */
    int nd = src->nd;
    int64_t *dimensions = src->dimensions;
    int itemsize = itemsize_cast_by_sdtype(restype);
    if (itemsize == -1) {
        SET_ERROR_MESSAGE("np_cast: unsupported sdtype.");
        goto fail;
    }
    result = ndarray_create(nd, dimensions, itemsize, restype);
    if (result == NULL) {
        goto fail;
    }
    int64_t total = get_totalelements(result->nd, result->dimensions);
    if (src->flags & NDARRAY_FLAG_C_CONTIGUOUS) {
        ArrayCast arraycast = srcarray_to_resarray_table[src->sdtype][restype];
        if (arraycast == NULL) {
            SET_ERROR_MESSAGE("np_cast: arraycast is NULL.");
            goto fail; //pass C#
        }
        char* src_ptr = src->data;
        char* res_ptr = result->data;
        arraycast(src_ptr, res_ptr, total);
    }
    else {
        // src->sdtype が
        // ① bool, int8_t, int16_t, int32_t, int64_t → int64_tにキャストして利用
        // ② float, double → doubleにキャストして使用
        // ③ uint8_t, uint16_t, uint_32_t, uint64_t → uint64_tにキャストして利用
        SafeCastType *safe = get_safecasttype(src->sdtype);
        if (safe == NULL) {
            SET_ERROR_MESSAGE("np_cast: safe is NULL.");
            goto fail;
        }
        switch (safe->sdtype) {
            case Long: {
                LongScalarCast cast = longscalar_cast_by_sdtype[restype];
                for (int64_t f = 0; f < total; f++) {
                    int64_t indices[NDARRAY_MAX_DIMENSIONS];
                    assign_indices(src->nd, src->dimensions, f, indices);
                    char *address = get_address(src->data, indices, src->strides, src->nd);
                    int64_t value = address_to_long(address, src->sdtype);
                    cast(result->data + f * result->itemsize, value);
                } break;
            }
            case ULong: {
                ULongScalarCast cast = ulongscalar_cast_by_sdtype[restype];
                for (int64_t f = 0; f < total; f++) {
                    int64_t indices[NDARRAY_MAX_DIMENSIONS];
                    assign_indices(src->nd, src->dimensions, f, indices);
                    char *address = get_address(src->data, indices, src->strides, src->nd);
                    uint64_t value = address_to_ulong(address, src->sdtype);
                    cast(result->data + f * result->itemsize, value);
                } break;
            }
            case Double: {
                DoubleScalarCast cast = doublescalar_cast_by_sdtype[restype];
                for (int64_t f = 0; f < total; f++) {
                    int64_t indices[NDARRAY_MAX_DIMENSIONS];
                    assign_indices(src->nd, src->dimensions, f, indices);
                    char *address = get_address(src->data, indices, src->strides, src->nd);
                    double value = address_to_double(address, src->sdtype);
                    cast(result->data + f * result->itemsize, value);
                } break;
            }
        }
    }
    return result;
    fail:
        ndarray_free(result);
    return NULL;
}

/* np arange */
NdArray*
np_arange(double start, double end, double step, SDType sdtype, char order) //lも実装予定
{
    NdArray *result = NULL;
    if (step == 0) {
        SET_ERROR_MESSAGE("np_arange: step cannot be zero.");
        goto fail;
    }
	int nd = NDARRAY_MIN_ND;
	int64_t dimensions[NDARRAY_MAX_DIMENSIONS] = {(int64_t)ceil((end - start) / step)};
    int itemsize = itemsize_cast_by_sdtype(sdtype);
    if (itemsize == -1) {
        SET_ERROR_MESSAGE("np_arange: unsupported sdtype.");
        goto fail;
    }
	result = ndarray_create(nd, dimensions, itemsize, sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_arange: ndarray_create failed.");
        goto fail;
    }
    DoubleScalarCast cast = doublescalar_cast_by_sdtype[sdtype];
    if (cast == NULL) {
        SET_ERROR_MESSAGE("np_arange: cast is NULL.");
        goto fail;
    }
	for (int i = 0; i < dimensions[0]; i++) {
		double value = start + i * step;
        cast(result->data + i * itemsize, value);
	}
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}

void
assign_np_sum_return_scalar(NdArray *src, void *out_result)
{
    NdArray *cast_array = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("assign_np_sum_return_scalar: src is NULL.");
        goto fail;
    }
    SafeCastType safe = get_safecasttype(src->sdtype);
    switch (safe->sdtype)
    {
        case Long:
            cast_array = np_cast(src, Long);
            break;
        case ULong:
            cast_array = np_cast(src, ULong);
            break;
        case Double:
            cast_array = np_cast(src, Double);
            break;
    }
    if (cast_array == NULL) {
        SET_ERROR_MESSAGE("assign_np_sum_return_scalar: cast_array is NULL.");
        goto fail;
    }
    int64_t total = get_totalelements(cast_array->nd, cast_array->dimensions);
    switch (safe->sdtype)
    {
        case Long:
            int64_t sum_l = 0;
            int64_t *ptr_l = (int64_t*)cast_array->data;
            for (int64_t f = 0; f < total; f++) {
                sum_l += ptr_l[f];
            }
            memcpy(out_result, &sum_l, src->itemsize);
            break;
        case ULong:
            uint64_t sum_ul = 0;
            uint64_t *ptr_ul = (uint64_t*)cast_array->data;
            for (int64_t f = 0; f < total; f++) {
                sum_ul += ptr_ul[f];
            }
            memcpy(out_result, &sum_ul, src->itemsize);
            break;
        case Double:
            double sum_d = 0;
            double *ptr_d = (double*)cast_array->data;
            for (int64_t f = 0; f < total; f++) {
                sum_d += ptr_d[f];
            }
            memcpy(out_result, &sum_d, src->itemsize);
            break;
    }
    ndarray_free(cast_array);
    return;
    fail:
        ndarray_free(cast_array);
        out_result = NULL;
}

NdArray*
np_sum_return_array(NdArray *src, int32_t axis, bool keepdims)
{
    NdArray *result = NULL;
    NdArray *cast_array = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_sum_return_array: src is NULL.");
        goto fail;
    }
    /* conditions scalar */
    if (axis == AXIS_NONE) {
        double scalar = assign_np_sum_return_scalar(src); //
        int64_t dims[NDARRAY_MIN_DIMENSIONS] = { NDARRAY_MIN_ND };
        result = np_full(dims, 1, scalar, src->sdtype, 'C');
        if (result == NULL) {
            SET_ERROR_MESSAGE("np_sum_return_array: result is NULL.");
            goto fail;
        }
        return result;
    }
    /* create cast_array */
    cast_array = np_cast(src, Double);
    if (cast_array == NULL) {
        SET_ERROR_MESSAGE("np_sum_return_array: cast_array is NULL.");
        goto fail;
    }
    /* conditions error */
    axis = get_adjust_axis(axis, cast_array->nd);
    if (axis < 0 || axis >= cast_array->nd) {
        SET_ERROR_MESSAGE_ARGUMENT("np_sum_return_array: axis %d is out of range.", axis);
        goto fail;
    }
    /* copy dimensions */
    int res_nd = keepdims? cast_array->nd : cast_array->nd - 1;
    int64_t res_dims[NDARRAY_MAX_DIMENSIONS];
    int res_idx = 0;
    for (int d = 0; d < cast_array->nd; d++) { //nd == 4, axis == 1
        if (d == axis) {
            if (keepdims) { //次元を保持するなら
                res_dims[res_idx++] = NDARRAY_MIN_DIMENSIONS; //dimensionsの要素ではなく、1を代入する。axisに該当する次元の要素数を1にする
            }
        } 
        else {
            res_dims[res_idx++] = cast_array->dimensions[d]; //cast_array->dimensionsの各要素をres_dimsにコピー
        }
    }
    int itemsize = itemsize_cast_by_sdtype(src->sdtype);
    if (itemsize == -1) {
        SET_ERROR_MESSAGE("np_sum_return_array: unsupported restype.");
        goto fail;
    }
    result = ndarray_create(res_nd, res_dims, itemsize, src->sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_sum_return_array: result is NULL.");
        goto fail;
    }
    /* azis方向に計算 */
    int64_t outer = 1, inner = 1;
    for (int d = 0; d < axis; d++) { // dims(2, 4, 3), axis == 1
        outer *= cast_array->dimensions[d];
    }
    for (int d = axis + 1; d < cast_array->nd; d++) {
        inner *= cast_array->dimensions[d];
    }
    int64_t axis_len = cast_array->dimensions[axis];
    double *src_data = (double *)cast_array->data;
    DScalarCast cast = dscalar_cast_by_sdtype[result->sdtype];
    if (cast == NULL) {
        SET_ERROR_MESSAGE("np_sum_return_array: cast is NULL.");
        goto fail;
    }
    for (int64_t o = 0; o < outer; o++) {
        for (int64_t in = 0; in < inner; in++) {
            double sum = 0.0;
            for (int64_t a = 0; a < axis_len; a++) {
                sum += src_data[o * axis_len * inner + a * inner + in];
            }
            cast(result->data + (o * inner + in) * itemsize, sum);
        }
    }
    ndarray_free(cast_array);
    return result;
    fail:
        ndarray_free(result);
        ndarray_free(cast_array);
        return NULL;
}
NdArray*
np_pad(NdArray *src, int64_t pad_width, PadModeType mode, void* value)
{
    NdArray *result = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_pad: src is NULL.");
        goto fail;
    }
	if (pad_width < 0) {
		SET_ERROR_MESSAGE("np_pad: pad_width must be non-negative.");
		goto fail;
	}
	else if (pad_width == 0) {
		result = ndarray_copy(src);
		return result;
	}
    /* 出力配列の形状を計算 */
    int nd = src->nd;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    for (int d = 0; d < nd; d++) {
        dimensions[d] = src->dimensions[d] + pad_width * 2;
    }
    result = ndarray_create(nd, dimensions, src->itemsize, src->sdtype); //(int nd, int64_t *dimensions, int itemsize, SDType sdtype)
    /* PadModeTypeごとに出力配列を生成 */
    //result = np_full(dimensions, nd, value, src->sdtype, 'C'); //最初からfullで追加ぶんを代入しておく
    Assign_Switch_Pading assign_pad = assign_switch_pading_table[mode];
    assign_pad(src, value, pad_width, result);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_pad: result is NULL.");
        goto fail;
    }
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}

void assign_pad_constant(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    int64_t total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices);
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        bool inbounds = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Constant); //src側とindexが対応するようres_indicesを変換し、src_indicesに代入する
        char *value_address;
        if (inbounds) { //拡張されていないindicesだった場合、
            value_address = get_address(src->data, src_indices, src->strides, src->nd);
        }
        else { //拡張されているindicesだった場合
            value_address = (char*)value;
        }
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, src_address, src->itemsize);
    }
}
void assign_pad_edge(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{//端の値で埋める
    int64_t total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices);
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        bool inbounds_type = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Edge); //src側とindexが対応するようres_indicesを変換し、src_indicesに代入する
        char *value_address = get_address(src->data, src_indices, src->strides, src->nd);
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, src_address, src->itemsize);
    }
}
void assign_pad_linearramp(NdArray* src, void *end_value, int64_t pad_width, NdArray *out_result)
{// 中央から端に向かって、値を線形補間
    int64_t total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices); //res_indicesの確定
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds_type = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, LinearRamp); //src_indicesの確定
        char *value_address = get_address(src->data, src_indices, src->strides, src->nd);
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        if (inbounds_type != 0) {
            // src_indicesからvalueを計算し、変数として保持する
            int64_t pad_i = get_pad_i_linearramp(res_indices, src->nd, src->dimentions, pad_width, inbounds_type);
            // sdtypeに合うようvalueを算出する
            // それのアドレスをvalue_adressへ代入する
            assign_adjustvalue_linearramp(value_address, end_value, pad_width, pad_i, value_address, src->sdtype); //OK
        }
        memcpy(out_address, value_address, src->itemsize);
    }
}
void assign_pad_maximum(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    void *ope = NULL, *max = src->data;
    int64_t src_total = get_totalelements(src->nd, src->dimensions);
    SafeCastType safe = get_safecasttype(src->sdtype);
    switch (safe->sdtype) {
    case Long:
        for (int64_t f = 1; f < src_total; f++) {
            ope = src->data + f * src->itemsize;
            max = *(int64_t*)max > *(int64_t*)ope? max : ope;
        } break;
    case Ulong:
        for (int64_t f = 1; f < src_total; f++) {
            ope = src->data + f * src->itemsize;
            max = *(uint64_t*)max > *(uint64_t*)ope? max : ope;
        } break;
    case Double:
        for (int64_t f = 1; f < src_total; f++) {
            ope = src->data + f * src->itemsize;
            max = *(double*)max > *(double*)ope? max : ope;
        } break;
    }
    int64_t out_total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < out_total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices);
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Maximum); //src側とindexが対応するようres_indicesを変換し、src_indicesに代入する
        char *value_address;
        if (inbounds == 0) { //拡張されていないindicesだった場合、
            value_address = get_address(src->data, src_indices, src->strides, src->nd);
        }
        else { //拡張されているindicesだった場合
            value_address = (char*)max;
        }
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, value_address, src->itemsize);
    }
}
void assign_pad_mean(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    void *mean = malloc(src->itemsize), *sum = malloc(src->itemsize);
    if (mean == NULL || sum == NULL) {
        SET_ERROR_MESSAGE("assign_pad_mean: malloc is NULL.");
        goto fail;
    }
    assign_np_sum_return_scalar(src, sum);
    int64_t src_total = get_totalelements(src->nd, src->dimensions);
    SafeCastType safe = get_safecasttype(src->sdtype);
    switch (safe->sdtype) {
        case Long:
            int64_t mean_l = *(int64_t*)sum / src_total;
            memcpy(mean, &mean_l, src->itemsize);
            break;
        case Ulong:
            uint64_t mean_ul = *(uint64_t*)sum / (uint64_t)src_total;
            memcpy(mean, &mean_ul, src->itemsize);
            break;
    case Double:
            double mean_d = *(double*)sum / (double)src_total;
            memcpy(mean, &mean_d, src->itemsize);
            break;
    }
    int64_t out_total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < out_total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices);
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Mean); //src側とindexが対応するようres_indicesを変換し、src_indicesに代入する
        char *value_address;
        if (inbounds == 0) { //拡張されていないindicesだった場合、
            value_address = get_address(src->data, src_indices, src->strides, src->nd);
        }
        else { //拡張されているindicesだった場合
            value_address = (char*)mean;
        }
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, value_address, src->itemsize);
    }
    free(mean);
    free(sum);
    return;
    fail:
        free(mean);
        free(sum);
}
void assign_pad_median(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    NdArray *sorted = NULL;
    void *median = malloc(src->itemsize);
    if (median == NULL) {
        SET_ERROR_MESSAGE("assign_pad_mean: malloc is NULL.");
        goto fail;
    }
    sorted = np_sort(src, AXIS_NONE, Quicksort);
    if (sorted == NULL) {
        SET_ERROR_MESSAGE("assign_pad_median: sorted is NULL.");
        goto fail;
    }
    int64_t total = get_totalelements(src->nd, src->dimensions);
    int64_t med_max = total / 2; // (6)5 / 2 → 2, (5)4 / 2 → 2
    // value(median)の確定
    if ((total & 1) == 0) {
        int64_t med_min = med_max - 1;
        SafeCastType safe = get_safecasttype(src->sdtype);
        switch (safe->sdtype) {
            case Long:
                int64_t median_l = (*(int64_t*)(sorted->data + med_min * src->itemsize) + *(int64_t*)(sorted->data + med_max * src->itemsize)) / 2;
                memcpy(median, &median_l, src->itemsize);
                break;
            case Ulong:
                uint64_t median_ul = (*(uint64_t*)(sorted->data + med_min * src->itemsize) + *(uint64_t*)(sorted->data + med_max * src->itemsize)) / 2;
                memcpy(median, &median_ul, src->itemsize);
                break;
            case Double:
                double median_d = (*(double*)(sorted->data + med_min * src->itemsize) + *(double*)(sorted->data + med_max * src->itemsize)) / 2;
                memcpy(median, &median_d, src->itemsize);
                break;
        }
    }
    else {
        memcpy(median, sorted->data + med_max * src->itemsize, src->itemsize);
    }
    int64_t out_total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < out_total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices);
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Median); //src側とindexが対応するようres_indicesを変換し、src_indicesに代入する、必要はない
        char *value_address;
        if (inbounds == 0) { //拡張されていないindicesだった場合、
            value_address = get_address(src->data, src_indices, src->strides, src->nd);
        }
        else { //拡張されているindicesだった場合
            value_address = (char*)median;
        }
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, value_address, src->itemsize);
    }
    free(median);
    ndarray_free(sorted);
    return;
    fail:
        free(median);
        ndarray_free(sorted);
}
void assign_pad_minimum(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    void *ope = NULL, *min = src->data;
    int64_t src_total = get_totalelements(src->nd, src->dimensions);
    SafeCastType safe = get_safecasttype(src->sdtype);
    switch (safe->sdtype) {
    case Long:
        for (int64_t f = 1; f < src_total; f++) {
            ope = src->data + f * src->itemsize;
            min = *(int64_t*)min < *(int64_t*)ope? min : ope;
        } break;
    case Ulong:
        for (int64_t f = 1; f < src_total; f++) {
            ope = src->data + f * src->itemsize;
            min = *(uint64_t*)min < *(uint64_t*)ope? min : ope;
        } break;
    case Double:
        for (int64_t f = 1; f < src_total; f++) {
            ope = src->data + f * src->itemsize;
            min = *(double*)min < *(double*)ope? min : ope;
        } break;
    }
    int64_t out_total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < out_total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices);
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Minimum); //src側とindexが対応するようres_indicesを変換し、src_indicesに代入する
        char *value_address;
        if (inbounds == 0) { //拡張されていないindicesだった場合、
            value_address = get_address(src->data, src_indices, src->strides, src->nd);
        }
        else { //拡張されているindicesだった場合
            value_address = (char*)min;
        }
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, value_address, src->itemsize);
    }
}
/*
    Constant,    // 固定値で埋める（デフォルト：0）//
    Edge,        // 端の値で埋める
    LinearRamp,  // 端の値から終端値への線形補間で埋める
    Maximum,     // 最大値で埋める
    Mean,        // 平均値で埋める
    Median,      // 中央値で埋める
    Minimum,     // 最小値で埋める
    Reflect,     // 端の値を軸に反転して埋める
    Symmetric,   // 端の値を含めて対称に埋める
    Wrap,        // 配列を循環させて埋める
    Empty,       // 未定義値で埋める
*/
void assign_pad_reflect(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    int64_t total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices); //res_indicesの確定
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds_type = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Reflect); //src_indicesの確定。res_indicesをsrc対応に変換し、行列の端の要素へアクセスできるようにする。
        char *value_address = get_address(src->data, src_indices, src->strides, src->nd);
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, value_address, src->itemsize);
    }
}
// assign_adjustvalue_reflect
void assign_pad_symmetric(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    int64_t total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices); //res_indicesの確定
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds_type = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Symmetric); //src_indicesの確定。res_indicesをsrc対応に変換し、行列の端の要素へアクセスできるようにする。
        char *value_address = get_address(src->data, src_indices, src->strides, src->nd);
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, value_address, src->itemsize);
    }
}
void assign_pad_wrap(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    int64_t total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices); //res_indicesの確定
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds_type = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Wrap); //src_indicesの確定。res_indicesをsrc対応に変換し、行列の端の要素へアクセスできるようにする。
        char *value_address = get_address(src->data, src_indices, src->strides, src->nd);
        char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
        memcpy(out_address, value_address, src->itemsize);
    }
}
void assign_pad_empty(NdArray* src, void *value, int64_t pad_width, NdArray *out_result)
{
    int64_t total = get_totalelements(out_result->nd, out_result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(out_result->nd, out_result->dimensions, f, res_indices); //res_indicesの確定
        int64_t src_indices[NDARRAY_MAX_DIMENSIONS];
        int inbounds_type = check_indicesinbounds_and_assign_indicestosrc(res_indices, src->nd, pad_width, src, src_indices, Wrap); //src_indicesの確定。res_indicesをsrc対応に変換し、行列の端の要素へアクセスできるようにする。
        if (inbounds == 0) {
            char *src_address = get_address(src->data, src_indices, src->strides, src->nd);
            char *out_address = get_address(out_result->data, res_indices, out_result->strides, out_result->nd);
            memcpy(out_address, src_address, src->itemsize);
        }
    }
}
int //
check_indicesinbounds_and_assign_indicestosrc(int64_t *overflow_indices, int indices_nd, int64_t pad_width, NdArray *src, int64_t* out_result, PadModeType mode) //mode を NONE, CHECK_INDICES, ASSIGN_INDICES,のマクロで分岐したい 
{
    int result = 0; //マクロ化したい
    for (int d = 0; d < indices_nd; d++) {
        if (overflow_indices[d] <= pad_width) {
            if (mode == Edge) {
                out_result[d] = 0;
            }
            else if (mode == LinearRamp) { //右の式で利用されるvalueのアドレス先をindicesとして代入する → value + (int32_t)round(abs(value - end_value) * pad_i / pad_width)
                out_result[d] = 0;
            }
            else if (mode == Reflect) {
                out_result[d] = pad_width - overflow_indices[d];
            }
            else if (mode == Symmetric) {
                if (overflow_indices[d] == pad_width || overflow_indices[d] == pad_width - 1) { // 同じsrc_indicesを参照
                    out_result[d] = 0;
                }
                else {
                    out_result[d] = pad_width - overflow_indices[d] - 1; //overflow_indices[d] == 1, pad_width == 4, src->values == {2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4}, corecct == 2
                }
            }
            else if (mode == Wrap) {  // overflow_indices[d] == pad_width の時のみ out_result[d] = overflow_indices[d] - pad_width;にしなければならない
                out_result[d] = src->dimensions[d] - pad_width - overflow_indices[d]; //overflow_indices[d] == 1, pad_width == 4, src->values == {2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4}, corecct == 2
                // (src->dimensions[d]) - offset → offset == pad_width - overflow_indices[d](3) == 1
            }
            result = -1;
        }
        else if (overflow_indices[d] >= src->dimensions[d] + pad_width) { //※
            if (mode == Edge) {
                out_result[d] = src->dimensions[d] - 1;
            }
            else if (mode == LinearRamp) {
                out_result[d] = src->dimensions[d] - 1;
            }
            else if (mode == Reflect) {
                out_result[d] = (src->dimensions[d] - 1) - (overflow_indices[d] - (src->dimensions[d] + pad_width - 1));
            }
            else if (mode == Symmetric) {//overflow_indices[d] == 11, pad_width == 4, src->values == {4, 3, 2, 1, 1, 2, 3, 4, 5, 5, 4, 3, 2}, corecct == 2
                if (overflow_indices[d] == src->dimensions[d] + pad_width) { // 同じsrc_indicesを参照
                    out_result[d] = src->dimensions[d] - 1;
                }
                else {
                    out_result[d] = (src->dimensions[d] - 1) - (overflow_indices[d] - (src->dimensions[d] + pad_width));
                }
            }
            else if (mode == Wrap) { //overflow_indices[d] == 9, pad_width == 4, src->dimensions[d] == 5, src->values == {2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4}, corecct == 0
                out_result[d] = overflow_indices[d] - pad_width - src->dimensions[d];
            }
            result = 1;
        }
        else {
            out_result[d] = overflow_indices[d] - pad_width; // srcで渡されたindicesに対応するres_indicesを代入する
        }
    }
    return result;
}
void
assign_adjustvalue_linearramp(void *base_value, void *end_value, int64_t pad_width, int64_t pad_i, void *out_result, SDType sdtype)
{
    SafeCastType safe = get_safecasttype(src->sdtype);
    switch (safe->sdtype) {
        // base_value + round(abs(base_value - end_value) * pad_i / pad_width)
        case Long:
            int64_t value = *(int64_t *)base_value + (int64_t)round((double)(llabs((*(int64_t *)base_value - (*(int64_t *)end_value) * (double)(pad_i / pad_width)));
            memcpy(out_result, &value, sizeof(int64_t));
        case ULong:
            uint64_t value = *(uint64_t *)base_value + (uint64_t)round((NDARRAY_MAX((*(uint64_t *)base_value, (*(uint64_t *)end_value) - NDARRAY_MIN((*(uint64_t *)base_value, (*(uint64_t *)end_value)) * (double)(pad_i / pad_width)));
            memcpy(out_result, &value, sizeof(uint64_t));
        case Double:
            double value = *(double *)base_value + fabs(*(double *)base_value - *(double *)end_value) * (double)(pad_i / pad_width);
            memcpy(out_result, &value, sizeof(double));
    }
}
int64_t
get_pad_i_linearramp(int64_t *src_indices, int indices_nd, int64_t *src_dimentions, int64_t pad_width, int inbounds_type)
{
    int64_t result = 0;
    // ここの関数にclamped_indicesが渡されている時点で、inboundsではない
    int64_t a = src_indices[0];
    for (int d = 1; d < indices_nd; d++) {
        if (inbounds_type == -1) {
            a = NDARRAY_MIN(a, src_indices[d]);
        }
        else if (inbounds_type == 1) {
            a = NDARRAY_MAX(a, src_indices[d]);
        }
        else {
            // コードロジックに不備あり
            return -1;
        }
    }
    result = inbounds_type == 1? pad_width - (pad_width - a) : src_dimensions[a] - a;
    return result;
}
NdArray*
np_sort(NdArray *src, int axis, SortKind kind)
{
    NdArray *result = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_sort: src is NULL.");
        goto fail;
    }
    Assign_Switch_Sorting assign_sort = assign_switch_sorting_table[kind];
    if (assign_sort == NULL) {
        SET_ERROR_MESSAGE("np_sort: assign_sort is NULL.");
        goto fail;
    }
    axis = get_adjust_axis(axis, src->nd);
    if ((axis < 0 || axis >= src->nd) && axis != AXIS_NONE) {
        SET_ERROR_MESSAGE_ARGUMENT("np_sort: axis %d is out of range.", axis);
        goto fail;
    }
    result = ndarray_create(src->nd, src->dimensions, src->itemsize, src->sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_sort.ndarray_create: result is NULL.");
        goto fail;
    }
    // kindごとの条件分岐
    /*
    Quicksort, → クイックソートアルゴリズム◎
    Mergesort, → 
    Heapsort, → 
    Stable → 
     */
    assign_sort(src, axis, result);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_sort.assign_sort: result is NULL.");
        goto fail;
    }
    return result;
}
void assign_sort_quicksort(NdArray *src, int axis, NdArray *out_result)
{
    NdArray *target_line = NULL;
    if (src == NULL || out_result == NULL) {
        SET_ERROR_MESSAGE("assign_sort_assign_ndarray_quicksort: src or out_result is NULL.");
        goto fail;
    }
    SafeCastType safe = get_safecasttype(src->sdtype); 
    if (safe == NULL) {
        SET_ERROR_MESSAGE("np_sum_return_array: safe is NULL.");
        goto fail;
    }
    /* AXIS_NONE */
    if (axis == AXIS_NONE) {
        target_line = np_ravel(src);
        int64_t total = get_totalelements(target_line->nd, target_line->dimensions);
        assign_ndarray_quicksort(target_line, 0, total - 1, safe);
        memcpy(out_result->data, target_line->data, total * target_line->itemsize);
        goto done;
    }
    /* other axis */
    int64_t outer = 1, inner = 1;
    for (int d = 0; d < axis; d++) {
        outer *= src->dimensions[d];
    }
    for (int d = axis + 1; d < src->nd; d++) {
        inner *= src->dimensions[d];
    }
    int64_t axis_len = src->dimensions[axis];
    int64_t dims_line[NDARRAY_MIN_ND] = { axis_len };
    target_line = ndarray_create(NDARRAY_MIN_ND, dims_line, src->itemsize, src->sdtype);
    if (target_line == NULL) {
        SET_ERROR_MESSAGE("np_sum_return_array: target_line is NULL.");
        goto fail;
    }
    for (int64_t o = 0; o < outer; o++) {
        for (int64_t in = 0; in < inner; in++) {
            for (int64_t a = 0; a < axis_len; a++) {
                int64_t flat = o * axis_len * inner + a * inner + in;
                memcpy(target_line->data + a * src->itemsize, src->data + flat * src->itemsize, src->itemsize);
            }
            assign_ndarray_quicksort(target_line, 0, axis_len - 1, safe);
            for (int64_t a = 0; a < axis_len; a++) {
                int64_t flat = o * axis_len * inner + a * inner + in;
                memcpy(out_result->data + flat * out_result->itemsize, target_line->data + a * src->itemsize, src->itemsize);
            }
        }
    }
    done:
        ndarray_free(target_line);
        return;
    fail:
        ndarray_free(target_line);
        out_result = NULL;
}
void assign_ndarray_quicksort(NdArray *out_src, int64_t low_i, int64_t high_i, SafeCastType safe) {
    if (low_i < high_i) {
        int64_t pivot_i = partition(out_src, low_i, high_i, safe);
        assign_ndarray_quicksort(out_src, low_i, pivot_i - 1, safe);  // 左から探索
        assign_ndarray_quicksort(out_src, pivot_i + 1, high_i, safe); // 右を探索
    }
}
int64_t partition(NdArray *out_src, int64_t low_i, int64_t high_i, SafeCastType safe) //out_src → ndarray
{
    // out_srcのメモリアクセス処理から書いていく → 一旦void*で書いていく
    void *pivot = out_src->data + high_i * out_src->itemsize; // 末尾の要素をピボットに選ぶ
    int64_t min_i = low_i - 1;   // 小さい要素の境界
    switch (safe->sdtype) {
        case Long:
            for (int64_t i = low_i; i < high_i; i++) { //線形探索
                int64_t com = 0;
                memcpy(&com, out_src->data + i * out_src->itemsize, sizeof(int64_t));
                if (com < *(int64_t*)pivot) {
                    // out_src[min_i] と out_src[i] を交換
                    assign_ndarray_swapelements(out_src, ++min_i, i);
                }
            } break;
        case ULong:
            for (int64_t i = low_i; i < high_i; i++) {
                uint64_t com = 0;
                memcpy(&com, out_src->data + i * out_src->itemsize, sizeof(uint64_t));
                if (com < *(uint64_t*)pivot) {
                    assign_ndarray_swapelements(out_src, ++min_i, i);
                }
            } break;
        case Double:
            for (int64_t i = low_i; i < high_i; i++) {
                double com = 0;
                memcpy(&com, out_src->data + i * out_src->itemsize, sizeof(double));
                if (com < *(double*)pivot) {
                    assign_ndarray_swapelements(out_src, ++min_i, i);
                }
            } break;
    }
    // ピボットを正しい位置に配置
    min_i++;
    void *tmp = malloc(out_src->itemsize);
    memcpy(tmp, out_src->data + min_i * out_src->itemsize, out_src->itemsize); //tmp = out_src[min_i];
    memcpy(out_src->data + min_i * out_src->itemsize, pivot, out_src->itemsize); //out_src[min_i] = pivot;
    memcpy(out_src->data + high_i * out_src->itemsize, tmp, out_src->itemsize); //out_src[high_i] = tmp;
    return min_i; // ピボットの最終位置を返す
}
void assign_ndarray_swapelements(NdArray *out_src, int64_t a, int64_t b)
{
    void* tmp = malloc(out_src->itemsize);
    memcpy(tmp, out_src->data + a * out_src->itemsize, out_src->itemsize);
    memcpy(out_src->data + a * out_src->itemsize, out_src->data + b * out_src->itemsize, out_src->itemsize);
    memcpy(out_src->data + b * out_src->itemsize, tmp, out_src->itemsize);
}

void assign_sort_mergesort(NdArray *src, int axis, NdArray *out_result)
{
}
void assign_sort_heapsort(NdArray *src, int axis, NdArray *out_result)
{
}
void assign_sort_stable(NdArray *src, int axis, NdArray *out_result)
{
}

NdArray*
np_where(NdArray *conditions, NdArray *a, NdArray *b)
{
    NdArray *result = NULL;
	if (conditions == NULL) {
		SET_ERROR_MESSAGE("np_where: src is NULL.");
		goto fail;
	}
	if (conditions->sdtype != Bool) {
		SET_ERROR_MESSAGE("np_where: conditions should be Bool.");
		goto fail;
	}
	if (a == NULL && b == NULL) {
		// return conditions tuple
		// condisionsのture部分のindexを取得する
		result = get_ndarray_boolndarrayindices(conditions);
	}
    else if (a != NULL && b != NULL) {
		// return conditions value
		if (a->sdtype != b->sdtype) { //C#のコンパイルで弾かれる条件
			SET_ERROR_MESSAGE("np_where: sdtype mismatch between trueValue and falseValue.");
			goto fail;
		}
		result = get_ndarray_where(conditions, a, b);
	} 
    else {
		SET_ERROR_MESSAGE("np_where: a and b must both be specified or both be NULL.");
    	goto fail;
	}
	// 条件にNdArrayが指定された際の処理は、indexingの関数を流用できる → できるとすればboolindexingだけかな。== 演算子ならfancyも利用できるね → operator_overload関数群に処理を移して、indexingはそれを呼び出す設計にすると可読性が上がるかも
	// 条件の部分で、何が引数に渡されてくるかわからない。そう考えると、whereの中で引数の型に合わせて条件分岐する必要がある。→ C#側ですべての条件をNdArray<bool>に変換する。CNative.np_whereではそのNdArray<bool>を引数に受け取り、実装を行う方針で。
	// 条件のみを引数に渡した際は、trueのindicesをタプルに変換して戻り値に返し、第一・第二引数を指定した場合はndarrayを戻り値に返す仕様。→ 条件のみの場合でもNdArray<T>(indices)を返す仕様にしよう
	// 第一・第二引数は、C#側が必ずNdArrayに変換するため問題ない
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}

static NdArray*
get_ndarray_boolndarrayindices(NdArray *conditions)
{
    NdArray *result = NULL;
    if (conditions == NULL) {
        SET_ERROR_MESSAGE("get_ndarray_boolndarrayindices: conditions is NULL.");
        goto fail;
    }
    if (conditions->sdtype != Bool) {
        SET_ERROR_MESSAGE("get_ndarray_boolndarrayindices: conditions must be Bool type.");
        goto fail;
    }
	int64_t indices_count = 0;
    int64_t indices_array[NDARRAY_MAX_DIMENSIONS][NDARRAY_MAX_DIMENSIONS];
	int64_t total = get_totalelements(conditions->nd, conditions->dimensions);
	for (int f = 0; f < total; f++) {
		bool condition;
		memcpy(&condition, conditions->data + f * conditions->itemsize, sizeof(bool));
		if (condition) {
			assign_indices(conditions->nd, conditions->dimensions, f, indices_array[indices_count++]);
		}
	}
	result = indicesndarray_create((int64_t)conditions->nd, indices_count);
	memcpy(result->data, indices_array, result->dimensions[0] * result->dimensions[1] * sizeof(int64_t));
	return result;
    fail:
        ndarray_free(result);
        return NULL;
}

static NdArray*
indicesndarray_create(int64_t indices_nd, int64_t indices_count)
{
    NdArray *result = NULL;
	// ndは固定値, dimenions[0]は計算が必要, [1]はwhereの場合不要。他から呼び出す場合はわからない
	int nd = INDICES_DEFAULT_ND; //2
	int64_t dimensions[INDICES_DEFAULT_ND];
	dimensions[0] = indices_count;
	dimensions[1] = indices_nd;
	result = ndarray_create(nd, dimensions, sizeof(int64_t), Long);
    if (result == NULL) {
        SET_ERROR_MESSAGE("indicesndarray_create: result is NULL.");
        goto fail;
    }
	return result;
    fail:
        ndarray_free(result);
        return NULL;
}

static NdArray*
get_ndarray_where(NdArray *conditions, NdArray *a, NdArray *b)
{
    NdArray *result = NULL;
    if (conditions == NULL || a == NULL || b == NULL) {
        SET_ERROR_MESSAGE("get_ndarray_where: conditions, a or b is NULL.");
        goto fail;
    }
    if (check_scalar(conditions)) {
        SET_ERROR_MESSAGE("get_ndarray_where: conditions must not be scalar.");
        goto fail;
    }
    int nd = 0;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
	// conditions,a,bの形状が一致 or 1 であればOk
    NdArray *arrays[3] = {conditions, a, b};
    bool valid = checkshape_and_decideshape(arrays, 3, &nd, dimensions);
    if (!valid) {
        SET_ERROR_MESSAGE("np_where: shape mismatch.");
        goto fail;
    }
	// result用ndarrayの作成
	result = ndarray_create(nd, dimensions, a->itemsize, a->sdtype);
	// a,b指定がある場合のwhereの計算処理
	int64_t total = get_totalelements(conditions->nd, conditions->dimensions);
	for (int f = 0; f < total; f++) {
		bool condition;
		memcpy(&condition, conditions->data + f * conditions->itemsize, sizeof(bool));
		// resultへの代入
		if (condition) {
			memcpy(result->data + f * result->itemsize, check_scalar(a)? a->data : a->data + f * a->itemsize, result->itemsize);
		}
	    else {
			memcpy(result->data + f * result->itemsize, check_scalar(b)? b->data : b->data + f * b->itemsize, result->itemsize);
		}
	}
	return result;
    fail:
        ndarray_free(result);
        return NULL;
}

static bool
checkshape_and_decideshape(NdArray **arrays, int array_count, int *out_nd, int64_t *out_dimensions) //形状の完全一致 or スカラーを許容
{
    int nd = NDARRAY_MIN_ND;
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    memset(dimensions, 0, sizeof(dimensions));
    dimensions[0] = NDARRAY_MIN_DIMENSIONS;
    // 基準となる形状を取得
    for (int i = 0; i < array_count; i++) {
        if (check_scalar(arrays[i])) {
            continue; // scalar is skip
        }
        nd = arrays[i]->nd;
        memcpy(dimensions, arrays[i]->dimensions, sizeof(int64_t) * arrays[i]->nd);
        break;
    }
    // 形状チェック
    for (int i = 0; i < array_count; i++) {
        if (check_scalar(arrays[i])) {
            continue;
        }
        if (arrays[i]->nd != nd) {
            SET_ERROR_MESSAGE("check_shape: nd mismatch.");
            return false;
        }
        for (int d = 0; d < nd; d++) {
            if (arrays[i]->dimensions[d] != dimensions[d]) {
                SET_ERROR_MESSAGE("check_shape: dimensions mismatch.");
                return false;
            }
        }
    }
    *out_nd = nd;
    memcpy(out_dimensions, dimensions, sizeof(int64_t) * nd);
    return true;
}

/* view create not copy 生data */ //NdArrayをview化する
NdArray*
np_broadcast_to(NdArray *src, int64_t *dest_dimensions, int dest_nd)
{
    NdArray *result = NULL;
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_broadcast_to: src is NULL.");
        goto fail;
    }
    /* check can broadcast */
    if (!check_broadcastable(src, dest_nd, dest_dimensions)) {
        SET_ERROR_MESSAGE("np_broadcast_to: target shape is smaller than source shape.");
        goto fail;
    }
    /* create result ndarray */
    result = ndarray_create(dest_nd, dest_dimensions, src->itemsize, src->sdtype);
	if (result == NULL) {
		SET_ERROR_MESSAGE("np_broadcast_to: result is NULL.");
		goto fail;
	}
    /* adjust strides */
    assign_broadcastingstrides(src, result, result->strides);
    ndarray_asreference(src, result);
	if (checkndarray_flag_c_contiguous(result)) {
		assign_flags_c_contiguous_on(&result->flags);
	}
	else {
		assign_flags_c_contiguous_off(&result->flags);
	}
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}

/* properties */
int
np_nd(NdArray *src)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_nd: src is NULL.");
        goto fail;
    }
    return src->nd;
    fail:
        return -1;
}
void
np_shape(NdArray *src, int64_t *out_result)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_shape: src is NULL.");
        goto fail;
    }
    if (out_result == NULL) {
        SET_ERROR_MESSAGE("np_shape: out_result is NULL.");
        goto fail;
    }
    memcpy(out_result, src->dimensions, sizeof(int64_t) * src->nd);
    fail:
        return;
}
void
np_strides(NdArray *src, int64_t *out_result)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_strides: src is NULL.");
        goto fail;
    }
    if (out_result == NULL) {
        SET_ERROR_MESSAGE("np_strides: out_result is NULL.");
        goto fail;
    }
    memcpy(out_result, src->strides, sizeof(int64_t) * src->nd);
    return;
    fail:
        return;
}
int32_t
np_itemsize(NdArray *src)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_itemsize: src is NULL.");
        goto fail;
    }
    return src->itemsize;
    fail:
        return -1;
}
int64_t
np_nbytes(NdArray *src)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_nbytes: src is NULL.");
        goto fail;
    }
    return get_totalelements(src->nd, src->dimensions) * src->itemsize;
    fail:
        return -1;
}
void
np_indices(NdArray *src, NdArray **out_result, SDType restype)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_indices: src is NULL.");
        goto fail;
    }
    if (out_result == NULL) {
        SET_ERROR_MESSAGE("np_indices: out_result is NULL.");
        goto fail;
    }
    if (restype != Int && restype != Long) {
        SET_ERROR_MESSAGE("ndarray_indices: restype is invalid.");
        goto fail;
    }
    int itemsize = itemsize_cast_by_sdtype(restype);
    int64_t total = get_totalelements(src->nd, src->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t indices[NDARRAY_MAX_DIMENSIONS];
        assign_indices(src->nd, src->dimensions, f, indices);
        int64_t dimensions[NDARRAY_MIN_DIMENSIONS] = { src->nd };
        NdArray *value = ndarray_create(NDARRAY_MIN_ND, dimensions, itemsize, restype);
        if (value == NULL) {
            SET_ERROR_MESSAGE("ndarray_indices: result is NULL.");
            goto fail;
        }
        memcpy(value->data, indices, itemsize * src->nd);
        assign_flags_isindices_on(&value->flags);
        out_result[f] = value;
    }
    return;
    fail:
        return;
}

int64_t
np_size(NdArray* src)
{
    return get_totalelements(src->nd, src->dimensions);
}
