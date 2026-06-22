// SDType 指定で下記の(配列同士キャスト関数)を呼び出すことができるメソッド → 破壊的操作で変更する形に → 内部でsrcarray_to_resarray_tableを呼び出す
NdArray *np_cast (NdArray *src, SDType restype); //NdArray.dataに反映させれば良い → np_cast にリネーム

typedef void (*Assign_Switch_Pading)(NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_constant (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_edge     (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_linearramp(NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_maximum  (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_mean     (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_median   (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_minimum  (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_reflect  (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_symmetric(NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_wrap     (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
void assign_pad_empty    (NdArray* src, void *value, int64_t pad_width, NdArray *out_result);
static Assign_Switch_Pading assign_switch_pading_table[] = {
    assign_pad_constant,
    assign_pad_edge,
    assign_pad_linearramp,
    assign_pad_maximum,
    assign_pad_mean,
    assign_pad_median,
    assign_pad_minimum,
    assign_pad_reflect,
    assign_pad_symmetric,
    assign_pad_wrap,
    assign_pad_empty
};

typedef void (*Assign_Switch_Sorting)(NdArray *src, int axis, NdArray *out_result);
void assign_sort_quicksort (NdArray *src, int axis, NdArray *out_result);
void assign_sort_mergesort (NdArray *src, int axis, NdArray *out_result);
void assign_sort_heapsort  (NdArray *src, int axis, NdArray *out_result);
void assign_sort_stable    (NdArray *src, int axis, NdArray *out_result);
static Assign_Switch_Sorting assign_switch_sorting_table[] = {
    assign_sort_quicksort,
    assign_sort_mergesort,
    assign_sort_heapsort,
    assign_sort_stable
};