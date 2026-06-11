// indicesobject.c
IndicesObject *indicesobject_create(int count, int size)
{
    IndicesObject *obj = (IndicesObject *)malloc(sizeof(IndicesObject));
    if (!obj) return NULL;
    obj->data = (int64_t *)malloc(sizeof(int64_t) * count * size);
    if (!obj->data) {
        free(obj);
        return NULL;
    }
    obj->count = count;
    obj->size = size;
    obj->capacity = INDICES_MAX_CAPACITY(obj);
    return obj;
}

int64_t* indicesobject_indices(IndicesObject *obj, int d)
{
    int64_t *indices = obj->data + d * obj->size;
    return indices;
}

int64_t indicesobject_index(IndicesObject *obj, int d, int i)
{
    int64_t index = obj->data[d * obj->size + i];
    return index;
}

bool assign_indicesobject_append(int64_t *indices, IndicesObject *out_obj)
{
    /*
    if (obj->rows >= obj->capacity) {
        int new_capacity = obj->capacity * 2;
        int64_t *new_data = realloc(obj->data, new_capacity * obj->size * sizeof(int64_t));
        if (new_data == NULL) return false;
        obj->data = new_data;
        obj->capacity = new_capacity;
    }
    memcpy(obj->data + obj->rows * obj->size, indices, obj->size * sizeof(int64_t));
    obj->rows++;
    return true;
    */
}