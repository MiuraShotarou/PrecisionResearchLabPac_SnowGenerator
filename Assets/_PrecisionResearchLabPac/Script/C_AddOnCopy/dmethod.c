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
np_d_arange(double start, double end, double step, SDType sdtype, char order)
{
	int nd = 1;
	int64_t dimensions[1] = {(end - start) / step};
	int itemsize = itemsize_cast_by_sdtype(sdtype);
	if (itemsize == -1) {
		return NULL;
	}

	NdArray *result = ndarray_create(nd, dimensions, itemsize);
	if (result == NULL) {
		return NULL;
	}

	DoubleScalarCast doublescalarcast = doublescalar_cast_by_sdtype[sdtype];
	if (doublescalarcast == NULL) {
		return NULL;
	}
	for (double i = 0; i < dimensions[0]; i++) {
		double value = start + i * step;
		doublescalarcast(result->data + i * itemsize, value);
	}

	return result;
}