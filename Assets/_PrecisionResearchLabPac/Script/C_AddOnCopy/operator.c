// エラー条件 → 型の不一致
// T == bool の場合
// 整数除算 → floatへ昇格
// ゼロ除算 → <math.h> INFINITY を返す。
// 形状の不一致 → ①次元数が異なる場合、小さい方の先頭に1を補う。(3,) → (1, 3), ②各次元のサイズが異なる場合、どちらかが1であれば1の方を引き伸ばす。(2, 3) と (1, 3) → (2, 3) に引き伸ばす。③どちらも1でなく且つサイズが異なる場合はエラー。(2, 3) と (2, 4) → エラー ※NumCSharpでは戻り値の形状を、必ず右辺の式にある配列の中で最大のものと合わせる仕様にする
// 小数同士の除算 → fmodの結果

static NdArray* 
np_add (NdArray *a, NdArray *b) { return np_operatoroverloading(a, b, op_add);}
static NdArray*
np_subtract (NdArray *a, NdArray *b) { return np_operatoroverloading(a, b, op_subtract);}
static NdArray*
np_multiply (NdArray *a, NdArray *b) { return np_operatoroverloading(a, b, op_multiply);}
static NdArray*
np_divide (NdArray *a, NdArray *b) { return np_operatoroverloading(a, b, op_divide);}
static NdArray*
np_modulo (NdArray *a, NdArray *b) { return np_operatoroverloading(a, b, op_modulo);}

static NdArray*
np_operatoroverloading(NdArray *a, NdArray *b, double (*op)(double, double))
{
	if (a == NULL || b == NULL) {
		SET_ERROR_MESSAGE();
		return NULL;
	}

	if (a->sdtype != b->sdtype) {
		SET_ERROR_MESSAGE();
		return NULL;
	}
	
    int nd = 0;
    int64_t dimensions[64];
    // assign_broadcastingshape(NdArray** arrays, int array_count, int *out_nd, int64_t *out_dimensions)
	/* settings nd and dimensions */
	NdArray *arrays[] = {a, b};
	assign_broadcastingshape(arrays, 2, &nd, dimensions);
	
    NdArray *result = ndarray_create(nd, dimensions, a->itemsize, a->sdtype);
    if (result == NULL) {
		return NULL;
	}

    int64_t total = get_totalelements(result->nd, result->dimensions);
    DScalarCast cast = dscalar_cast_by_sdtype[a->sdtype];
    if (cast == NULL) {
		return NULL;
	}

	// exe
	/*
		a == {2, 5} + 
		b == {1, 7, 4}
		→
		a == {1, 2, 5} + 
		b == {1, 7, 4}
		→
		a == {1, 2, 5} + 
        b == {1, 7, 4}
		result == {2, 7, 6}
	*/
	NdArray *cast_a = np_ndarray_cast(a, Double);
    NdArray *cast_b = np_ndarray_cast(b, Double);
    if (cast_a == NULL || cast_b == NULL) {
        ndarray_free(cast_a);
        ndarray_free(cast_b);
        ndarray_free(result);
        return NULL;
    }

	for (int64_t f = 0; f < total; f++) {
		int64_t res_indices[64];
		get_indices(result->nd, result->dimensions, f, res_indices);
		double value_a = 0, value_b = 0;
		
		for (int64_t i = 0; i < result->nd; i++) {
			if (a->dimensions[i] < res_indices[i]) { //範囲外アクセスしようとした場合
				value_a = a->dimensions[i] == 1?
			}
			if (b->dimensions[i] < res_indices[i]) { //
				
			}
		}
		int64_t a_f = get_flat(cast_a->nd, cast_a->dimensions, res_indices); //必ず、indicesの次元数が大きい → dimensionsで範囲外アクセスしてしまう。
		int64_t b_f = get_flat(cast_b->nd, cast_b->dimensions, res_indices);

		int64_t a_indices[64];
		int64_t b_indices[64];
		memset(a_indices, 0, sizeof(a_indices));
		memset(b_indices, 0, sizeof(b_indices));
		get_indices(cast_a->nd, cast_a->dimensions, a_f, a_indices);
		get_indices(cast_b->nd, cast_b->dimensions, b_f, b_indices);

		double value = 0;
		for (int i = 0; i < result->nd; i++) {
			if (a_indices[i] > b_indices[i]) {
				char *a_address = get_address(cast_a->data, a_indices, cast_a->strides, cast_a->nd);
				memcpy(&value, a_address, sizeof(double));
				break;
			} else if (b_indices[i] > a_indices[i]) {
				char *b_address = get_address(cast_b->data, b_indices, cast_b->strides, cast_b->nd);
				memcpy(&value, b_address, sizeof(double));
				break;
			} else if (i == result->nd - 1) {
				char *a_address = get_address(cast_a->data, a_indices, cast_a->strides, cast_a->nd);
				double a_v = 0;
				memcpy(&a_v, a_address, sizeof(double));
				char *b_address = get_address(cast_b->data, b_indices, cast_b->strides, cast_b->nd);
				double b_v = 0;
				memcpy(&b_v, b_address, sizeof(double));
				value = op(a_v, b_v);
			}
		}
		/* set value result ndarray */
		char *res_address = get_address(result->data, res_indices, result->strides, result->nd);
		memcpy(res_address, &value, sizeof(double));
	}

    ndarray_free(cast_a);
    ndarray_free(cast_b);
    return result;
}

static double op_add     (double a, double b) { return a + b; }
static double op_subtract(double a, double b) { return a - b; }
static double op_multiply(double a, double b) { return a * b; }
static double op_divide  (double a, double b) { return a / b; }
static double op_modulo  (double a, double b) { return fmod(a, b); }