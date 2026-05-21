// エラー条件 → 型の不一致
// T == bool の場合
// 整数除算 → floatへ昇格
// ゼロ除算 → <math.h> INFINITY を返す。
// 形状の不一致 → ①次元数が異なる場合、小さい方の先頭に1を補う。(3,) → (1, 3), ②各次元のサイズが異なる場合、どちらかが1であれば1の方を引き伸ばす。(2, 3) と (1, 3) → (2, 3) に引き伸ばす。③どちらも1でなく且つサイズが異なる場合はエラー。(2, 3) と (2, 4) → エラー ※NumCSharpでは戻り値の形状を、必ず右辺の式にある配列の中で最大のものと合わせる仕様にする
// 小数同士の除算 → fmodの結果

static NdArray* 
np_add (NdArray *a, NdArray *b) { return np_elementwise(a, b, op_add);}
static NdArray*
np_subtract (NdArray *a, NdArray *b) { return np_elementwise(a, b, op_subtract);}
static NdArray*
np_multiply (NdArray *a, NdArray *b) { return np_elementwise(a, b, op_multiply);}
static NdArray*
np_divide (NdArray *a, NdArray *b) { return np_elementwise(a, b, op_divide);}
static NdArray*
np_modulo (NdArray *a, NdArray *b) { return np_elementwise(a, b, op_modulo);}

static NdArray*
np_(NdArray *a, NdArray *b, double (*op)(double, double))
{
    
    
    int nd = 0;
    int64_t dimensions[64];
    

    // result生成 (a の sdtype に合わせる)
    NdArray *result = ndarray_create(nd, dimensions, a->itemsize, a->sdtype);
    if (result == NULL) return NULL;

    int64_t total = get_totalelements(dimensions, nd);
    DScalarCast cast = dscalar_cast_by_sdtype[a->sdtype];
    if (cast == NULL) return NULL;

    // キャスト用
    NdArray *cast_a = np_ndarray_cast(a, Double);
    NdArray *cast_b = np_ndarray_cast(b, Double);
    if (cast_a == NULL || cast_b == NULL) {
        ndarray_free(cast_a);
        ndarray_free(cast_b);
        ndarray_free(result);
        return NULL;
    }

    double *data_a = (double *)cast_a->data;
    double *data_b = (double *)cast_b->data;

    for (int64_t i = 0; i < total; i++) {
        double val_a = check_scalar(cast_a) ? data_a[0] : data_a[i];
        double val_b = check_scalar(cast_b) ? data_b[0] : data_b[i];
        double res   = op(val_a, val_b);
        cast(result->data + i * result->itemsize, res);
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