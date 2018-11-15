#ifndef SEMANTIC_H_
#define SEMANTIC_H_

#include "parser.h"
#include "vector_map.h"

#define MAX_BLOCK_DEPTH 128

enum NameSpace{
    NS_GLOBAL,
    NS_LOCAL,
    NS_ARG,
    NS_LABEL
};

typedef struct Symbol{
    char *name;
    int type;
    struct Node *ast;
    int name_space;
}Symbol_t;

typedef struct SymTable{
    Vector_t *global;
    Vector_t *arg;
    Vector_t *label;
    Vector_t *string;
    int local_index;
    Vector_t *local[MAX_BLOCK_DEPTH];
}SymTable_t;


SymTable_t* sym_table_new();
Symbol_t* sym_new(char* name, int type, Node_t* ast, int name_space);
void sem_analy(Node_t* ast, int level);

extern SymTable_t* sym_table;

#endif
