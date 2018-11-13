#include <stdio.h>
#include <assert.h>
#include "codegen.h"

int rsp_allign;

void codegen(Node_t* node){
    if(node == NULL){
        assert(0);
    }

    int num_arg;
    switch(node->op){
        case AST_INT:
            printf("  movl $%d, %%eax\n", node->val);
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case '+':
        case '-':
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            if(node->op == '+'){
                printf("  addl %%ebx, %%eax\n");
            }
            else if(node->op == '-'){
                printf("  subl %%ebx, %%eax\n");
            }
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case '*':
        case '/':
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            if(node->op == '*'){
                printf("  imul %%ebx, %%eax\n");
            }
            else if(node->op == '/'){
                printf("  cltd\n");
                printf("  div %%ebx\n");
            }
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case AST_EQ:
        case AST_NEQ:
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  popq %%rbx\n");
            printf("  popq %%rax\n");
            rsp_allign -= 8;
            printf("  cmpl %%eax, %%ebx\n");
            if(node->op == TK_EQ)   
                printf("  sete %%al\n");
            if(node->op == TK_NEQ)   
                printf("  setne %%al\n");
            printf("  movzbl %%al, %%eax\n");
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case '=':
            codegen_lval(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            printf("  movl %%ebx, (%%rax)\n");
            //do not need this.
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case AST_ID:
            printf("  movl -%d(%%rbp), %%eax\n", *map_search(var, node->name));
            printf("  pushq %%rax\n");
            rsp_allign += 4;
            break;

        case AST_POST_FIX:
            num_arg = 0;
            if(node->rhs != NULL && node->rhs->op == AST_ARG_LIST) 
                num_arg = codegen_arg(node->rhs, 0);
            for(int arg = num_arg-1; arg >= 0; arg--){
                //max:6
                if(arg == 0) printf("  pop %%rdi\n");
                else if(arg == 1) printf("  pop %%rsi\n");
                else if(arg == 2) printf("  pop %%rdx\n");
                else if(arg == 3) printf("  pop %%rcx\n");
                else if(arg == 4) printf("  pop %%r8\n");
                else if(arg == 5) printf("  pop %%r9\n");
            }
            int rem = 16 - rsp_allign % 16;
            if(rem > 0) printf("  subq $%d, %%rsp\n", rem);
            printf("  call %s\n", node->lhs->name);
            if(rem > 0) printf("  addq $%d, %%rsp\n", rem);
            break;

        case AST_FUNC:
            codegen(node->rhs);

        case AST_COMP_STMT:
            codegen_comp_stmt(node);
    }
}


void codegen_lval(Node_t* node){
    if(node->op == TK_ID){
        printf("  leaq -%d(%%rbp), %%rax\n", *map_search(var, node->name));
        printf("  pushq  %%rax\n");
        rsp_allign += 4;
    }
    else{
        fprintf(stderr, "not ID lvalue\n");
        assert(0);
    }
}

int codegen_arg(Node_t* node, int num){
    if(node->op != AST_ARG_LIST){
        codegen(node);
        num++;
    }
    else{
        if(node->lhs != NULL){
            num = codegen_arg(node->lhs, num);
        }
        if(node->rhs != NULL){
            num = codegen_arg(node->rhs, num);
        }
    }

    return num;
}

void codegen_comp_stmt(Node_t* node){
    ;    
}
