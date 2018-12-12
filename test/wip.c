
int* malloc();

typedef struct Vector{
    void** item;     
    int len;
    int cap;
}Vector_t;

typedef struct Map{
    //char -> int
    Vector_t* key;
    Vector_t* val;
}Map_t;

Vector_t *vector_new(){
    struct Vector* vec;
    //free when program is done.
    vec = malloc(sizeof(struct Vector));
    vec->item = malloc(sizeof(void*));
    vec->cap = 1;
    vec->len = 0;
    return vec;
}

Map_t* map_new(){
    Map_t* map = malloc(sizeof(Map_t));
    map->key = vector_new();
    map->val = vector_new();
    //fprintf(stderr, "map:%x\n", map);
    return map;
}
