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
    assign_pad_constant,  // Constant
    assign_pad_edge,      // Edge
    assign_pad_linearramp,// LinearRamp
    assign_pad_maximum,   // Maximum
    assign_pad_mean,      // Mean
    assign_pad_median,    // Median
    assign_pad_minimum,   // Minimum
    assign_pad_reflect,   // Reflect
    assign_pad_symmetric, // Symmetric
    assign_pad_wrap,      // Wrap
    assign_pad_empty,     // Empty
};