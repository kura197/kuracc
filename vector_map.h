#ifndef VECTOR_MAP_H
#define VECTOR_MAP_H

#include <stdio.h>

typedef struct Vector{
    void** val;     
    size_t len;
    size_t cap;
}Vector_t;

typedef struct Map{
    char* name;
    int val;
}Map_t;

Vector_t *vector_new();
void vector_push(Vector_t *vec, void *item);
void *vector_get(Vector_t *vec, size_t idx);
size_t vector_size(Vector_t *vec);
void test_vector();

#endif
