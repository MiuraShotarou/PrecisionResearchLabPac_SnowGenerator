#include <stdbool.h>
#include "error.h"

//typedef struct {
//    char    *data;          // 実データへのポインタ
//    int      nd;            // 次元数
//    int64_t *dimensions;   // 各次元のサイズ
//    int64_t *strides;      // 各次元でステップする際ののバイト数 → 転置ができる
//    int   itemsize;      // 1要素のバイト数 → 実質データ型でありstridesでもある
//} NdArray;

 /* scalar to ndarray */
static NdArray *
np_asarray(void *value, SDType sdtype)
{
	// NULL チェック

	int itemsize = itemsize_cast_by_sdtype(sdtype);
	NdArray *result = ndarray_convert(value, NDARRAY_MIN_ND, NDARRAY_MIN_DIMENSIONS, itemsize, sdtype);

	if (!check_scalar(result)) {
		return NULL;
	}
	
	return result;
}

 /*  */
static NdArray *
np_zeros(int64_t *size, int size_nd, SDtype sdtype)//order='C'C言語, 'F'Fotran 内部でfullを呼んでも良い
{
    int itemsize = itemsize_cast_by_sdtype(sdtype);
    
    NdArray *array = ndarray_create(size_nd, size, itemsize);
    if (array == NULL) {
		return NULL;
	}
    
    return array;
}
 /*  */
static NdArray *
np_ones(int64_t *size, int size_nd, SDType sdtype)//order='C'C言語, 'F'Fotran
{
    double value = 1.0;
    NdArray *array = np_full(size, size_nd, value, sdtype);
    return array;
}

/*  */
static NdArray *
np_full(int64_t *size, int size_nd, double value, SDType sdtype, char order)
{
    int itemsize = itemsize_cast_by_sdtype(sdtype);
    
    NdArray *array = ndarray_create(size_nd, size, itemsize);
    
    int64_t total = 1;
    for (int i = 0; i < size_nd; i++) {
        total *= size[i];
    }
    
    DScalarCast cast = dscalar_cast_by_sdtype[sdtype];
    
    for (int64_t i = 0; i < total; i++) {
        cast(array->data + i * itemsize, value);
    }
    return array;
}

// エラー条件を検出する必要
/*  */
static NdArray *
np_arange(int start, int end, int step, SDType sdtype, char order)
{
	int nd = 1;
	int64_t dimensions[1] = {(end - start) / step};
    int itemsize = itemsize_cast_by_sdtype(sdtype);
    if (itemsize == -1) {
        return NULL;
    }

	NdArray *array = ndarray_create(nd, dimensions, itemsize);
    if (array == NULL) {
		return NULL;
	}
    
    DoubleScalarCast doublescalarcast = doublescalar_cast_by_sdtype[sdtype];
    if (doublescalarcast == NULL) {
        return NULL;
    }
	for (int i = 0; i < dimensions[0]; i++) {
		double value = (double)(start + i * step); //型をitemsizeに合わせる必要あり
        doublescalarcast(array->data + i * itemsize, value);
	}
    
    return array;
}

/* */ //スカラーは実装しない（System.Random.Rangeがあるため）
static NdArray *
np_random_choice(NdArray *src, int64_t *size, int size_nd, bool replace, float *p, int p_len, SDType sdtype) //IntPtr src, long[] size, int size_length, bool replace, float[] p, int p_length, SDType sdtype
{
    /* 総要素数を計算 */
    size_t src_n = 1;
    for (int i = 0; i < src->nd; i++) {
        src_n *= (size_t)src->dimensions[i];
    }

    /* 出力配列の総要素数を計算 */
    size_t res_n = 1;
    for (int i = 0; i < size_nd; i++) {
        res_n *= (size_t)size[i];
    }

    /* 出力配列を作成 */
    int itemsize = itemsize_cast_by_sdtype(sdtype);
    if (itemsize == -1) {
        return NULL;
    }

    NdArray *result = ndarray_create(size_nd, size, itemsize);
    if (result == NULL) {
        return NULL;
    }

    /* 重複なし（replace=false）の場合、res_n <= src_n であること */ //構造的に不可な場合
    if (!replace && res_n > src_n) {
        ndarray_free(result);
        SET_ERROR_MESSAGE("np_random_choice: Cannot take a larger sample than population when replace=false.");
        return NULL;
    }

    /* インデックスプール（重複なしの場合に使用） */
    size_t *pool = NULL;
    if (!replace) {
        pool = (size_t *)malloc(sizeof(size_t) * src_n);
        if (pool == NULL) {
            ndarray_free(result);
            return NULL;
        }
        for (size_t i = 0; i < src_n; i++) pool[i] = i;
    }

    /* 各要素をランダムに選択してコピー */
    for (size_t i = 0; i < res_n; i++) {
        size_t idx;

        if (p != NULL) {
            /* 確率指定あり（replace関係なく確率で選択） */
            float rnd = (float)rand() / (float)RAND_MAX;
            float cumsum = 0.0f;
            idx = src_n - 1;
            for (size_t j = 0; j < src_n; j++) {
                cumsum += p[j];
                if (rnd <= cumsum) {
                    idx = j;
                    break;
                }
            }
        } else {
            /* 均等確率 */
            idx = (size_t)rand() % src_n;
        }

        if (!replace) {
            /* 重複なし：選択済みインデックスをプールから除外 */
            // poolを使ってidxをスワップ
            idx = pool[idx % (src_n - i)];
            pool[idx] = pool[src_n - i - 1];
        }

        memcpy(
            result->data + i * itemsize,
            src->data + idx * src->itemsize,
            itemsize
        );
    }
    if (pool != NULL) free(pool);
    return result;
}

static double
np_sum_return_scalar(NdArray *src, SDType srctype) //引数のsdtypeを削除予定
{
    NdArray *cast_array = np_ndarray_cast(src, srctype, Double);
    if (cast_array == NULL) {
        SET_ERROR_MESSAGE("");
        return 0.0;
    };

    int64_t total = get_totalelements(cast_array->dimensions, cast_array->nd);
    double result = 0.0;
    double *data = (double *)cast_array->data;

    for (int64_t i = 0; i < total; i++) {
        result += data[i];
    }

    ndarray_free(cast_array);
    return result;
}

static NdArray *
np_sum_return_array(NdArray *src, SDType srctype, SDType restype, int32_t axis, bool keepdims) //引数のsdtypeを削除予定
{
    /* conditions scalar */
    if (axis == -1) {
        double scalar = np_sum_return_scalar(src, srctype);

        int64_t dims[1] = { 1 };
        NdArray* result = np_full(dims, 1, scalar, restype, 'C');
        if (result == NULL) {
            SET_ERROR_MESSAGE("");
            return NULL;
        }
        
        return result;
    }
    
    /* create cast_array */
    NdArray *cast_array = np_ndarray_cast(src, srctype, Double);
    if (cast_array == NULL) {
        SET_ERROR_MESSAGE("");
        return NULL;
    };
    
    /* conditions error */
    if (axis < 0 || axis >= cast_array->nd) {
        SET_ERROR_MESSAGE_ARGUMENT("np_sum_return_array: axis %d is out of range.", axis);
        ndarray_free(cast_array);
        return NULL;
    }
    
    /* copy dimensions */
    int res_nd = keepdims ? cast_array->nd : cast_array->nd - 1;
    int64_t res_dims[64];
    int res_idx = 0;
    for (int i = 0; i < cast_array->nd; i++) { //nd == 4, axis == 1
        if (i == axis) {
            if (keepdims) //次元を保持するなら
            {
                res_dims[res_idx++] = 1; //dimensionsの要素ではなく、1を代入する。axisに該当する次元の要素数を1にする
            }
        } else {
            res_dims[res_idx++] = cast_array->dimensions[i]; //cast_array->dimensionsの各要素をres_dimsにコピー
        }
    }
    
    int itemsize = itemsize_cast_by_sdtype(restype);
    if (itemsize == -1) {
        ndarray_free(cast_array);
        return NULL;
    }
    
    NdArray *result = ndarray_create(res_nd, res_dims, itemsize);
    if (result == NULL) {
        ndarray_free(cast_array);
        return NULL;
    }
    
    /* azis方向に計算 */
    int64_t outer = 1, inner = 1;
    for (int i = 0; i < axis; i++) { // dims(2, 4, 3), axis == 1
        outer *= cast_array->dimensions[i];
    }
    for (int i = axis + 1; i < cast_array->nd; i++) {
        inner *= cast_array->dimensions[i];
    }
    int64_t axis_len = cast_array->dimensions[axis];
    
    double *src_data = (double *)cast_array->data;
    DScalarCast cast = dscalar_cast_by_sdtype[restype];
    if (cast == NULL) {
        SET_ERROR_MESSAGE("");
        ndarray_free(cast_array);
        ndarray_free(result);
        return NULL;
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
}

static NdArray*
np_pad(NdArray *src, int32_t pad_width, PadModeType mode, double value, SDType restype)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_pad: src is NULL.");
        return NULL;
    }

    /* 出力配列の形状を計算 */
    int src_nd = src->nd;
    int64_t res_dims[64];
    for (int i = 0; i < src_nd; i++) {
        res_dims[i] = src->dimensions[i] + (int64_t)pad_width * 2; //加算のみで良いのか？
    }

    /* 出力配列を生成 */
    NdArray *result = np_full(res_dims, src_nd, value, restype); //最初からfullとかで良い気がする long[] size, int size_nd, double value, SDType sdtype
    if (result == NULL) {
        SET_ERROR_MESSAGE("np_pad: ndarray_create failed.");
        return NULL;
    }
    
    int64_t src_total = get_totalelements(src->dimensions, src_nd);
    for (int64_t idx = 0; idx < src_total; idx++) {
        /* src のフラットインデックスから各次元のインデックスを計算 */
        int64_t res_idx = 0;
        int64_t tmp = idx;
        int64_t stride = 1;

        for (int d = src_nd - 1; d >= 0; d--) {
            int64_t dim_idx = tmp % src->dimensions[d];  // d次元のインデックス
            tmp /= src->dimensions[d];
            res_idx += (dim_idx + pad_width) * stride;
            stride *= res_dims[d];
        }

        memcpy(
            result->data + res_idx * src->itemsize,
            src->data    + idx    * src->itemsize,
            src->itemsize
        );
    }
    return result;
}

static NdArray*
np_where(NdArray *conditions, NdArray *a, NdArray *b)
{
	if (conditions == NULL) {
		SET_ERROR_MESSAGE("np_where: src is NULL.");
		return NULL;
	}
	if (conditions->sdtype != Bool) {
		SET_ERROR_MESSAGE("np_where: conditions should be Bool.");
		return NULL;
	}

	if (a == NULL && b == NULL) {
		// return conditions tuple
		// condisionsのture部分のindexを取得する
		return get_ndarray_boolndarrayindices(conditions);
	} else if (a != NULL && b != NULL) {
		// return conditions value
		if (a->sdtype != b->sdtype) {//C#のコンパイルで弾かれる条件
			SET_ERROR_MESSAGE("np_where: sdtype mismatch between trueValue and falseValue.");
			return NULL;
		}
		return get_ndarray_where(conditions, a, b);
	} else {
		SET_ERROR_MESSAGE("np_where: a and b must both be specified or both be NULL.");
    	return NULL;
	}
	// 条件にNdArrayが指定された際の処理は、indexingの関数を流用できる → できるとすればboolindexingだけかな。== 演算子ならfancyも利用できるね → operator_overload関数群に処理を移して、indexingはそれを呼び出す設計にすると可読性が上がるかも
	// 条件の部分で、何が引数に渡されてくるかわからない。そう考えると、whereの中で引数の型に合わせて条件分岐する必要がある。→ C#側ですべての条件をNdArray<bool>に変換する。CNative.np_whereではそのNdArray<bool>を引数に受け取り、実装を行う方針で。
	// 条件のみを引数に渡した際は、trueのindicesをタプルに変換して戻り値に返し、第一・第二引数を指定した場合はndarrayを戻り値に返す仕様。→ 条件のみの場合でもNdArray<T>(indices)を返す仕様にしよう
	// 第一・第二引数は、C#側が必ずNdArrayに変換するため問題ない
}

static NdArray*
get_ndarray_boolndarrayindices(NdArray *conditions)
{
	int64_t indices_count = 0;
	int64_t indices_array[64][64];
	int64_t total = get_totalelements(conditions->nd, conditions->dimensions);
	for (int f = 0; f < total; f++) {
		bool condition;
		memcpy(&condition, conditions->data + f * conditions->itemsize, sizeof(bool));
		if (condition) {
			get_indices(conditions->nd, conditions->dimensions, f, indices_array[indices_count++]);
		}
	}
	NdArray *result = indicesndarray_create((int64_t)conditions->nd, indices_count);
	
	memcpy(result->data, indices_array, result->dimensions[0] * result->dimensions[1] * sizeof(int64_t));

	return result;
}

static NdArray*
indicesndarray_create(int64_t indices_nd, int64_t indices_count)
{
	// ndは固定値, dimenions[0]は計算が必要, [1]はwhereの場合不要。他から呼び出す場合はわからない
	int nd = INDICES_DEFAULT_ND; //
	int64_t dimensions[INDICES_DEFAULT_ND];
	dimensions[0] = indices_count;
	dimensions[1] = indices_nd;
	NdArray *result = ndarray_create(nd, dimensions, sizeof(int64_t), Long);
	return result;
}

static NdArray*
get_ndarray_where(NdArray *conditions, NdArray *a, NdArray *b)
{
	int nd = 0;
	int64_t dimensions[64];
	// conditions,a,bの形状が一致 or 1 であればOk
	NdArray *arrays[] = { conditions, a, b };
	bool valid = checkshape_and_decideshape(arrays, 3, &nd, dimensions);
	if (!valid) {
		// error メッセージ
		return NULL;
	}

	// result用ndarrayの作成
	NdArray *result = ndarray_create(nd, dimensions, a->itemsize, a->sdtype);
	// a,b指定がある場合のwhereの計算処理
	int64_t total = get_totalelements(conditions->nd, conditions->dimensions);
	for (int f = 0; f < total; f++) {
		bool condition;
		memcpy(&condition, conditions->data + f * conditions->itemsize, sizeof(bool));
		// resultへの代入
		if (condition) {
			memcpy(result->data + f * result->itemsize, check_scalar(a)? a->data : a->data + f * a->itemsize, result->itemsize);
		} else {
			memcpy(result->data + f * result->itemsize, check_scalar(b)? b->data : b->data + f * b->itemsize, result->itemsize);
		}
	}
	return result;
}

static bool
checkshape_and_decideshape(NdArray **arrays, int array_count, int *out_nd, int64_t *out_dimensions) //形状の完全一致 or スカラーを許容
{
	int ref_nd = 1;
	int64_t ref_dims[64];
    memset(ref_dims, 0, sizeof(ref_dims));
	ref_dims[0] = 1;
	// 基準となる形状を取得
    for (int i = 0; i < array_count; i++) {
        if (check_scalar(arrays[i])) {
			continue; // scalar is skip
		}
        ref_nd = arrays[i]->nd;
        memcpy(ref_dims, arrays[i]->dimensions, sizeof(int64_t) * arrays[i]->nd);
        break;
    }
    // 形状チェック
    for (int i = 0; i < array_count; i++) {
        if (check_scalar(arrays[i])) {
			 continue;
		}
        if (arrays[i]->nd != ref_nd) {
            SET_ERROR_MESSAGE("check_shape: nd mismatch.");
            return false;
        }
        for (int d = 0; d < ref_nd; d++) {
            if (arrays[i]->dimensions[d] != ref_dims[d]) {
                SET_ERROR_MESSAGE("check_shape: dimensions mismatch.");
                return false;
            }
        }
    }
	*out_nd = ref_nd;
	memcpy(out_dimensions, ref_dims, sizeof(int64_t) * ref_nd);
    return true;
}

static NdArray*
np_broadcast_to(NdArray *src, int64_t *dist_dimensions, int dist_nd)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("np_broadcast_to: src is NULL.");
        return NULL;
    }
    
    /* check can broadcast */
    if (!)
    
    /* create result ndarray */
    NdArray *result = ndarray_copy(src);
    
    /* add dimensions */
    if (src->nd < dist_nd) {
        result->nd = dist_nd;
        assgin_
    } else if ()
    /* adjust strides */
    
    /*  */
    assign_broadcastingstrides(int nd, int64_t *src->dimensions, result->strides)
}