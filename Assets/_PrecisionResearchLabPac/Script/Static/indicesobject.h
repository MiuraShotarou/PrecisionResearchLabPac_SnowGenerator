// indicesobject.h

typedef struct {
    int64_t *data;          // 実データへのポインタ（先頭アドレス）
    int count;
    int size;
    int capacity;
} IndicesObject;

IndicesObject *indicesobject_create(int count, int size);

int64_t* indicesobject_indices(IndicesObject *obj, int d);

int64_t indicesobject_index(IndicesObject *obj, int d, int i);

bool assign_indicesobject_append(int64_t *indices, IndicesObject *out_obj);

//// i番目のindicesにアクセス
//int64_t *index = indices->data + i * indices->cols;

//// i番目のindicesのj番目の要素にアクセス
//int64_t val = indices->data[i * indices->cols + j];