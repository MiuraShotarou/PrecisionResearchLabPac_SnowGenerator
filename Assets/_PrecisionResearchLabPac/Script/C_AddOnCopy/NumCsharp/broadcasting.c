// broadcasting.c
/* Broadcasting */
// 形状一致型のブロードキャストが可能かどうかを判断する
bool
check_broadcastable(NdArray *a, int b_nd, int64_t *b_dimensions) {
    if (a == NULL) {
        SET_ERROR_MESSAGE("a is NULL");
        return false;
    }
    
    int offset = abs(a->nd - b_nd);
    if (a->nd > b_nd) { //Max_Is_A
        for (int d = 0; d < b_nd; d++) {
            if (a->dimensions[d + offset] != b_dimensions[d]) {
                if (a->dimensions[d + offset] != 1 && b_dimensions[d] != 1) {
                    goto fail;
                }
            }
        }
    } 
    else { //Max_Is_B or same
        for (int d = 0; d < a->nd; d++) {
            if (a->dimensions[d] != b_dimensions[d + offset]) {
                if (a->dimensions[d] != 1 && b_dimensions[d + offset] != 1) {
                    goto fail;
                }
            }
        }
    }
    
    return true;
    fail:
        SET_ERROR_MESSAGE("check_broadcastable: shape mismatch.");
        return false;
}
// 形状拡張型のブロードキャストが可能かどうかを判断する
bool
check_broadcastexpand(NdArray *src, int dest_nd, int64_t *dest_dimensions)
{
    if (src == NULL) {
        SET_ERROR_MESSAGE("src is NULL");
        return false;
    }
    
    if (src->nd > dest_nd) {
        goto fail;
    }
    
    int offset = dest_nd - src->nd;
    for (int d = 0; d < src->nd; d++)
    {
        if (src->dimensions[d] > dest_dimensions[d + offset]) {
            goto fail;
        }
        if (src->dimensions[d] < dest_dimensions[d + offset]) {
            if (src->dimensions[d] != 1) {
                goto fail;
            }
        }
    }
    
    return true;
    fail:
        SET_ERROR_MESSAGE("check_broadcastexpand: shape mismatch.");
        return false;
}
// 形状一致型のブロードキャストを行い、結果として確定される形状をnd, dimensionsパラメータで取得する
void
assign_broadcastingshape(NdArray *a, NdArray *b, int *out_nd, int64_t *out_dimensions) {
    if (a == NULL || b == NULL) {
        SET_ERROR_MESSAGE("a or b is NULL");
        goto fail;
    }
    
    /* can broadcast table */
    if (!check_broadcastable(a, b->nd, b->dimensions)) {
        SET_ERROR_MESSAGE("assign_broadcastingshape: shape mismatch.");
        goto fail;
    }
    
    int nd = NDARRAY_MAX(a->nd, b->nd);
    int64_t dimensions[NDARRAY_MAX_DIMENSIONS];
    
    int d = nd - 1;
    int offset = abs(a->nd - b->nd);
    // a or b でサイズが小さい方の配列に合わせ各dimensionsを比較する
    if (a->nd > b->nd) {
        do {
            dimensions[d] = NDARRAY_MAX(a->dimensions[d], b->dimensions[d - offset]);
        } while (d-- > offset);
        do {
            dimensions[d] = a->dimensions[d];
        } while (d-- > 0);
    } 
    else if (a->nd < b->nd) {  //a->nd == 4(0~3), b->nd == 6(0~5)
        do {
            dimensions[d] = NDARRAY_MAX(a->dimensions[d - offset], b->dimensions[d]);
        } while (d-- > offset);
        do {
            dimensions[d] = b->dimensions[d];
        } while (d-- > 0);
    } 
    else {
        do {
            dimensions[d] = NDARRAY_MAX(a->dimensions[d], b->dimensions[d]);
        } while (d-- > 0);
    }
    *out_nd = nd;
    memcpy(out_dimensions, dimensions, sizeof(int64_t) * nd);
    fail:
        return;
}

void //実質、既存ndarrayの拡張操作。view に切り替える
assign_broadcastingstrides(NdArray *src, NdArray *dest, int64_t *out_strides)
{
    if (src == NULL || dest == NULL) {
        SET_ERROR_MESSAGE("assign_broadcastingstrides: src or dest is NULL.");
        goto fail;
    }
    
    if (!check_broadcastexpand(src, dest->nd, dest->dimensions)) {
        SET_ERROR_MESSAGE("assign_broadcastingstrides: check_broadcastexpand failed.");
        goto fail;
    }
    
    /* override original strides */
    int offset = dest->nd - src->nd; //2
    int d = dest->nd - 1;
    do {  //src->nd == 4(0~3), dest->nd == 6(0~5)
        if (src->dimensions[d - offset] == 1) {
            out_strides[d] = 0;
        } else {
            out_strides[d] = get_recalculatstride(d, dest->nd, dest->dimensions, dest->itemsize); // ndの拡張により、src->strides[d]と書くことはできない
        }
    } while (d-- > offset);
    /* apply additional strides */
    if (d > 0) {
        do { //d == 1
            out_strides[d] = 0;
        } while (d-- > 0);
    }
}