#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

Node_t* new_node(int op, Node_t* lhs, Node_t* rhs){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node_t* new_node_num(int val){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = TK_INT;
    node->val = val;
    return node;
}

void error(Token_t* tk){
    fprintf(stderr, "read token : %s\n", token_name[tk->kind]);
}

Node_t* expr(){
    Node_t *node = expr_mul();

    Token_t* next = read_token();
    while(next->kind == '+' || next->kind == '-'){
        if(next->kind == '+'){
            consume_token('+');
            node = new_node('+', node, expr_mul());

        }
        else if(next->kind == '-'){
            consume_token('-');
            node = new_node('-', node, expr_mul());
        }
        next = read_token();
    }

    return node;
}

Node_t* expr_mul(){
    Node_t* node = expr_term();

    Token_t* next = read_token();
    while(next->kind == '*' || next->kind == '/'){
        if(next->kind == '*'){
            consume_token('*');
            node = new_node('*', node, expr_term());

        }
        else if(next->kind == '/'){
            consume_token('/');
            node = new_node('/', node, expr_term());
        }
        next = read_token();
    }
    return node;
}

Node_t* expr_term(){
    Token_t* next = read_token();
    if(next->kind == TK_INT){
        consume_token(TK_INT);
        return new_node_num(next->value);
    }
    else if(next->kind == '('){
        consume_token('(');
        Node_t* node = expr();
        if(read_token()->kind != ')'){
            error(read_token());
            assert(0);
        }
        consume_token(')');
        return node;
    }
    else{
        error(next);
        assert(0);
    }
    return NULL;
}

Node_t* equ_expr(){
    Node_t* node = expr();
    Token_t* next = read_token();
    if(next->kind == TK_EQ){
        consume_token(TK_EQ);
        node = new_node(TK_EQ, node, expr());
    }
    else if(next->kind == TK_NEQ){
        consume_token(TK_NEQ);
        node = new_node(TK_NEQ, node, expr());
    }

    return node;
}

void dump_node(Node_t* node, int num){
    if(node->lhs != NULL){
        dump_node(node->lhs, num+1);
    }

    if(node->op == TK_INT)
        printf("%3d : %s(%d)\n", num, token_name[node->op], node->val);
    else
        printf("%3d : %s\n", num, token_name[node->op]);

    if(node->rhs != NULL){
        dump_node(node->rhs, num+1);
    }
}
