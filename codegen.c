#include <stdio.h>
#include <assert.h>
#include "codegen.h"
#include "semantic.h"

int rsp_allign;
int num_jmp;
SymTable_t* symt;
Symbol_t* sym;

void codegen(Node_t* node){
    if(node == NULL){
        assert(0);
    }

    int num_arg;
    int tmp_num_jmp;
    switch(node->op){
        case AST_INT:
            printf("  movl $%d, %%eax\n", node->val);
            printf("  pushq %%rax\n");
            rsp_allign += 8;
            break;

        case AST_ADD:
        case AST_SUB:
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 16;
            if(node->type->ty == TYPE_PTR){
                if(node->ltype->ty == TYPE_PTR){
                    int mul_val;
                    switch(node->ltype->ptrof->ty){
                        case TYPE_PTR: mul_val = 8; break;
                        default: mul_val = 4; break;
                    }
                    printf("  imul $%d, %%ebx\n", mul_val);
                }
                else{
                    int mul_val;
                    switch(node->ltype->ptrof->ty){
                        case TYPE_PTR: mul_val = 8; break;
                        default: mul_val = 4; break;
                    }
                    printf("  imul $%d, %%eax\n", mul_val);
                }
            }
            if(node->op == AST_ADD){
                printf("  addl %%ebx, %%eax\n");
            }
            else if(node->op == AST_SUB){
                printf("  subl %%ebx, %%eax\n");
            }
            printf("  pushq %%rax\n");
            rsp_allign += 8;
            break;

        case AST_MUL:
        case AST_DIV:
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 16;
            if(node->op == AST_MUL){
                printf("  imul %%ebx, %%eax\n");
            }
            else if(node->op == AST_DIV){
                printf("  cltd\n");
                printf("  div %%ebx\n");
            }
            printf("  pushq %%rax\n");
            rsp_allign += 8;
            break;

        case AST_EQ:
        case AST_NEQ:
            codegen(node->lhs);
            codegen(node->rhs);
            printf("  popq %%rbx\n");
            printf("  popq %%rax\n");
            rsp_allign -= 16;
            printf("  cmpl %%eax, %%ebx\n");
            if(node->op == AST_EQ)   
                printf("  sete %%al\n");
            if(node->op == AST_NEQ)   
                printf("  setne %%al\n");
            printf("  movzbl %%al, %%eax\n");
            printf("  pushq %%rax\n");
            rsp_allign += 8;
            break;

        case AST_ASSIGN:
            codegen_lval(node->lhs);
            codegen(node->rhs);
            printf("  pop %%rbx\n");
            printf("  pop %%rax\n");
            rsp_allign -= 16;
            printf("  movq %%rbx, (%%rax)\n");
            //do not need this.
            //printf("  pushq %%rax\n");
            //rsp_allign += 4;
            break;

        case AST_ID:
            if(node->unary != NULL){
                int unary = *(int*)vector_get(node->unary, 0);
                if(unary == '&'){
                    codegen_lval(node);
                }
                else if(unary == '*'){
                    sym = get_sym(node->name);
                    printf("  movq -%d(%%rbp), %%rdx\n", sym->offset);
                    printf("  movq (%%rdx), %%rax\n");
                    for(int i = 1; i < vector_size(node->unary); i++){
                        unary = *(int*)vector_get(node->unary, i);
                        if(unary == '*'){
                            printf("  movq %%rax, %%rdx\n");
                            printf("  movq (%%rdx), %%rax\n");
                        }
                        else{
                            fprintf(stderr, "not yet implemented\n");
                            assert(0);
                        }
                    }
                    printf("  pushq %%rax\n");
                    rsp_allign += 8;
                }
                break;
            }
            sym = get_sym(node->name);
            printf("  movl -%d(%%rbp), %%eax\n", sym->offset);
            printf("  pushq %%rax\n");
            rsp_allign += 8;
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
            rsp_allign -= 8*num_arg;
            int diff_allign = rsp_allign % 16;
            int rem = (diff_allign != 0) ? 16 - (diff_allign) : 0;
            if(rem > 0) printf("  subq $%d, %%rsp\n", rem);
            printf("  call %s\n", node->lhs->name);
            if(rem > 0) printf("  addq $%d, %%rsp\n", rem);
            printf("  pushq %%rax\n");
            break;

        case AST_FUNC:
            printf("\n%s:\n", node->name);
            printf("  pushq %%rbp\n");
            printf("  movq %%rsp, %%rbp\n");

            symt = node->sym_table;
            rsp_allign = 8;
            if(symt->offset > 0){
                printf("  subq $%d, %%rsp\n", symt->offset);
                rsp_allign += symt->offset;
            }
            //printf("num_arg:%d\n", node->num_arg);
            for(int arg = 0; arg < map_size(symt->arg); arg++){
                //max:6
                Symbol_t* sym = (Symbol_t*)vector_get(symt->arg->val, arg);
                int offset = sym->offset;
                if(arg == 0)
                    printf("  movq %%rdi, -%d(%%rbp)\n", offset);
                else if(arg == 1)
                    printf("  movq %%rsi, -%d(%%rbp)\n", offset);
                else if(arg == 2)
                    printf("  movq %%rdx, -%d(%%rbp)\n", offset);
                else if(arg == 3)
                    printf("  movq %%rcx, -%d(%%rbp)\n", offset);
                else if(arg == 4)
                    printf("  movq %%r8, -%d(%%rbp)\n",  offset);
                else if(arg == 5)
                    printf("  movq %%r9, -%d(%%rbp)\n",  offset);
            }

            codegen(node->rhs);
            printf("  pop %%rax\n");

            printf("  movq %%rbp, %%rsp\n");
            printf("  pop %%rbp\n");
            printf("  ret\n");
            break;

        case AST_COMP_STMT:
            codegen_comp_stmt(node);
            break;

        case AST_WHILE:
            tmp_num_jmp = num_jmp;
            num_jmp++;
            printf(".L%dbegin:\n", tmp_num_jmp);
            codegen(node->lhs);
            printf("  pop %%rax\n");
            printf("  cmp $0, %%rax\n");
            printf("  je .L%dend\n", tmp_num_jmp);
            codegen(node->rhs);
            printf("  jmp .L%dbegin\n", tmp_num_jmp);
            printf(".L%dend:\n", tmp_num_jmp);
            break;

        case AST_IF:
            tmp_num_jmp = num_jmp;
            num_jmp++;
            if(node->else_stmt == NULL){
                codegen(node->lhs);
                printf("  pop %%rax\n");
                printf("  cmp $0, %%rax\n");
                printf("  je .L%dend\n", tmp_num_jmp);
                codegen(node->rhs);
                printf(".L%dend:\n", tmp_num_jmp);
            }
            else{
                codegen(node->lhs);
                printf("  pop %%rax\n");
                printf("  cmp $0, %%rax\n");
                printf("  je .L%dlatt\n", tmp_num_jmp);
                codegen(node->rhs);
                printf("  jmp .L%dend\n", tmp_num_jmp);
                printf(".L%dlatt:\n", tmp_num_jmp);
                codegen(node->else_stmt);
                printf(".L%dend:\n", tmp_num_jmp);
            }
            break;

        case AST_FOR:
            tmp_num_jmp = num_jmp;
            num_jmp++;
            if(node->lfor != NULL) codegen(node->lfor);
            printf("  jmp .L%dmid\n", tmp_num_jmp);
            printf(".L%dright:\n", tmp_num_jmp);
            if(node->rfor != NULL) codegen(node->rfor);
            codegen(node->lhs);
            printf(".L%dmid:\n", tmp_num_jmp);
            if(node->mfor != NULL) codegen(node->mfor);
            printf("  pop %%rax\n");
            printf("  cmp $0, %%rax\n");
            printf("  jne .L%dright\n", tmp_num_jmp);
            break;

        case AST_FUNC_DEC:
            break;

        default:
            printf("node : %s\n", ast_name[node->op]);
            assert(0);
            break;
    }
}


void codegen_lval(Node_t* node){
    if(node->op == AST_ID){
        if(node->unary != NULL && *(int*)vector_get(node->unary, 0) == '*'){
            codegen(node);
            return;
        }
        sym = get_sym(node->name);
        printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
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
    if(node->op != AST_COMP_STMT && node->op != AST_DEC){
        codegen(node);
    }
    else{
        if(node->lhs != NULL){
            codegen_comp_stmt(node->lhs);
        }
        if(node->rhs != NULL){
            codegen_comp_stmt(node->rhs);
        }
    }
}

Symbol_t* get_sym(char* name){
    Symbol_t* sym = map_search(symt->local[0], name);
    if(sym == NULL) sym = map_search(symt->arg, name);
    if(sym == NULL) sym = map_search(symt->global, name);
    if(sym == NULL) assert(0);
    return sym;
}
