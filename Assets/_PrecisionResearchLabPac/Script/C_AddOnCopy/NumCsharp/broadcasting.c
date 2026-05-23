/* Broadcasting */
static bool
check_broadcastable(NdArray *src, int dest_nd, int64_t *dest_dimensions) {
    if (src == NULL) {
        SET_ERROR_MESSAGE(("src is NULL");
        return false;
    }
    
    if (src->nd > dest_nd ) { //ブロードキャストの意味がない
        SET_ERROR_MESSAGE("src_nd is greater than dest_nd");
        return false;
    }
    // must src->nd =< dest->nd
    int offset = dest_nd - src->nd;
    for (int d = 0; d < src->nd; d++) {
        if (src->dimensions[d] > dest_dimensions[d + offset]) {
            SET_ERROR_MESSAGE("");
            return false;
        }
    }
}

static void
assign_broadcastingstrides(int nd, int64_t *src_dimensions, int64_t *dest_dimensions, int64_t *out_strides)
{
    for (int d = 0; d < nd; d++) {
        if (src_dimensions[d] == 1) {
            otu_strides[d] = 0;
        }
    }
}

static void
assign_broadcastingshape(int64_t *src_out_dimensions, int64_t *dest_dimensions, int *dest_nd) {
    // nd のインクリメント
    // dimensions の設定
}

static void
assign_broadcastingshape(NdArray** arrays, int array_count, int *out_nd, int64_t *out_dimensions) //2つのNdArrayを比較して、大きいほうの形状だけ採用する単純なメソッドに変更したい
{
    int nd = 0;
    for (int i = 0; i < array_count; i++) {
        if (nd < arrays[i]->nd) {
            nd = arrays[i]->nd;
        }
    }
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    memset(dimensions, 0, sizeof(dimensions));
    for (int i = 0; i < array_count; i++) {
        int offset = nd > arrays[i]->nd ? nd - arrays[i]->nd : 0;
        for (int d = 0; d < arrays[i]->nd; d++) {
            if (dimensions[d + offset] < arrays[i]->dimensions[d]) {
                dimensions[d + offset] = arrays[i]->dimensions[d];
            }
        }
    }
    *out_nd = nd;
    memcpy(out_dimensions, dimensions, sizeof(int64_t) * nd);
}