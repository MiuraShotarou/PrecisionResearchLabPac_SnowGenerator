// random_l_method.c

NdArray* // ①(最大値, 個数, + replace=false(値の重複なし))
np_random_l_choice_argumentscalar(Random *random, int64_t max, int64_t count, bool replace, SDType sdtype)
{
    NdArray* result = NULL;
    NdArray* values = NULL;
    values = np_l_arange(0, max, 1, sdtype, 'C');
    if (values == NULL) {
        SET_ERROR_MESSAGE("np_random_choice_scalar: values is NULL.");
        goto fail;
    }
    result = np_random_l_choice_argumentndarray(random, values, count, replace);
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
np_random_l_choice_argumentndarray(Random *random, NdArray *values, int64_t count, bool replace)
{
    NdArray *result = NULL;
    int64_t *indexes = NULL;
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
        for (int64_t i = 0; i < count; i++) {
            uint64_t rand = ((uint64_t)get_random(&random->param) << 32) | get_random(&random->param);
            int64_t f = (int64_t)(rand % (uint64_t)total); //次元index
            char* address = values->data + f * values->itemsize;
            memcpy(result->data + i * result->itemsize, address, values->itemsize);
        }
    }
    else {
        // Fisher-Yates
        indexes = (int64_t *)malloc(sizeof(int64_t) * total);
        if (indexes == NULL) {
            SET_ERROR_MESSAGE("np_random_choice_ndarray: malloc failed.");
            goto fail;
        }
        for (int64_t i = 0; i < total; i++) {
            indexes[i] = i;
        }
        for (int64_t i = 0; i < count; i++) {
            uint64_t rand = ((uint64_t)get_random(&random->param) << 32) | get_random(&random->param);
            int64_t r = i + (int64_t)(rand % (uint64_t)(total - i));
            int64_t tmp = indexes[i];
            indexes[i] = indexes[r];
            indexes[r] = tmp;
        }
        for (int64_t i = 0; i < count; i++) {
            // indexes[i] を使って values から値を取り出す
            int64_t f = indexes[i];
            char* address = values->data + f * values->itemsize;
            memcpy(result->data + i * result->itemsize, address, values->itemsize);
        }
    }
    free(indexes);
    return result;
    fail:
        free(indexes);
        ndarray_free(result);
        return NULL;
}