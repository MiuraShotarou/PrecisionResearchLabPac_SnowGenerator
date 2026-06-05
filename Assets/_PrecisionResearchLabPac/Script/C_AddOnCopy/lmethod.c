// lmethod.c

// dmethod.c → lmethod.cにリネームした
// ndarray_createの引数sdtypeを確認した
// staticの除外を行った
// get_totalelementsの引数順を修正した

NdArray *
np_l_arange(int64_t start, int64_t end, int64_t step, SDType sdtype, char order)
{
    NdArray *result = NULL;
    if (step == 0) {
        SET_ERROR_MESSAGE("np_arange: step cannot be zero.");
        goto fail;
    }
	int nd = NDARRAY_MIN_ND;
	int64_t dimensions[NDARRAY_MAX_DIMENSIONS] = {(end - start) / step};
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
        SET_ERROR_MESSAGE("np_arange: unsupported sdtype for cast.");
        goto fail;
    }
	for (int64_t i = 0; i < dimensions[0]; i++) {
		double value = (double)(start + i * step);
        cast(result->data + i * itemsize, value);
	}
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}