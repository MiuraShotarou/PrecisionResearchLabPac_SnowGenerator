// operator.c

// ゼロ除算 → <math.h> INFINITY を返す。
static double op_add     (double a, double b) { return a + b; } // bool, string意外を受け付ける
static double op_subtract(double a, double b) { return a - b; }
static double op_multiply(double a, double b) { return a * b; }
static double op_divide  (double a, double b) { return a / b; }
static double op_modulo  (double a, double b) { return fmod(a, b); }
static double op_less         (double a, double b) { return a <  b; } //実際、戻り値はboolとして扱われる
static double op_less_equal   (double a, double b) { return a <= b; }
static double op_greater      (double a, double b) { return a >  b; }
static double op_greater_equal(double a, double b) { return a >= b; }

static bool op_equal        (double a, double b) { return a == b; } //すべての型を受け付ける
static bool op_not_equal    (double a, double b) { return a != b; }

static bool op_logical_and(bool a, bool b) { return a && b; } //boolのみ受け付ける
static bool op_logical_or (bool a, bool b) { return a || b; }
static bool op_logical_xor(bool a, bool b) { return a ^  b; }

static double op_logical_not(double a) { return !a; } // scalarのみ受け付ける
static double op_negative (double a) { return -a; }

static NdArray* np_add (NdArray *a, NdArray *b, SDType restype) { return numeric_operatoroverloading(a, b, restype, op_add);}
static NdArray* np_subtract (NdArray *a, NdArray *b, SDType restype) { return numeric_operatoroverloading(a, b, restype, op_subtract);}
static NdArray* np_multiply (NdArray *a, NdArray *b, SDType restype) { return numeric_operatoroverloading(a, b, restype, op_multiply);}
static NdArray* np_divide (NdArray *a, NdArray *b, SDType restype) { return numeric_operatoroverloading(a, b, restype, op_divide);}
static NdArray* np_modulo (NdArray *a, NdArray *b, SDType restype) { return numeric_operatoroverloading(a, b, restype, op_modulo);}
static NdArray* np_less (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, Bool, op_less);}
static NdArray* np_less_equal (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, Bool, op_less_equal);}
static NdArray* np_greater (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, Bool, op_greater);}
static NdArray* np_greater_equal(NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, Bool, op_greater_equal);}

static NdArray* np_equal (NdArray *a, NdArray *b) { return alltype_operatoroverloading(a, b, op_equal);}
static NdArray* np_not_equal (NdArray *a, NdArray *b) { return alltype_operatoroverloading(a, b, op_not_equal);}

static NdArray* np_logical_and (NdArray *a, NdArray *b) { return boolean_operatoroverloading(a, b, op_logical_and);}
static NdArray* np_logical_or (NdArray *a, NdArray *b) { return boolean_operatoroverloading(a, b, op_logical_or);}
static NdArray* np_logical_xor (NdArray *a, NdArray *b) { return boolean_operatoroverloading(a, b, op_logical_xor);}

static NdArray* np_logical_not (NdArray *a) { return scalar_operatoroverloading(a, op_logical_not);}
static NdArray* np_negative (NdArray *a) { return scalar_operatoroverloading(a, op_negative);}

static NdArray* //
numeric_operatoroverloading(NdArray *a, NdArray *b, SDType restype, double (*op)(double, double))
{
	if (a == NULL || b == NULL) {
		SET_ERROR_MESSAGE();
		return NULL;
	}

	if (a->sdtype != b->sdtype) {
		SET_ERROR_MESSAGE();
		return NULL;
	}

	if (a->sdtype == Bool) {
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
	
    NdArray *result = ndarray_create(dest_nd, dest_dimensions, itemsize_cast_by_sdtype(restype), restype);
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

		if (value_b == 0.0) {
			// ゼロ除算処理を必ず書くこと
			return NULL;
		}

		double value = op(value_a, value_b);

		cast(result->data + f * result->itemsize, value);
	}
    ndarray_free(view_a);
    ndarray_free(view_b);
    return result;
}

static NdArray*
alltype_operatoroverloading(NdArray *a, NdArray *b, bool (*op)(double, double))
{
	if (a == NULL || b == NULL) {
		SET_ERROR_MESSAGE();
		return NULL;
	}

	if (a->sdtype != b->sdtype) {
		SET_ERROR_MESSAGE();
		return NULL;
	}
	
	if (!check_broadcastable(a, b->nd, b->dimensions)) {//check_broadcastable(NdArray *src, int dest_nd, int64_t *dest_dimensions)
		SET_ERROR_MESSAGE();
		return NULL;
	}

	int dest_nd = 0;
	int64_t dest_dimensions[64];
	assign_broadcastingshape(a, b, &dest_nd, dest_dimensions); //assign_broadcastingshape(NdArray *a, NdArray *b, int *out_nd, int64_t *out_dimensions)
	
    NdArray *result = ndarray_create(dest_nd, dest_dimensions, sizeof(bool), Bool);
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

		bool value = op(value_a, value_b);

		cast(result->data + f * result->itemsize, value);
	}
    ndarray_free(view_a);
    ndarray_free(view_b);
    return result;
}

static NdArray*
boolean_operatoroverloading(NdArray *a, NdArray *b, bool (*op)(bool, bool))
{
	if (a == NULL || b == NULL) {
		SET_ERROR_MESSAGE();
		return NULL;
	}

	if (a->sdtype != b->sdtype) {
		SET_ERROR_MESSAGE();
		return NULL;
	}

	if (a->sdtype != Bool) {
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
	
    NdArray *result = ndarray_create(dest_nd, dest_dimensions, sizeof(bool), Bool);
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
		
		bool value_a;
		bool value_b;
		memcpy(&value_a, adress_a, sizeof(bool));
		memcpy(&value_b, adress_b, sizeof(bool));
		bool value = op(value_a, value_b);

		cast(result->data + f * result->itemsize, value);
	}
    ndarray_free(view_a);
    ndarray_free(view_b);
    return result;
}

static NdArray*
scalar_operatoroverloading(NdArray *a, double (*op)(double))
{
	if (a == NULL) {
		SET_ERROR_MESSAGE();
		return NULL;
	}
	
    NdArray *result = ndarray_create(a->nd, a->dimensions, a->itemsize, a->sdtype);
    if (result == NULL) {
		return NULL;
	}

	DScalarCast cast = dscalar_cast_by_sdtype[result->sdtype];
	if (cast == NULL) {
		SET_ERROR_MESSAGE();
		return NULL;
	}

    int64_t total = get_totalelements(result->nd, result->dimensions);
    for (int64_t f = 0; f < total; f++) {
		double value = address_to_double(a->data + f * a->itemsize, a->sdtype);
		cast(result->data + f * result->itemsize, op(value));
	}
    return result;
}