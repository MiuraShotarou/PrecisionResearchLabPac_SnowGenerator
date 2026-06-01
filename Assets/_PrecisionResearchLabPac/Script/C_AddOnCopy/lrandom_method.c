// lrandom.c

NdArray* // ①(最大値, 個数, + replace=false(値の重複なし))
np_l_random_choice_argumentscalar(int64_t max, int64_t count, bool replace, SDType sdtype)
{
    NdArray* result = NULL;
    NdArray* values = NULL;
    values = np_arange(0, max, 1, sdtype, 'C'); //
    if (values == NULL) {
        SET_ERROR_MESSAGE("np_random_choice_scalar: values is NULL.");
        goto fail;
    }
    result = np_l_random_choice_argumentndarray(values, count, replace);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_random_choice_scalar: result is NULL.");
        goto fail;
    }
    ndarray_free(values);
    return result;
    fail:
        ndarray_free(values);
        ndarray_free(result);
        return NULL;
}
/* np_random_choice */
NdArray* //引数 ②(配列, 個数) + replace=false(値の重複なし), count == 0 の場合は空の配列が返される
np_l_random_choice_argumentndarray(NdArray *values, int64_t count, bool replace)
{
    NdArray *result = NULL;
    if (values == NULL) {
        SET_ERROR_MESSAGE("np_random_choice_ndarray: values is NULL.");
        goto fail;
    }
    if (values->nd != 1) {
        SET_ERROR_MESSAGE("np_random_choice_ndarray: values must be 1-dimensional.");
        goto fail;
    }
    if (count < 0) {
        SET_ERROR_MESSAGE("np_random_choice_ndarray: count must be non-negative.");
        goto fail;
    }
    int64_t dimensions[NDARRAY_MIN_DIMENSIONS] = { count };
    if (count == 0) {
        dimensions[0] = NDARRAY_MIN_DIMENSIONS;
        result = np_zeros(NDARRAY_MIN_ND, dimensions, values->sdtype);
        if (result == NULL) {
            SET_ERROR_MESSAGE("np_random_choice_ndarray: result is NULL.");
            goto fail;
        }
        return result;
    }
    DoubleScalarCast cast = doublescalar_cast_by_sdtype[values->sdtype];
    if (cast == NULL) {
        SET_ERROR_MESSAGE("np_random_choice_ndarray: cast is NULL.");
        goto fail;
    }
    result = ndarray_create(NDARRAY_MIN_ND, dimensions, values->itemsize, values->sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_random_choice_ndarray: result is NULL.");
        goto fail;
    }
    int64_t total = get_totalelements(values->nd, values->dimensions);
    if (!replace && count > total) {
        SET_ERROR_MESSAGE("np_random_choice_ndarray: count exceeds total elements when replace is false.");
        goto fail;
    }
    if (replace) { //値の重複を許容
        for (int i = 0; i < count; i++) {
            int f = rand() % total; //次元index
            char* address = values->data + f * values->itemsize;
            double value = address_to_double(address, values->sdtype);
            cast(result->data + i * result->itemsize, value);
        }
    }
    else {
        // Fisher-Yates
        int indexes[total];
        for (int i = 0; i < total; i++) {
            indexes[i] = i;
        }
        for (int i = 0; i < count; i++) {
            // i 以降のランダムな値を選ぶ
            int r = i + rand() % (total - i);
            int tmp = indexes[i];
            indexes[i] = indexes[r];
            indexes[r] = tmp;
        }
        for (int i = 0; i < count; i++) {
            // indexes[i] を使って values から値を取り出す
            int f = indexes[i];
            char* address = values->data + f * values->itemsize;
            double value = address_to_double(address, values->sdtype);
            cast(result->data + i * result->itemsize, value);
        }
    }
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}