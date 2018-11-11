#include <stdio.h>
#include <assert.h>
#include "codegen.h"


void codegen(Node_t* node){
    if(node == NULL){
        assert(0);
    }

    switch(node->op){
        case TK_INT:
            printf("  movl $%d, %%eax\n", node->val);
            printf("  pushq %%rax\n");
            break;

        case '+':
        case '-':
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            if(node->op == '+'){
                printf("  addl %%ebx, %%eax\n");
            }
            else if(node->op == '-'){
                printf("  subl %%ebx, %%eax\n");
            }
            printf("  pushq %%rax\n");
            break;

        case '*':
        case '/':
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            if(node->op == '*'){
                printf("  imul %%ebx, %%eax\n");
            }
            else if(node->op == '/'){
                printf("  cltd\n");
                printf("  div %%ebx\n");
            }
            printf("  pushq %%rax\n");
            break;

        case TK_EQ:
        case TK_NEQ:
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  popq %%rbx\n");
            printf("  popq %%rax\n");
            printf("  cmpl %%eax, %%ebx\n");
            if(node->op == TK_EQ)   
                printf("  sete %%al\n");
            if(node->op == TK_NEQ)   
                printf("  setne %%al\n");
            printf("  movzbl %%al, %%eax\n");
            printf("  pushq %%rax\n");
            break;

        case '=':
            codegen_lval(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            printf("  movl %%ebx, (%%rax)\n");
            //do not need this.
            printf("  pushq %%rax\n");
            break;

        case TK_ID:
            printf("  movl -%d(%%rbp), %%eax\n", vector_search(var, node->name));
            printf("  pushq %%rax\n");
            break;
    }
}


void codegen_lval(Node_t* node){
    if(node->op == TK_ID){
        printf("  leaq -%d(%%rbp), %%rax\n", vector_search(var, node->name));
        printf("  pushq  %%rax\n");
    }
    else{
        fprintf(stderr, "not ID lvalue\n");
        assert(0);
    }
}