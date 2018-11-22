#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "codegen.h"
#include "semantic.h"

int rsp_allign;
int num_jmp;
int num_ret;
SymTable_t* symt;
Symbol_t* sym;
Map_t* strlabel;
char* arg_regq_name[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
char* arg_regl_name[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};


void codegen(Node_t* node){
    if(node == NULL){
        assert(0);
    }
    int num_arg;
    int tmp_num_jmp;

    if(node->op == AST_INT){
        printf("  movl $%d, %%eax\n", node->val);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(node->op == AST_ADD || node->op == AST_SUB){
        codegen(node->lhs);
        codegen(node->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(node->type->ty == TYPE_PTR || node->type->ty == TYPE_ARRAY){
            if(node->ltype->ty == TYPE_PTR || node->ltype->ty == TYPE_ARRAY){
                int mul_val;
                mul_val = get_type_size(node->ltype->ptrof);
                printf("  imul $%d, %%ebx\n", mul_val);
            }
            else{
                int mul_val;
                mul_val = get_type_size(node->rtype->ptrof);
                printf("  imul $%d, %%eax\n", mul_val);
            }
        }
        if(node->op == AST_ADD){
            printf("  addq %%rbx, %%rax\n");
        }
        else if(node->op == AST_SUB){
            printf("  subq %%rbx, %%rax\n");
        }
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(node->op == AST_MUL || node->op == AST_DIV){
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
    }
    else if(node->op == AST_EQ || node->op == AST_NEQ){
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
    }
    else if(node->op == AST_ASSIGN){
        codegen_lval(node->lhs);
        codegen(node->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(is_ptr(node->type))
            printf("  movq %%rbx, (%%rax)\n");
        else{
            if(get_type_size(node->type) == 4)
                printf("  movl %%ebx, (%%rax)\n");
            else if(get_type_size(node->type) == 1)
                printf("  movb %%bl, (%%rax)\n");
            else
                assert(0);
        }
    }
    else if(node->op == AST_INIT_DEC){
        if(node->global) return;
        codegen_lval(node->lhs);
        codegen(node->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(is_ptr(node->type))
            printf("  movq %%rbx, (%%rax)\n");
        else{
            if(get_type_size(node->type) == 4)
                printf("  movl %%ebx, (%%rax)\n");
            else if(get_type_size(node->type) == 1)
                printf("  movb %%bl, (%%rax)\n");
            else
                assert(0);
        }
    }
    else if(node->op == AST_STRING){
        char* label = map_search(strlabel, node->name);
        printf("  movq $%s, %%rax\n", label);
        printf("  pushq %%rax\n");
    }
    else if(node->op == AST_ID){
        if(search_sym(node->name) == NS_GLOBAL){
            if(node->type->ty == TYPE_ARRAY){
                printf("  leaq %s(%%rip), %%rax\n", node->name);
            }
            else{
                if(is_ptr(node->type))
                    printf("  movq %s(%%rip), %%rax\n", node->name);
                else{
                    if(get_type_size(node->type) == 4)
                        printf("  movl %s(%%rip), %%eax\n", node->name);
                    else if(get_type_size(node->type) == 1)
                        printf("  movzbl %s(%%rip), %%eax\n", node->name);
                    else
                        assert(0);
                }
            }
            printf("  pushq %%rax\n");
            rsp_allign += 8;
        }
        else{
            sym = get_sym(node->name);
            if(node->type->ty == TYPE_ARRAY){
                printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
            }
            else{
                if(is_ptr(node->type))
                    printf("  movq -%d(%%rbp), %%rax\n", sym->offset);
                else{
                    if(get_type_size(node->type) == 4)
                        printf("  movl -%d(%%rbp), %%eax\n", sym->offset);
                    else if(get_type_size(node->type) == 1)
                        printf("  movzbl -%d(%%rbp), %%eax\n", sym->offset);
                    else
                        assert(0);
                }
            }
            printf("  pushq %%rax\n");
            rsp_allign += 8;
        }
    }
    else if(node->op == AST_FUNC_CALL){
        num_arg = 0;
        if(node->rhs != NULL && node->rhs->op == AST_ARG_LIST) 
            num_arg = codegen_arg(node->rhs, 0);
        for(int arg = num_arg-1; arg >= 0; arg--){
            //max:6
            printf("  pop %%%s\n", arg_regq_name[arg]);
        }
        rsp_allign -= 8*num_arg;
        int diff_allign = rsp_allign % 16;
        int rem = (diff_allign != 0) ? 16 - (diff_allign) : 0;
        if(rem > 0) printf("  subq $%d, %%rsp\n", rem);
        printf("  movq $0, %%rax\n");
        printf("  call %s\n", node->lhs->name);
        if(rem > 0) printf("  addq $%d, %%rsp\n", rem);
        printf("  pushq %%rax\n");
    }
    else if(node->op == AST_FUNC){
        printf("\n%s:\n", node->name);
        printf("  pushq %%rbp\n");
        printf("  movq %%rsp, %%rbp\n");

        symt = node->sym_table;
        rsp_allign = 8;
        if(symt->offset > 0){
            printf("  subq $%d, %%rsp\n", allign4(symt->offset));
            rsp_allign += symt->offset;
        }
        //printf("num_arg:%d\n", node->num_arg);
        for(int arg = 0; arg < map_size(symt->arg); arg++){
            //max:6
            Symbol_t* sym = (Symbol_t*)vector_get(symt->arg->val, arg);
            int offset = sym->offset;
            if(is_ptr(sym->type))
                printf("  movq %%%s, -%d(%%rbp)\n", arg_regq_name[arg], offset);
            else
                printf("  movl %%%s, -%d(%%rbp)\n", arg_regl_name[arg], offset);
        }

        codegen(node->rhs);
        //printf("  pop %%rax\n");

        printf(".End%d:\n", num_ret++);
        printf("  movq %%rbp, %%rsp\n");
        printf("  pop %%rbp\n");
        printf("  ret\n");
    }
    else if(node->op == AST_COMP_STMT){
        codegen_comp_stmt(node);
    }
    else if(node->op == AST_WHILE){
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
    }
    else if(node->op == AST_IF){
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
    }
    else if(node->op == AST_FOR){
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
    }
    else if(node->op == AST_FUNC_DEC){
    }
    else if(node->op == AST_UNARY_PTR){
        codegen(node->lhs);
        printf("  pop %%rbx\n");
        printf("  movq (%%rbx), %%rax\n");
        printf("  pushq %%rax\n");
    }
    else if(node->op == AST_UNARY_ADR){
        codegen_lval(node->lhs);
    }
    else if(node->op == AST_UNARY_MINUS){
        codegen(node->lhs);
        printf("  pop %%rax\n");
        printf("  negl %%eax\n");
        printf("  pushq %%rax\n");
    }
    else if(node->op == AST_DEC){   //global
    }
    else if(node->op == AST_RET){
        if(node->lhs != NULL){
            codegen(node->lhs);
            printf("  pop %%rax\n");
            printf("  jmp .End%d\n", num_ret);
        }
    }
    else if(node->op == AST_AND){
        codegen(node->lhs);
        codegen(node->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        printf("  andl %%ebx, %%eax\n");
        printf("  pushq %%rax\n");
    }
    else{
        printf("node : %s\n", ast_name[node->op]);
        assert(0);
    }
}


void codegen_lval(Node_t* node){
    if(node->op == AST_ID){
        if(search_sym(node->name) == NS_GLOBAL){
            //sym = map_search(symt->global, node->name);
            printf("  leaq %s(%%rip), %%rax\n", node->name);
            printf("  pushq  %%rax\n");
            rsp_allign += 4;
        }
        else{
            sym = get_sym(node->name);
            printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
            printf("  pushq  %%rax\n");
            rsp_allign += 4;
        }
    }
    else if(node->op == AST_UNARY_PTR){
        codegen(node->lhs);
    }
    else if(node->op == AST_DEC){   //for AST_INIT_DEC
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

int search_sym(char* name){
    if(map_search(symt->local[0], name) != NULL)
        return NS_LOCAL;
    else if(map_search(symt->arg, name) != NULL)
        return NS_ARG;
    else if(map_search(symt->global, name) != NULL)
        return NS_GLOBAL;
    else if(map_search(symt->label, name) != NULL)
        return NS_LABEL;
    else
        return -1;
}

int is_ptr(Type_t* type){
    if(type->ty == TYPE_PTR || (type->ty == TYPE_ARRAY && type->ptrof->ty == TYPE_PTR))
        return 1;
    else
        return 0;
}

int allign4(int x){
    int n = 0;
    while(x > n){
        n += 4;
    }
    return n;
}

void codegen_str(){
    for(int i = 0; i < vector_size(str_lit); i++){
        char* str = vector_get(str_lit, i);
        char* label = (char*)malloc(8*sizeof(char));
        map_push(strlabel, str, label);
        sprintf(label, ".LC%d", i);
        printf("%s:\n", label);
        printf("  .string \"%s\"\n", str);
    }
}

void codegen_global_init(){
    for(int i = 0; i < map_size(global_init); i++){
        Node_t* ast = vector_get(global_init->val, i);
        char *name = vector_get(global_init->key, i);
        if(ast->op == AST_UNARY_MINUS){
            ast = ast->lhs;
            ast->val *= -1;
        }

        if(ast->op == AST_STRING){
            char* label = map_search(strlabel, ast->name);
            printf("%s:\n", name);
            printf("  .quad %s\n", label);
        }else{
            Symbol_t* sym = map_search(global, name);
            int size = get_type_size(sym->type);
            printf("%s:\n", name);
            if(size == 4){
                printf("  .long %d\n", ast->val);
            }
            else if(size == 1){
                printf("  .byte %d\n", ast->val);
            }
        }
    }
}

void codegen_global(){
    for(int i = 0; i < map_size(global); i++){
        Symbol_t* sym = vector_get(global->val, i);
        if(sym->role == FUNC) continue;
        if(map_search(global_init, sym->name) != NULL) continue;
        int size = get_type_size(sym->type);
        if(sym->type->ty == TYPE_ARRAY)
            size *= sym->type->array_size;
        printf("  .comm  %s,%d\n", sym->name, size);
    }
}

