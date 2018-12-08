#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mycc.h"


Vector_t *vector_new(){
    struct Vector* vec;
    //free when program is done.
    vec = malloc(sizeof(struct Vector));
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

void *vector_get(Vector_t *vec, int idx){
    return vec->item[idx];
}

int vector_size(Vector_t *vec){
    return vec->len;
}

void vector_pop(Vector_t *vec){
    vec->len--;
}

void vector_delete(Vector_t *vec, int idx){
    vec->len--;
    if(vec->len == 0){
        vec = NULL;
        return;
    }
    for(int i = idx; i < vec->len; i++){
        vec->item[i] = vec->item[i+1];
    }
}

Map_t* map_new(){
    Map_t* map = malloc(sizeof(Map_t));
    map->key = vector_new();
    map->val = vector_new();
    return map;
}

void map_push(Map_t* map, char* key, void* val){
    vector_push(map->key, key);
    vector_push(map->val, val);
}

void* map_search(Map_t* map, char* key){
    for(int i = vector_size(map->key)-1; i >= 0; i--){
        if(!strcmp(vector_get(map->key, i), key))
            return vector_get(map->val, i);
    }
    return NULL;
}

int map_size(Map_t* map){
    return vector_size(map->key);
}

void map_pop(Map_t* map){
    vector_pop(map->key);
    vector_pop(map->val);
}

void test_vector(){
    Vector_t* vec = vector_new();

    int x[1024];
    for(int i = 0; i < 1024; i++){
        x[i] = i;
        //vector_push(vec, (int*)&x[i]);
        vector_push(vec, &x[i]);
    }

    if(vector_size(vec) != 1024){
        //printf("vector_len : %d\n", (int)vector_size(vec));
        printf("vector_len : %d\n", vector_size(vec));
        assert(0);
    }

    for(int i = 0; i < 1024; i++){
        int *y = vector_get(vec, i);
        if(*y != x[i]){
            printf("item : %d\n", *y);
            assert(0);
        }
    }

    vector_delete(vec, 100);
    int *z = vector_get(vec, 100);
    if(*z != 101){
        printf("item : %d\n", *z);
        assert(0);
    }

    free(vec);

    printf("Vector test passed.\n");
}

void test_map(){
    Map_t* map = map_new();

    int* val[5];
    for(int i = 0; i < 5; i++){
        val[i] = malloc(sizeof(int));
    }
    *val[0] = 5;
    *val[1] = 12;
    *val[2] = 40;
    *val[3] = 23;
    *val[4] = 10;
    map_push(map, "John", val[0]);
    map_push(map, "Bob", val[1]);
    map_push(map, "Alice", val[2]);
    map_push(map, "Rob", val[3]);
    map_push(map, "Peter", val[4]);

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


