#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"
#include "vector_map.h"

typedef struct Node{
    int op;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char *name;
}Node_t;

Node_t* new_node(int op, Node_t* lhs, Node_t* rhs);
Node_t* new_node_num(int val);
void error(Token_t* tk);
void dump_node(Node_t* node, int num);
Node_t* expr_add();
Node_t* expr_mul();
Node_t* expr_term();
Node_t* equ_expr();
Node_t* assign_expr();

extern Map_t* var;

#endif
