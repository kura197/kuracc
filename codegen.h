#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "parser.h"

void codegen(Node_t *node);
void codegen_lval(Node_t *node);
int codegen_arg(Node_t* node, int num);

extern int rsp_allign;

#endif
