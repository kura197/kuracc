#ifndef SEMANTIC_H_
#define SEMANTIC_H_

#include "parser.h"
#include "vector_map.h"

#define MAX_BLOCK_DEPTH 128

struct Node;
struct Type;

enum NameSpace{
    NS_GLOBAL,
    NS_LOCAL,
    NS_ARG,
    NS_LABEL
};

typedef struct Symbol{
    char *name;
    struct Type *type;
    struct Node *ast;
    int name_space;
    int offset;
    enum {VAR, FUNC} role;
}Symbol_t;

typedef struct SymTable{
    Map_t *global;
    Map_t *arg;
    Map_t *label;
    Map_t *string;
    int local_index;
    int num_var;
    int offset;
    Map_t *local[MAX_BLOCK_DEPTH];
}SymTable_t;


SymTable_t* sym_table_new();
Symbol_t* sym_new(char* name, struct Type* type, struct Node* ast, int name_space, int num_var, int role);
void sem_analy(struct Node* ast);
Symbol_t* local_sym_search(SymTable_t* symt, char* name);

extern Map_t* global;
extern Map_t* global_init;
extern Vector_t* str_lit;

#endif
