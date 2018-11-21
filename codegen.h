#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "parser.h"
#include "semantic.h"

void codegen(Node_t *node);
void codegen_lval(Node_t *node);
int codegen_arg(Node_t* node, int num);
void codegen_comp_stmt(Node_t* node);
Symbol_t* get_sym(char* name);
int is_ptr(Type_t* type);
int search_sym(char* name);
int allign4(int x);
void codegen_str();
void codegen_global_init();
void codegen_global();

extern Map_t* strlabel;

#endif
