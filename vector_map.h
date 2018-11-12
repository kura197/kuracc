#ifndef VECTOR_MAP_H
#define VECTOR_MAP_H

#include <stdio.h>

typedef struct Vector{
    void** item;     
    size_t len;
    size_t cap;
}Vector_t;

typedef struct Map{
    //char -> int
    Vector_t* key;
    Vector_t* val;
}Map_t;

Vector_t* vector_new();
void vector_push(Vector_t *vec, void *item);
void* vector_get(Vector_t *vec, size_t idx);
size_t vector_size(Vector_t *vec);

Map_t* map_new();
void map_push(Map_t* map, char* key, int val);
int* map_search(Map_t* map, char* key);
size_t map_size(Map_t* map);

void test_vector();
void test_map();

#endif
