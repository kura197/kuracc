#include "vector_map.h"
#include <stdlib.h>
#include <assert.h>

//valに問題あり？？
Vector_t *vector_new(){
    struct Vector* vec;
    //free when program is done.
    vec = (Vector_t*)malloc(sizeof(struct Vector));
    vec->val = malloc(sizeof(void*));
    vec->cap = 1;
    vec->len = 0;
    return vec;
}

void vector_push(Vector_t *vec, void *item){
    if(vec->len >= vec->cap){
        vec->val = realloc(vec->val, 2*vec->cap*sizeof(void*));
        vec->cap *= 2;
    }

    vec->val[vec->len++] = item;
}

void *vector_get(Vector_t *vec, size_t idx){
    return vec->val[idx];
}

size_t vector_size(Vector_t *vec){
    return vec->len;
}

void test_vector(){
    Vector_t* vec = vector_new();

    int x[1024];
    for(int i = 0; i < 1024; i++){
        x[i] = i;
        vector_push(vec, (int*)&x[i]);
    }

    if(vector_size(vec) != 1024){
        printf("vector_len : %d\n", (int)vector_size(vec));
        assert(0);
    }

    for(int i = 0; i < 1024; i++){
        int *y = vector_get(vec, i);
        if(*y != x[i]){
            printf("val : %d\n", *y);
            assert(0);
        }
    }
    free(vec);

    printf("Vector test passed.\n\n");
}
