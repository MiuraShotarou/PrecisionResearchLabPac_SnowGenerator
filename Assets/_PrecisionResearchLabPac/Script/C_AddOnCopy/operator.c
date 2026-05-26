// operator.c

// 整数除算 → floatへ昇格
// ゼロ除算 → <math.h> INFINITY を返す。

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

	if (a->sdtype == bool) {
		SET_ERROR_MESSAGE();
		return NULL;
	}
	
	if (!check_broadcastable(a, b->nd, b->dimensions)) //check_broadcastable(NdArray *src, int dest_nd, int64_t *dest_dimensions)
	{
		SET_ERROR_MESSAGE();
		return NULL;
	}

	int dest_nd = 0;
	int64_t dest_dimensions[64];
	assign_broadcastingshape(a, b, &dest_nd, dest_dimensions); //assign_broadcastingshape(NdArray *a, NdArray *b, int *out_nd, int64_t *out_dimensions)
	
    NdArray *result = ndarray_create(dest_nd, dest_dimensions, a->itemsize, a->sdtype);
    if (result == NULL) {
		return NULL;
	}
	NdArray *view_a = np_broadcast_to(a, result->dimensions, result->nd);
	NdArray *view_b = np_broadcast_to(b, result->dimensions, result->nd);
	if (view_a == NULL || view_b == NULL) {
    	ndarray_free(view_a);
    	ndarray_free(view_b);
    	ndarray_free(result);
    	return NULL;
	}

	DScalarCast cast = dscalar_cast_by_sdtype[result->sdtype];
	if (cast == NULL) {
		SET_ERROR_MESSAGE();
		ndarray_free(view_a);
    	ndarray_free(view_b);
    	ndarray_free(result);
		return NULL;
	}

    int64_t total = get_totalelements(result->nd, result->dimensions);
    for (int64_t f = 0; f < total; f++) {
		int64_t res_indices[64];
		get_indices(result->nd, result->dimensions, f, res_indices);
		char *adress_a = get_address(view_a->data, res_indices, view_a->strides, view_a->nd);
		char *adress_b = get_address(view_b->data, res_indices, view_b->strides, view_b->nd);
		
		double value_a = address_to_double(adress_a, a->sdtype);
		double value_b = address_to_double(adress_b, b->sdtype);

		double value = op(value_a, value_b);

		cast(result->data + f * result->itemsize, value);
	}
    ndarray_free(view_a);
    ndarray_free(view_b);
    return result;
}

static double op_add     (double a, double b) { return a + b; }
static double op_subtract(double a, double b) { return a - b; }
static double op_multiply(double a, double b) { return a * b; }
static double op_divide  (double a, double b) { return a / b; }
static double op_modulo  (double a, double b) { return fmod(a, b); }