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

int vector_search(Vector_t *vec, char* str){
    for(int i = vec->len-1; i>=0; i--){
        Map_t* map = (Map_t*)vec->item[i];
        if(!strcmp(str, map->name)){
            return map->val;
        }
    }
    return -1;
}

Map_t* map_create(char* name, int val){
    Map_t* map = (Map_t*)malloc(sizeof(Map_t));
    map->name = name;
    map->val = val;
    return map;
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
    Vector_t* vec = vector_new();

    Map_t* map[5];

    map[0] = map_create("John", 5);
    map[1] = map_create("Bob", 12);
    map[2] = map_create("Alice", 40);
    map[3] = map_create("Rob", 23);
    map[4] = map_create("Peter", 10);

    for(int i = 0; i < 5; i++){
        vector_push(vec, (Map_t*)map[i]);
    }

    Map_t* test = vector_get(vec, 3);
    if(test->val != map[3]->val){
        printf("val : %d\n", test->val);
        assert(0);
    }

    if(vector_search(vec, "Bob") != 12){
        printf("search : %d\n", vector_search(vec,"Bob"));
        assert(0);
    }

    if(vector_search(vec, "Yumi") != -1){
        printf("search : %d\n", vector_search(vec,"Yumi"));
        assert(0);
    }

    printf("Map test passed.\n");
}


