#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"
#include "vector_map.h"

typedef struct Node{
    int op;
    struct Node *lhs;
    struct Node *rhs;
    //op == TK_INT
    int val; 
    //op == TK_ID
    char *name;
}Node_t;

enum ast_kind{
    AST_INT,  
    AST_EQ, 
    AST_NEQ,
    AST_ID,
    AST_POST_FIX,
    //AST_FUNC,
    AST_FUNC_ARG
};

Node_t* new_node(int op, Node_t* lhs, Node_t* rhs);
Node_t* new_node_num(int val);
void error(Token_t* tk);
void dump_node(Node_t* node, int num);
Node_t* primary_expr();
Node_t* add_expr();
Node_t* mul_expr();
Node_t* equ_expr();
Node_t* assign_expr();
Node_t* postfix_expr();
Node_t* arg_expr_list();

extern Map_t* var;
extern char *ast_name[];

#endif
