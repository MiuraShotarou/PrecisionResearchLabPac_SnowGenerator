// operator.c
// DoubleScalarCastを利用して全ての値をdoubleに統一して計算するコードから、C#のジェネリクスで同一の型同士でしか演算されない仕様を利用し、すべてのプリミティブに対応する演算処理を実装した
typedef void (*BinaryOp)(void *a, void *b, void *result);
typedef void (*UnaryOp) (void *a, void *result);
typedef bool (*BinaryBoolOp)(void *a, void *b);

static void op_add_sbyte (void *a, void *b, void *r) { *(int8_t   *)r = *(int8_t   *)a + *(int8_t   *)b; }
static void op_add_byte  (void *a, void *b, void *r) { *(uint8_t  *)r = *(uint8_t  *)a + *(uint8_t  *)b; }
static void op_add_short (void *a, void *b, void *r) { *(int16_t  *)r = *(int16_t  *)a + *(int16_t  *)b; }
static void op_add_ushort(void *a, void *b, void *r) { *(uint16_t *)r = *(uint16_t *)a + *(uint16_t *)b; }
static void op_add_int   (void *a, void *b, void *r) { *(int32_t  *)r = *(int32_t  *)a + *(int32_t  *)b; }
static void op_add_uint  (void *a, void *b, void *r) { *(uint32_t *)r = *(uint32_t *)a + *(uint32_t *)b; }
static void op_add_long  (void *a, void *b, void *r) { *(int64_t  *)r = *(int64_t  *)a + *(int64_t  *)b; }
static void op_add_ulong (void *a, void *b, void *r) { *(uint64_t *)r = *(uint64_t *)a + *(uint64_t *)b; }
static void op_add_float (void *a, void *b, void *r) { *(float    *)r = *(float    *)a + *(float    *)b; }
static void op_add_double(void *a, void *b, void *r) { *(double   *)r = *(double   *)a + *(double   *)b; }

static BinaryOp op_add_by_sdtype[] = {
    NULL,          // Bool
    op_add_sbyte,  // SByte
    op_add_byte,   // Byte
    op_add_short,  // Short
    op_add_ushort, // UShort
    op_add_int,    // Int
    op_add_uint,   // UInt
    op_add_long,   // Long
    op_add_ulong,  // ULong
    op_add_float,  // Float
    op_add_double, // Double
};

static void op_sub_sbyte (void *a, void *b, void *r) { *(int8_t   *)r = *(int8_t   *)a - *(int8_t   *)b; }
static void op_sub_byte  (void *a, void *b, void *r) { *(uint8_t  *)r = *(uint8_t  *)a - *(uint8_t  *)b; }
static void op_sub_short (void *a, void *b, void *r) { *(int16_t  *)r = *(int16_t  *)a - *(int16_t  *)b; }
static void op_sub_ushort(void *a, void *b, void *r) { *(uint16_t *)r = *(uint16_t *)a - *(uint16_t *)b; }
static void op_sub_int   (void *a, void *b, void *r) { *(int32_t  *)r = *(int32_t  *)a - *(int32_t  *)b; }
static void op_sub_uint  (void *a, void *b, void *r) { *(uint32_t *)r = *(uint32_t *)a - *(uint32_t *)b; }
static void op_sub_long  (void *a, void *b, void *r) { *(int64_t  *)r = *(int64_t  *)a - *(int64_t  *)b; }
static void op_sub_ulong (void *a, void *b, void *r) { *(uint64_t *)r = *(uint64_t *)a - *(uint64_t *)b; }
static void op_sub_float (void *a, void *b, void *r) { *(float    *)r = *(float    *)a - *(float    *)b; }
static void op_sub_double(void *a, void *b, void *r) { *(double   *)r = *(double   *)a - *(double   *)b; }

static BinaryOp op_sub_by_sdtype[] = {
    NULL, op_sub_sbyte, op_sub_byte, op_sub_short, op_sub_ushort,
    op_sub_int, op_sub_uint, op_sub_long, op_sub_ulong, op_sub_float, op_sub_double,
};

// ----------------------------------------------------------------
// 乗算
// ----------------------------------------------------------------
static void op_mul_sbyte (void *a, void *b, void *r) { *(int8_t   *)r = *(int8_t   *)a * *(int8_t   *)b; }
static void op_mul_byte  (void *a, void *b, void *r) { *(uint8_t  *)r = *(uint8_t  *)a * *(uint8_t  *)b; }
static void op_mul_short (void *a, void *b, void *r) { *(int16_t  *)r = *(int16_t  *)a * *(int16_t  *)b; }
static void op_mul_ushort(void *a, void *b, void *r) { *(uint16_t *)r = *(uint16_t *)a * *(uint16_t *)b; }
static void op_mul_int   (void *a, void *b, void *r) { *(int32_t  *)r = *(int32_t  *)a * *(int32_t  *)b; }
static void op_mul_uint  (void *a, void *b, void *r) { *(uint32_t *)r = *(uint32_t *)a * *(uint32_t *)b; }
static void op_mul_long  (void *a, void *b, void *r) { *(int64_t  *)r = *(int64_t  *)a * *(int64_t  *)b; }
static void op_mul_ulong (void *a, void *b, void *r) { *(uint64_t *)r = *(uint64_t *)a * *(uint64_t *)b; }
static void op_mul_float (void *a, void *b, void *r) { *(float    *)r = *(float    *)a * *(float    *)b; }
static void op_mul_double(void *a, void *b, void *r) { *(double   *)r = *(double   *)a * *(double   *)b; }

static BinaryOp op_mul_by_sdtype[] = {
    NULL, op_mul_sbyte, op_mul_byte, op_mul_short, op_mul_ushort,
    op_mul_int, op_mul_uint, op_mul_long, op_mul_ulong, op_mul_float, op_mul_double,
};

// ----------------------------------------------------------------
// 除算
// ----------------------------------------------------------------
static void op_div_sbyte (void *a, void *b, void *r) { *(int8_t   *)r = *(int8_t   *)a / *(int8_t   *)b; }
static void op_div_byte  (void *a, void *b, void *r) { *(uint8_t  *)r = *(uint8_t  *)a / *(uint8_t  *)b; }
static void op_div_short (void *a, void *b, void *r) { *(int16_t  *)r = *(int16_t  *)a / *(int16_t  *)b; }
static void op_div_ushort(void *a, void *b, void *r) { *(uint16_t *)r = *(uint16_t *)a / *(uint16_t *)b; }
static void op_div_int   (void *a, void *b, void *r) { *(int32_t  *)r = *(int32_t  *)a / *(int32_t  *)b; }
static void op_div_uint  (void *a, void *b, void *r) { *(uint32_t *)r = *(uint32_t *)a / *(uint32_t *)b; }
static void op_div_long  (void *a, void *b, void *r) { *(int64_t  *)r = *(int64_t  *)a / *(int64_t  *)b; }
static void op_div_ulong (void *a, void *b, void *r) { *(uint64_t *)r = *(uint64_t *)a / *(uint64_t *)b; }
static void op_div_float (void *a, void *b, void *r) { *(float    *)r = *(float    *)a / *(float    *)b; }
static void op_div_double(void *a, void *b, void *r) { *(double   *)r = *(double   *)a / *(double   *)b; }

static BinaryOp op_div_by_sdtype[] = {
    NULL, op_div_sbyte, op_div_byte, op_div_short, op_div_ushort,
    op_div_int, op_div_uint, op_div_long, op_div_ulong, op_div_float, op_div_double,
};

// ----------------------------------------------------------------
// 剰余
// ----------------------------------------------------------------
static void op_mod_sbyte (void *a, void *b, void *r) { *(int8_t   *)r = *(int8_t   *)a % *(int8_t   *)b; }
static void op_mod_byte  (void *a, void *b, void *r) { *(uint8_t  *)r = *(uint8_t  *)a % *(uint8_t  *)b; }
static void op_mod_short (void *a, void *b, void *r) { *(int16_t  *)r = *(int16_t  *)a % *(int16_t  *)b; }
static void op_mod_ushort(void *a, void *b, void *r) { *(uint16_t *)r = *(uint16_t *)a % *(uint16_t *)b; }
static void op_mod_int   (void *a, void *b, void *r) { *(int32_t  *)r = *(int32_t  *)a % *(int32_t  *)b; }
static void op_mod_uint  (void *a, void *b, void *r) { *(uint32_t *)r = *(uint32_t *)a % *(uint32_t *)b; }
static void op_mod_long  (void *a, void *b, void *r) { *(int64_t  *)r = *(int64_t  *)a % *(int64_t  *)b; }
static void op_mod_ulong (void *a, void *b, void *r) { *(uint64_t *)r = *(uint64_t *)a % *(uint64_t *)b; }
static void op_mod_float (void *a, void *b, void *r) { *(float    *)r = fmodf(*(float *)a, *(float *)b); }
static void op_mod_double(void *a, void *b, void *r) { *(double   *)r = fmod (*(double*)a, *(double*)b); }

static BinaryOp op_mod_by_sdtype[] = {
    NULL, op_mod_sbyte, op_mod_byte, op_mod_short, op_mod_ushort,
    op_mod_int, op_mod_uint, op_mod_long, op_mod_ulong, op_mod_float, op_mod_double,
};

// ----------------------------------------------------------------
// 比較演算（戻り値はbool）
// ----------------------------------------------------------------
static bool op_less_sbyte (void *a, void *b) { return *(int8_t   *)a <  *(int8_t   *)b; }
static bool op_less_byte  (void *a, void *b) { return *(uint8_t  *)a <  *(uint8_t  *)b; }
static bool op_less_short (void *a, void *b) { return *(int16_t  *)a <  *(int16_t  *)b; }
static bool op_less_ushort(void *a, void *b) { return *(uint16_t *)a <  *(uint16_t *)b; }
static bool op_less_int   (void *a, void *b) { return *(int32_t  *)a <  *(int32_t  *)b; }
static bool op_less_uint  (void *a, void *b) { return *(uint32_t *)a <  *(uint32_t *)b; }
static bool op_less_long  (void *a, void *b) { return *(int64_t  *)a <  *(int64_t  *)b; }
static bool op_less_ulong (void *a, void *b) { return *(uint64_t *)a <  *(uint64_t *)b; }
static bool op_less_float (void *a, void *b) { return *(float    *)a <  *(float    *)b; }
static bool op_less_double(void *a, void *b) { return *(double   *)a <  *(double   *)b; }

static BinaryBoolOp op_less_by_sdtype[] = {
    NULL, op_less_sbyte, op_less_byte, op_less_short, op_less_ushort,
    op_less_int, op_less_uint, op_less_long, op_less_ulong, op_less_float, op_less_double,
};

static bool op_less_equal_sbyte (void *a, void *b) { return *(int8_t   *)a <= *(int8_t   *)b; }
static bool op_less_equal_byte  (void *a, void *b) { return *(uint8_t  *)a <= *(uint8_t  *)b; }
static bool op_less_equal_short (void *a, void *b) { return *(int16_t  *)a <= *(int16_t  *)b; }
static bool op_less_equal_ushort(void *a, void *b) { return *(uint16_t *)a <= *(uint16_t *)b; }
static bool op_less_equal_int   (void *a, void *b) { return *(int32_t  *)a <= *(int32_t  *)b; }
static bool op_less_equal_uint  (void *a, void *b) { return *(uint32_t *)a <= *(uint32_t *)b; }
static bool op_less_equal_long  (void *a, void *b) { return *(int64_t  *)a <= *(int64_t  *)b; }
static bool op_less_equal_ulong (void *a, void *b) { return *(uint64_t *)a <= *(uint64_t *)b; }
static bool op_less_equal_float (void *a, void *b) { return *(float    *)a <= *(float    *)b; }
static bool op_less_equal_double(void *a, void *b) { return *(double   *)a <= *(double   *)b; }

static BinaryBoolOp op_less_equal_by_sdtype[] = {
    NULL, op_less_equal_sbyte, op_less_equal_byte, op_less_equal_short, op_less_equal_ushort,
    op_less_equal_int, op_less_equal_uint, op_less_equal_long, op_less_equal_ulong, op_less_equal_float, op_less_equal_double,
};

static bool op_greater_sbyte (void *a, void *b) { return *(int8_t   *)a >  *(int8_t   *)b; }
static bool op_greater_byte  (void *a, void *b) { return *(uint8_t  *)a >  *(uint8_t  *)b; }
static bool op_greater_short (void *a, void *b) { return *(int16_t  *)a >  *(int16_t  *)b; }
static bool op_greater_ushort(void *a, void *b) { return *(uint16_t *)a >  *(uint16_t *)b; }
static bool op_greater_int   (void *a, void *b) { return *(int32_t  *)a >  *(int32_t  *)b; }
static bool op_greater_uint  (void *a, void *b) { return *(uint32_t *)a >  *(uint32_t *)b; }
static bool op_greater_long  (void *a, void *b) { return *(int64_t  *)a >  *(int64_t  *)b; }
static bool op_greater_ulong (void *a, void *b) { return *(uint64_t *)a >  *(uint64_t *)b; }
static bool op_greater_float (void *a, void *b) { return *(float    *)a >  *(float    *)b; }
static bool op_greater_double(void *a, void *b) { return *(double   *)a >  *(double   *)b; }

static BinaryBoolOp op_greater_by_sdtype[] = {
    NULL, op_greater_sbyte, op_greater_byte, op_greater_short, op_greater_ushort,
    op_greater_int, op_greater_uint, op_greater_long, op_greater_ulong, op_greater_float, op_greater_double,
};

static bool op_greater_equal_sbyte (void *a, void *b) { return *(int8_t   *)a >= *(int8_t   *)b; }
static bool op_greater_equal_byte  (void *a, void *b) { return *(uint8_t  *)a >= *(uint8_t  *)b; }
static bool op_greater_equal_short (void *a, void *b) { return *(int16_t  *)a >= *(int16_t  *)b; }
static bool op_greater_equal_ushort(void *a, void *b) { return *(uint16_t *)a >= *(uint16_t *)b; }
static bool op_greater_equal_int   (void *a, void *b) { return *(int32_t  *)a >= *(int32_t  *)b; }
static bool op_greater_equal_uint  (void *a, void *b) { return *(uint32_t *)a >= *(uint32_t *)b; }
static bool op_greater_equal_long  (void *a, void *b) { return *(int64_t  *)a >= *(int64_t  *)b; }
static bool op_greater_equal_ulong (void *a, void *b) { return *(uint64_t *)a >= *(uint64_t *)b; }
static bool op_greater_equal_float (void *a, void *b) { return *(float    *)a >= *(float    *)b; }
static bool op_greater_equal_double(void *a, void *b) { return *(double   *)a >= *(double   *)b; }

static BinaryBoolOp op_greater_equal_by_sdtype[] = {
    NULL, op_greater_equal_sbyte, op_greater_equal_byte, op_greater_equal_short, op_greater_equal_ushort,
    op_greater_equal_int, op_greater_equal_uint, op_greater_equal_long, op_greater_equal_ulong, op_greater_equal_float, op_greater_equal_double,
};

static bool op_equal_sbyte (void *a, void *b) { return *(int8_t   *)a == *(int8_t   *)b; }
static bool op_equal_byte  (void *a, void *b) { return *(uint8_t  *)a == *(uint8_t  *)b; }
static bool op_equal_short (void *a, void *b) { return *(int16_t  *)a == *(int16_t  *)b; }
static bool op_equal_ushort(void *a, void *b) { return *(uint16_t *)a == *(uint16_t *)b; }
static bool op_equal_int   (void *a, void *b) { return *(int32_t  *)a == *(int32_t  *)b; }
static bool op_equal_uint  (void *a, void *b) { return *(uint32_t *)a == *(uint32_t *)b; }
static bool op_equal_long  (void *a, void *b) { return *(int64_t  *)a == *(int64_t  *)b; }
static bool op_equal_ulong (void *a, void *b) { return *(uint64_t *)a == *(uint64_t *)b; }
static bool op_equal_float (void *a, void *b) { return *(float    *)a == *(float    *)b; }
static bool op_equal_double(void *a, void *b) { return *(double   *)a == *(double   *)b; }
static bool op_equal_bool  (void *a, void *b) { return *(bool     *)a == *(bool     *)b; }

static BinaryBoolOp op_equal_by_sdtype[] = {
    op_equal_bool, op_equal_sbyte, op_equal_byte, op_equal_short, op_equal_ushort,
    op_equal_int, op_equal_uint, op_equal_long, op_equal_ulong, op_equal_float, op_equal_double,
};

static bool op_not_equal_sbyte (void *a, void *b) { return *(int8_t   *)a != *(int8_t   *)b; }
static bool op_not_equal_byte  (void *a, void *b) { return *(uint8_t  *)a != *(uint8_t  *)b; }
static bool op_not_equal_short (void *a, void *b) { return *(int16_t  *)a != *(int16_t  *)b; }
static bool op_not_equal_ushort(void *a, void *b) { return *(uint16_t *)a != *(uint16_t *)b; }
static bool op_not_equal_int   (void *a, void *b) { return *(int32_t  *)a != *(int32_t  *)b; }
static bool op_not_equal_uint  (void *a, void *b) { return *(uint32_t *)a != *(uint32_t *)b; }
static bool op_not_equal_long  (void *a, void *b) { return *(int64_t  *)a != *(int64_t  *)b; }
static bool op_not_equal_ulong (void *a, void *b) { return *(uint64_t *)a != *(uint64_t *)b; }
static bool op_not_equal_float (void *a, void *b) { return *(float    *)a != *(float    *)b; }
static bool op_not_equal_double(void *a, void *b) { return *(double   *)a != *(double   *)b; }
static bool op_not_equal_bool  (void *a, void *b) { return *(bool     *)a != *(bool     *)b; }

static BinaryBoolOp op_not_equal_by_sdtype[] = {
    op_not_equal_bool, op_not_equal_sbyte, op_not_equal_byte, op_not_equal_short, op_not_equal_ushort,
    op_not_equal_int, op_not_equal_uint, op_not_equal_long, op_not_equal_ulong, op_not_equal_float, op_not_equal_double,
};

// ----------------------------------------------------------------
// 単項演算
// ----------------------------------------------------------------
static void op_negative_sbyte (void *a, void *r) { *(int8_t  *)r = -*(int8_t  *)a; }
static void op_negative_short (void *a, void *r) { *(int16_t *)r = -*(int16_t *)a; }
static void op_negative_int   (void *a, void *r) { *(int32_t *)r = -*(int32_t *)a; }
static void op_negative_long  (void *a, void *r) { *(int64_t *)r = -*(int64_t *)a; }
static void op_negative_float (void *a, void *r) { *(float   *)r = -*(float   *)a; }
static void op_negative_double(void *a, void *r) { *(double  *)r = -*(double  *)a; }

static UnaryOp op_negative_by_sdtype[] = {
    NULL,              // Bool
    op_negative_sbyte, // SByte
    NULL,              // Byte（符号なしは負にできない）
    op_negative_short, // Short
    NULL,              // UShort
    op_negative_int,   // Int
    NULL,              // UInt
    op_negative_long,  // Long
    NULL,              // ULong
    op_negative_float, // Float
    op_negative_double,// Double
};

// ----------------------------------------------------------------
// 公開関数（呼び出し側）
// ----------------------------------------------------------------
NdArray* np_add          (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, op_add_by_sdtype);          }
NdArray* np_subtract     (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, op_sub_by_sdtype);          }
NdArray* np_multiply     (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, op_mul_by_sdtype);          }
NdArray* np_divide       (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, op_div_by_sdtype);          }
NdArray* np_modulo       (NdArray *a, NdArray *b) { return numeric_operatoroverloading(a, b, op_mod_by_sdtype);          }
NdArray* np_less         (NdArray *a, NdArray *b) { return compare_operatoroverloading(a, b, op_less_by_sdtype);         }
NdArray* np_less_equal   (NdArray *a, NdArray *b) { return compare_operatoroverloading(a, b, op_less_equal_by_sdtype);   }
NdArray* np_greater      (NdArray *a, NdArray *b) { return compare_operatoroverloading(a, b, op_greater_by_sdtype);      }
NdArray* np_greater_equal(NdArray *a, NdArray *b) { return compare_operatoroverloading(a, b, op_greater_equal_by_sdtype);}
NdArray* np_equal        (NdArray *a, NdArray *b) { return compare_operatoroverloading(a, b, op_equal_by_sdtype);        }
NdArray* np_not_equal    (NdArray *a, NdArray *b) { return compare_operatoroverloading(a, b, op_not_equal_by_sdtype);    }
NdArray* np_logical_and  (NdArray *a, NdArray *b) { return boolean_operatoroverloading(a, b, op_logical_and);        }
NdArray* np_logical_or   (NdArray *a, NdArray *b) { return boolean_operatoroverloading(a, b, op_logical_or);         }
NdArray* np_logical_xor  (NdArray *a, NdArray *b) { return boolean_operatoroverloading(a, b, op_logical_xor);        }
NdArray* np_logical_not  (NdArray *a)             { return unary_operatoroverloading  (a,    op_logical_not);        }
NdArray* np_negative     (NdArray *a)             { return unary_operatoroverloading  (a,    op_negative_by_sdtype);     }

// ----------------------------------------------------------------
// 内部処理関数
// ----------------------------------------------------------------
static NdArray*
numeric_operatoroverloading(NdArray *a, NdArray *b, BinaryOp *op_by_sdtype)
{
    NdArray *result = NULL;
    NdArray *view_a = NULL;
    NdArray *view_b = NULL;
    if (a == NULL || b == NULL) {
        SET_ERROR_MESSAGE("numeric_operatoroverloading: a or b is NULL.");
        goto fail;
    }
    if (a->sdtype != b->sdtype) {
        SET_ERROR_MESSAGE("numeric_operatoroverloading: sdtype mismatch.");
        goto fail;
    }
    if (a->sdtype == Bool) {
        SET_ERROR_MESSAGE("numeric_operatoroverloading: Bool type is not supported.");
        goto fail;
    }
    BinaryOp op = op_by_sdtype[a->sdtype];
    if (op == NULL) {
        SET_ERROR_MESSAGE("numeric_operatoroverloading: unsupported sdtype.");
        goto fail;
    }
    if (!check_broadcasby_sdtype(a, b->nd, b->dimensions)) {
        SET_ERROR_MESSAGE("numeric_operatoroverloading: shape is not broadcasby_sdtype.");
        goto fail;
    }
    int dest_nd = 0;
    int64_t dest_dimensions[NDARRAY_MAX_DIMENSIONS];
    assign_broadcastingshape(a, b, &dest_nd, dest_dimensions);
    result = ndarray_create(dest_nd, dest_dimensions, a->itemsize, a->sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("numeric_operatoroverloading: result is NULL.");
        goto fail;
    }
    view_a = np_broadcast_to(a, result->dimensions, result->nd);
    view_b = np_broadcast_to(b, result->dimensions, result->nd);
    if (view_a == NULL || view_b == NULL) {
        SET_ERROR_MESSAGE("numeric_operatoroverloading: np_broadcast_to failed.");
        goto fail;
    }
    int64_t total = get_totalelements(result->nd, result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_INDICES];
        assign_indices(result->nd, result->dimensions, f, res_indices);
        char *adress_a = get_address(view_a->data, res_indices, view_a->strides, view_a->nd);
        char *adress_b = get_address(view_b->data, res_indices, view_b->strides, view_b->nd);
        op(adress_a, adress_b, result->data + f * result->itemsize);
    }
    ndarray_free(view_a);
    ndarray_free(view_b);
    return result;
    fail:
        ndarray_free(result);
        ndarray_free(view_a);
        ndarray_free(view_b);
        return NULL;
}

static NdArray*
compare_operatoroverloading(NdArray *a, NdArray *b, BinaryBoolOp *op_by_sdtype)
{
    NdArray *result = NULL;
    NdArray *view_a = NULL;
    NdArray *view_b = NULL;
    if (a == NULL || b == NULL) {
        SET_ERROR_MESSAGE("compare_operatoroverloading: a or b is NULL.");
        goto fail;
    }
    if (a->sdtype != b->sdtype) {
        SET_ERROR_MESSAGE("compare_operatoroverloading: sdtype mismatch.");
        goto fail;
    }
    BinaryBoolOp op = op_by_sdtype[a->sdtype];
    if (op == NULL) {
        SET_ERROR_MESSAGE("compare_operatoroverloading: unsupported sdtype.");
        goto fail;
    }
    if (!check_broadcasby_sdtype(a, b->nd, b->dimensions)) {
        SET_ERROR_MESSAGE("compare_operatoroverloading: shape is not broadcasby_sdtype.");
        goto fail;
    }
    int dest_nd = 0;
    int64_t dest_dimensions[NDARRAY_MAX_DIMENSIONS];
    assign_broadcastingshape(a, b, &dest_nd, dest_dimensions);
    result = ndarray_create(dest_nd, dest_dimensions, sizeof(bool), Bool);
    if (result == NULL) {
        SET_ERROR_MESSAGE("compare_operatoroverloading: result is NULL.");
        goto fail;
    }
    view_a = np_broadcast_to(a, result->dimensions, result->nd);
    view_b = np_broadcast_to(b, result->dimensions, result->nd);
    if (view_a == NULL || view_b == NULL) {
        SET_ERROR_MESSAGE("compare_operatoroverloading: np_broadcast_to failed.");
        goto fail;
    }
    int64_t total = get_totalelements(result->nd, result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        int64_t res_indices[NDARRAY_MAX_INDICES];
        assign_indices(result->nd, result->dimensions, f, res_indices);
        char *adress_a = get_address(view_a->data, res_indices, view_a->strides, view_a->nd);
        char *adress_b = get_address(view_b->data, res_indices, view_b->strides, view_b->nd);
        bool value = op(adress_a, adress_b);
        memcpy(result->data + f * sizeof(bool), &value, sizeof(bool));
    }
    ndarray_free(view_a);
    ndarray_free(view_b);
    return result;
    fail:
        ndarray_free(result);
        ndarray_free(view_a);
        ndarray_free(view_b);
        return NULL;
}

static NdArray*
unary_operatoroverloading(NdArray *a, UnaryOp *op_by_sdtype)
{
    NdArray *result = NULL;
    if (a == NULL) {
        SET_ERROR_MESSAGE("unary_operatoroverloading: a is NULL.");
        goto fail;
    }
    UnaryOp op = op_by_sdtype[a->sdtype];
    if (op == NULL) {
        SET_ERROR_MESSAGE("unary_operatoroverloading: unsupported sdtype.");
        goto fail;
    }
    result = ndarray_create(a->nd, a->dimensions, a->itemsize, a->sdtype);
    if (result == NULL) {
        SET_ERROR_MESSAGE("unary_operatoroverloading: result is NULL.");
        goto fail;
    }
    int64_t total = get_totalelements(result->nd, result->dimensions);
    for (int64_t f = 0; f < total; f++) {
        op(a->data + f * a->itemsize, result->data + f * result->itemsize);
    }
    return result;
    fail:
        ndarray_free(result);
        return NULL;
}