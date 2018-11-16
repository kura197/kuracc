#ifndef SEMANTIC_H_
#define SEMANTIC_H_

#include "parser.h"
#include "vector_map.h"

#define MAX_BLOCK_DEPTH 128

struct Node;

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
    int offset;
}Symbol_t;

typedef struct SymTable{
    Map_t *global;
    Map_t *arg;
    Map_t *label;
    Map_t *string;
    int local_index;
    int num_var;
    Map_t *local[MAX_BLOCK_DEPTH];
}SymTable_t;


SymTable_t* sym_table_new();
Symbol_t* sym_new(char* name, int type, struct Node* ast, int name_space, int num_var);
void sem_analy(struct Node* ast, int level);

extern Map_t* global;

#endif
