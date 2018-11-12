#include "vector_map.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//itemに問題あり？？
Vector_t *vector_new(){
    struct Vector* vec;
    //free when program is done.
    vec = (Vector_t*)malloc(sizeof(struct Vector));
    vec->item = malloc(sizeof(void*));
    vec->cap = 1;
    vec->len = 0;
    return vec;
}

void vector_push(Vector_t *vec, void *item){
    if(vec->len >= vec->cap){
        vec->item = realloc(vec->item, 2*vec->cap*sizeof(void*));
        vec->cap *= 2;
    }

    vec->item[vec->len++] = item;
}

void *vector_get(Vector_t *vec, size_t idx){
    return vec->item[idx];
}

size_t vector_size(Vector_t *vec){
    return vec->len;
}

Map_t* map_new(){
    Map_t* map = (Map_t*)malloc(sizeof(Map_t));
    map->key = vector_new();
    map->val = vector_new();
    return map;
}

void map_push(Map_t* map, char* key, int val){
    vector_push(map->key, (char*)key);
    int* v = (int*)malloc(sizeof(int));
    *v = val;
    vector_push(map->val, v);
}

int* map_search(Map_t* map, char* key){
    for(int i = vector_size(map->key)-1; i >= 0; i--){
        if(!strcmp(vector_get(map->key, i), key))
            return (int*)vector_get(map->val, i);
    }
    return NULL;
}

size_t map_size(Map_t* map){
    return vector_size(map->key);
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
            printf("item : %d\n", *y);
            assert(0);
        }
    }
    free(vec);

    printf("Vector test passed.\n");
}

void test_map(){
    Map_t* map = map_new();
    map_push(map, "John", 5);
    map_push(map, "Bob", 12);
    map_push(map, "Alice", 40);
    map_push(map, "Rob", 23);
    map_push(map, "Peter", 10);

    int* alice = map_search(map, "Alice");
    if(alice == NULL){
        printf("Address is NULL\n");
        assert(0);
    }
    if(*alice != 40){
        printf("val : %d\n", *alice);
        assert(0);
    }

    int* yumi = map_search(map, "Yumi");
    if(yumi != NULL){
        printf("search : %p\n", yumi);
        assert(0);
    }

    printf("Map test passed.\n");
}


