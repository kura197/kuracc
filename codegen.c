#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mycc.h"

int rsp_allign;
int num_jmp;
int num_ret;
SymTable_t* symt;
Symbol_t* sym;
Map_t* strlabel;
char* arg_regq_name[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
char* arg_regl_name[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
int case_label;
int break_label;
int cont_label;
int struct_var_flag;
int struct_var_en;

void codegen(Node_t* ast){
    if(ast == NULL){
        assert(0);
    }
    if(ast->op == AST_INT){
        printf("  movl $%d, %%eax\n", ast->val);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_CHAR){
        printf("  movb $%d, %%al\n", ast->val);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_ID){
        sym = ast->sym;
        int *offset;
        if(struct_var_en){
            struct_var_en = 0;
            struct_var_flag = 1;
        }
        if((offset = map_search(enum_dec, ast->name)) != NULL){
            printf("  movl $%d, %%eax\n", *offset);
        }
        else{
            if(sym->name_space == NS_GLOBAL){
                if(ast->type->ty == TYPE_ARRAY){
                    printf("  leaq %s(%%rip), %%rax\n", ast->name);
                }
                else{
                    if(is_ptr(ast->type))
                        printf("  movq %s(%%rip), %%rax\n", ast->name);
                    else{
                        if(get_type_size(ast->type) == 4)
                            printf("  movl %s(%%rip), %%eax\n", ast->name);
                        else if(get_type_size(ast->type) == 1)
                            printf("  movzbl %s(%%rip), %%eax\n", ast->name);
                        else
                            assert(0);
                    }
                }
            }
            else{
                if(ast->type->ty == TYPE_ARRAY){
                    printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
                }
                else{
                    if(is_ptr(ast->type))
                        printf("  movq -%d(%%rbp), %%rax\n", sym->offset);
                    else{
                        if(get_type_size(ast->type) == 4)
                            printf("  movl -%d(%%rbp), %%eax\n", sym->offset);
                        else if(get_type_size(ast->type) == 1)
                            printf("  movzbl -%d(%%rbp), %%eax\n", sym->offset);
                        else
                            assert(0);
                    }
                }
            }
        }
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_STRING){
        char* label = map_search(strlabel, ast->name);
        //printf("  movq $%s, %%rax\n", label);
        printf("  leaq %s(%%rip), %%rax\n", label);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_POST_INC || ast->op == AST_POST_DEC){
        codegen_lval(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        int add_val = 1;
        if(ast->type->ty == TYPE_PTR || ast->type->ty == TYPE_ARRAY){
            add_val = get_type_size(ast->ltype->ptrof);
        }
        printf("  movq (%%rax), %%rbx\n");
        if(ast->op == AST_POST_INC){
            printf("  addq $%d, (%%rax)\n", add_val);
        }
        else if(ast->op == AST_POST_DEC){
            printf("  subq $%d, (%%rax)\n", add_val);
        }
        printf("  pushq %%rbx\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_STRUCT_ID){
        struct_var_en = 1;
        Type_t* struct_type;
        Type_t* member_type;
        char* member_name = ast->rhs->name;
        char* struct_name;
        codegen_lval(ast->lhs);
        sym = ast->sym;
        if(struct_var_flag){
            struct_name = get_ptr_name(sym->type);
        }
        else{
            struct_name = ast->lhs->name;
        }
        if((struct_type = map_search(struct_dec, struct_name)) == NULL){
            fprintf(stderr, "Error : struct %s was not declared\n", struct_name);
            assert(0);
        }
        if((member_type = struct_search(struct_type, member_name)) == NULL){
            fprintf(stderr, "Error : struct %s does not have %s\n", struct_name, member_name);
            assert(0);
        }

        printf("  pop %%rbx\n");
        rsp_allign -= 8;
        int offset;
        offset = member_type->offset;
        if(ast->type->ty == TYPE_ARRAY){
            printf("  leaq %d(%%rbx), %%rax\n", offset);
        }
        else{
            if(is_ptr(ast->type))
                printf("  movq %d(%%rbx), %%rax\n", offset);
            else{
                if(get_type_size(ast->type) == 4)
                    printf("  movl %d(%%rbx), %%eax\n", offset);
                else if(get_type_size(ast->type) == 1)
                    printf("  movzbl %d(%%rbx), %%eax\n", offset);
                else
                    assert(0);
            }
        }
        printf("  pushq %%rax\n");
        rsp_allign += 8;
        struct_var_flag = 0;
    }
    else if(ast->op == AST_UNARY_PTR){
        codegen(ast->lhs);
        printf("  pop %%rbx\n");
        rsp_allign -= 8;
        printf("  movq (%%rbx), %%rax\n");
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_UNARY_ADR){
        codegen_lval(ast->lhs);
    }
    else if(ast->op == AST_UNARY_MINUS){
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  negl %%eax\n");
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_UNARY_REV){
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  cmpl $0, %%eax\n");
        printf("  sete %%al\n");
        printf("  movzbl %%al, %%eax\n");
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_PRE_INC || ast->op == AST_PRE_DEC){
        codegen_lval(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        int add_val = 1;
        if(ast->type->ty == TYPE_PTR || ast->type->ty == TYPE_ARRAY){
            add_val = get_type_size(ast->ltype->ptrof);
        }
        if(ast->op == AST_PRE_INC){
            printf("  addq $%d, (%%rax)\n", add_val);
        }
        else if(ast->op == AST_PRE_DEC){
            printf("  subq $%d, (%%rax)\n", add_val);
        }
        printf("  movq (%%rax), %%rbx\n");
        printf("  pushq %%rbx\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_SIZEOF){
        printf("  movl $%d, %%eax\n", ast->val);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_CAST){
        codegen(ast->lhs);
        int cast_type = ast->type->ty;
        int pre_type = ast->ltype->ty;
        if(cast_type != pre_type){
            if(cast_type == TYPE_INT && pre_type == TYPE_CHAR){
                printf("  pop %%rax\n");
                rsp_allign -= 8;
                printf("  movsbl %%al, %%eax\n");
                printf("  pushq %%rax\n");
            }
            else if(cast_type == TYPE_CHAR && pre_type == TYPE_INT){
            }
            else if(cast_type == TYPE_PTR && pre_type == TYPE_INT){
                printf("  pop %%rax\n");
                rsp_allign -= 8;
                printf("  cltq\n");
                printf("  pushq %%rax\n");
            }
            else{
                fprintf(stderr, "Error : not yet implemented.\n");
                assert(0);
            }
        }
    }
    else if(ast->op == AST_ADD || ast->op == AST_SUB){
        codegen(ast->lhs);
        codegen(ast->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(ast->type->ty == TYPE_PTR || ast->type->ty == TYPE_ARRAY){
            if(ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY){
                int mul_val;
                mul_val = get_type_size(ast->ltype->ptrof);
                printf("  imul $%d, %%ebx\n", mul_val);
            }
            else{
                int mul_val;
                mul_val = get_type_size(ast->rtype->ptrof);
                printf("  imul $%d, %%eax\n", mul_val);
            }
        }
        if(ast->op == AST_ADD){
            printf("  addq %%rbx, %%rax\n");
        }
        else if(ast->op == AST_SUB){
            printf("  subq %%rbx, %%rax\n");
        }
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_MUL || ast->op == AST_DIV || ast->op == AST_REM){
        codegen(ast->lhs);
        codegen(ast->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(ast->op == AST_MUL){
            printf("  imul %%ebx, %%eax\n");
        }
        else if(ast->op == AST_DIV){
            printf("  cltd\n");
            printf("  div %%ebx\n");
        }
        else if(ast->op == AST_REM){
            printf("  cltd\n");
            printf("  idivl %%ebx\n");
            printf("  movl %%edx, %%eax\n");
        }
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_RSHIFT || ast->op == AST_LSHIFT){
        codegen(ast->lhs);
        codegen(ast->rhs);
        printf("  pop %%rcx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(ast->op == AST_RSHIFT){
            printf("  sarl %%cl, %%eax\n");
        }
        else if(ast->op == AST_LSHIFT){
            printf("  sall %%cl, %%eax\n");
        }
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_LARGE || ast->op == AST_SMALL || ast->op == AST_LEQ || ast->op == AST_SEQ){
        codegen(ast->lhs);
        codegen(ast->rhs);
        printf("  popq %%rbx\n");
        printf("  popq %%rax\n");
        rsp_allign -= 16;
        if(ast->op == AST_LEQ || ast->op == AST_SMALL)  
            printf("  subl $1, %%eax\n");
        printf("  cmpl %%eax, %%ebx\n");
        if(ast->op == AST_LARGE || ast->op == AST_LEQ)   
            printf("  setg %%al\n");
        else if(ast->op == AST_SMALL || ast->op == AST_SEQ)   
            printf("  setle %%al\n");
        printf("  movzbl %%al, %%eax\n");
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_EQ || ast->op == AST_NEQ){
        codegen(ast->lhs);
        codegen(ast->rhs);
        printf("  popq %%rbx\n");
        printf("  popq %%rax\n");
        rsp_allign -= 16;
        printf("  cmpl %%eax, %%ebx\n");
        if(ast->op == AST_EQ)   
            printf("  sete %%al\n");
        else if(ast->op == AST_NEQ)   
            printf("  setne %%al\n");
        printf("  movzbl %%al, %%eax\n");
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_AND || ast->op == AST_OR || ast->op == AST_EXOR){
        codegen(ast->lhs);
        codegen(ast->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(ast->op == AST_AND)   
            printf("  andl %%ebx, %%eax\n");
        else if(ast->op == AST_OR)   
            printf("  orl %%ebx, %%eax\n");
        else if(ast->op == AST_EXOR)   
            printf("  xorl %%ebx, %%eax\n");
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_LOG_AND){
        int tmp_num_jmp1 = num_jmp;
        int tmp_num_jmp2 = num_jmp+1;
        num_jmp+=2;
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  cmpl $0, %%eax\n");
        printf("  je .L%d\n", tmp_num_jmp1);
        codegen(ast->rhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  cmpl $0, %%eax\n");
        printf("  je .L%d\n", tmp_num_jmp1);
        printf("  movl $1, %%eax\n");
        printf("  jmp .L%d\n", tmp_num_jmp2);
        printf(".L%d:\n", tmp_num_jmp1);
        printf("  movl $0, %%eax\n");
        printf(".L%d:\n", tmp_num_jmp2);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_LOG_OR){
        int tmp_num_jmp1 = num_jmp;
        int tmp_num_jmp2 = num_jmp+1;
        int tmp_num_jmp3 = num_jmp+2;
        num_jmp+=3;
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  cmpl $0, %%eax\n");
        printf("  jne .L%d\n", tmp_num_jmp1);
        codegen(ast->rhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  cmpl $0, %%eax\n");
        printf("  je .L%d\n", tmp_num_jmp2);
        printf(".L%d:\n", tmp_num_jmp1);
        printf("  movl $1, %%eax\n");
        printf("  jmp .L%d\n", tmp_num_jmp3);
        printf(".L%d:\n", tmp_num_jmp2);
        printf("  movl $0, %%eax\n");
        printf(".L%d:\n", tmp_num_jmp3);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_COND){
        int tmp_num_jmp1 = num_jmp++;
        int tmp_num_jmp2 = num_jmp++;
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  cmpl $0, %%eax\n");
        printf("  je .L%d\n", tmp_num_jmp1);
        codegen(ast->lcond);
        printf("  jmp .L%d\n", tmp_num_jmp2);
        printf(".L%d:\n", tmp_num_jmp1);
        codegen(ast->rcond);
        printf(".L%d:\n", tmp_num_jmp2);
    }
    else if(ast->op == AST_ASSIGN){
        codegen_lval(ast->lhs);
        codegen(ast->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(is_ptr(ast->type))
            printf("  movq %%rbx, (%%rax)\n");
        else{
            if(get_type_size(ast->type) == 4)
                printf("  movl %%ebx, (%%rax)\n");
            else if(get_type_size(ast->type) == 1)
                printf("  movb %%bl, (%%rax)\n");
            else
                assert(0);
        }
        printf("  movl (%%rax), %%ebx\n");
        printf("  pushq %%rbx\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_EXPR){
        codegen(ast->lhs);
        codegen(ast->rhs);
    }
    else if(ast->op == AST_INIT_DEC){
        if(ast->global) return;
        codegen_lval(ast->lhs);
        codegen(ast->rhs);
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");
        rsp_allign -= 16;
        if(is_ptr(ast->type))
            printf("  movq %%rbx, (%%rax)\n");
        else{
            if(get_type_size(ast->type) == 4)
                printf("  movl %%ebx, (%%rax)\n");
            else if(get_type_size(ast->type) == 1)
                printf("  movb %%bl, (%%rax)\n");
            else{
                //OK???
                printf("  movq %%rbx, (%%rax)\n");
                //assert(0);
            }
        }
        if(!ast->global) {
            printf("  movl (%%rax), %%ebx\n");
            printf("  pushq %%rax\n");
            rsp_allign += 8;
        }
    }
    else if(ast->op == AST_FUNC_CALL){
        int num_arg = 0;
        if(ast->rhs != NULL && ast->rhs->op == AST_ARG_LIST) 
            num_arg = codegen_arg(ast->rhs, 0);
        for(int arg = num_arg-1; arg >= 0; arg--){
            //max:6
            printf("  pop %%%s\n", arg_regq_name[arg]);
        }
        rsp_allign -= 8*num_arg;
        int diff_allign = rsp_allign % 16;
        int rem = (diff_allign != 0) ? 16 - (diff_allign) : 0;
        if(rem > 0) printf("  subq $%d, %%rsp\n", rem);
        printf("  movq $0, %%rax\n");
        printf("  call %s\n", ast->lhs->name);
        if(rem > 0) printf("  addq $%d, %%rsp\n", rem);
        printf("  pushq %%rax\n");
        rsp_allign += 8;
    }
    else if(ast->op == AST_FUNC){
        rsp_allign = 8;
        printf("\n  .global %s\n", ast->name);
        printf("\n%s:\n", ast->name);
        printf("  pushq %%rbp\n");
        rsp_allign += 8;
        printf("  movq %%rsp, %%rbp\n");

        symt = ast->sym_table;
        if(symt->offset > 0){
            printf("  subq $%d, %%rsp\n", allign(symt->offset, 8)+8);
            rsp_allign += allign(symt->offset, 8) + 8;
        }
        for(int arg = 0; arg < map_size(symt->arg); arg++){
            //max:6
            Symbol_t* sym = (Symbol_t*)vector_get(symt->arg->val, arg);
            int offset = sym->offset;
            if(is_ptr(sym->type))
                printf("  movq %%%s, -%d(%%rbp)\n", arg_regq_name[arg], offset);
            else
                printf("  movl %%%s, -%d(%%rbp)\n", arg_regl_name[arg], offset);
        }

        codegen(ast->rhs);
        //printf("  pop %%rax\n");

        printf(".End%d:\n", num_ret++);
        printf("  movq %%rbp, %%rsp\n");
        printf("  pop %%rbp\n");
        rsp_allign -= 8;
        printf("  ret\n");
    }
    else if(ast->op == AST_COMP_STMT){
        if(ast->lhs != NULL) codegen(ast->lhs);
    }
    else if(ast->op == AST_BLOCK){
        int lop = ast->lhs->op;
        int rop = ast->rhs->op;
        codegen(ast->lhs);
        if(lop == AST_ID || lop == AST_INT || lop == AST_CHAR || lop == AST_PRE_INC 
                || lop == AST_PRE_DEC || lop == AST_POST_INC || lop == AST_POST_DEC 
                || lop == AST_ASSIGN || lop == AST_INIT_DEC){
            printf("  pop %%rax\n");
            rsp_allign -= 8;
        }
        codegen(ast->rhs);
        if(rop == AST_ID || rop == AST_INT || rop == AST_CHAR || rop == AST_PRE_INC 
                || rop == AST_PRE_DEC || rop == AST_POST_INC || rop == AST_POST_DEC
                || rop == AST_ASSIGN || rop == AST_INIT_DEC){
            printf("  pop %%rax\n");
            rsp_allign -= 8;
        }
    }
    else if(ast->op == AST_FUNC_DEC){
    }
    else if(ast->op == AST_DECLN){   //struct
    }
    else if(ast->op == AST_DEC){   //global
    }
    else if(ast->op == AST_WHILE){
        int tmp_num_jmp0 = num_jmp++;
        int tmp_num_jmp1 = num_jmp++;
        cont_label = tmp_num_jmp0;
        break_label = tmp_num_jmp1;
        printf(".L%d:\n", tmp_num_jmp0);
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        printf("  cmp $0, %%rax\n");
        printf("  je .L%d\n", tmp_num_jmp1);
        codegen(ast->rhs);
        printf("  jmp .L%d\n", tmp_num_jmp0);
        printf(".L%d:\n", tmp_num_jmp1);
    }
    else if(ast->op == AST_IF){
        if(ast->else_stmt == NULL){
            int tmp_num_jmp = num_jmp++;
            codegen(ast->lhs);
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            printf("  cmp $0, %%rax\n");
            printf("  je .L%d\n", tmp_num_jmp);
            codegen(ast->rhs);
            printf(".L%d:\n", tmp_num_jmp);
        }
        else{
            int tmp_num_jmp0 = num_jmp++;
            int tmp_num_jmp1 = num_jmp++;
            codegen(ast->lhs);
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            printf("  cmp $0, %%rax\n");
            printf("  je .L%d\n", tmp_num_jmp0);
            codegen(ast->rhs);
            printf("  jmp .L%d\n", tmp_num_jmp1);
            printf(".L%d:\n", tmp_num_jmp0);
            codegen(ast->else_stmt);
            printf(".L%d:\n", tmp_num_jmp1);
        }
    }
    else if(ast->op == AST_SWITCH){
        case_label = num_jmp;
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        for(int i = 0; i < ast->num_case; i++){
            int label = ast->case_stmt[i]->lhs->val;
            int *offset;
            if((offset = map_search(enum_dec, ast->case_stmt[i]->lhs->name)) != NULL){
                label = *offset;
            }
            else if(ast->case_stmt[i]->lhs->op != AST_INT && ast->case_stmt[i]->lhs->op != AST_CHAR){
                fprintf(stderr, "Error : case label is only constant\n");
                assert(0);
            }
            printf("  cmpl $%d, %%eax\n", label);
            printf("  je .L%d\n", num_jmp++);
        }
        printf("  jmp .L%d\n", num_jmp);
        if(ast->default_stmt != NULL){
            num_jmp++;
        }
        break_label = num_jmp;
        codegen(ast->rhs);
        printf(".L%d:\n", num_jmp++);
    }
    else if(ast->op == AST_CASE){
        printf(".L%d:\n", case_label++);
        codegen(ast->rhs);
    }
    else if(ast->op == AST_DEFAULT){
        printf(".L%d:\n", case_label++);
        codegen(ast->rhs);
    }
    else if(ast->op == AST_FOR){
        int tmp_num_jmp0 = num_jmp++;
        int tmp_num_jmp1 = num_jmp++;
        int tmp_num_jmp2 = num_jmp++;
        int tmp_num_jmp3 = num_jmp++;
        cont_label = tmp_num_jmp1;
        break_label = tmp_num_jmp3;
        if(ast->lfor != NULL) codegen(ast->lfor);
        printf("  jmp .L%d\n", tmp_num_jmp2);
        printf(".L%d:\n", tmp_num_jmp0);
        codegen(ast->lhs);
        printf(".L%d:\n", tmp_num_jmp1);
        if(ast->rfor != NULL) codegen(ast->rfor);
        printf(".L%d:\n", tmp_num_jmp2);
        if(ast->mfor != NULL) codegen(ast->mfor);
        printf("  pop %%rax\n");
        rsp_allign -= 8;
        printf("  cmp $0, %%rax\n");
        printf("  jne .L%d\n", tmp_num_jmp0);
        printf(".L%d:\n", tmp_num_jmp3);
    }
    else if(ast->op == AST_RET){
        if(ast->lhs != NULL){
            codegen(ast->lhs);
            printf("  pop %%rax\n");
            rsp_allign -= 8;
            printf("  jmp .End%d\n", num_ret);
        }
    }
    else if(ast->op == AST_BREAK){
        printf("  jmp .L%d\n", break_label);
    }
    else if(ast->op == AST_CONT){
        printf("  jmp .L%d\n", cont_label);
    }
    else{
        fprintf(stderr, "Unkown AST : %s\n", ast_name[ast->op]);
        assert(0);
    }
}


void codegen_lval(Node_t* ast){
    if(ast->op == AST_ID){
        sym = ast->sym;
        if(struct_var_en){
            struct_var_en = 0;
            struct_var_flag = 1;
        }
        if(sym->name_space == NS_GLOBAL){
            printf("  leaq %s(%%rip), %%rax\n", ast->name);
            printf("  pushq  %%rax\n");
            rsp_allign += 8;
        }
        else{
            sym = ast->sym;
            printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
            printf("  pushq  %%rax\n");
            rsp_allign += 8;
        }
    }
    else if(ast->op == AST_UNARY_PTR){
        codegen(ast->lhs);
    }
    else if(ast->op == AST_DEC){   //for AST_INIT_DEC
        sym = ast->sym;
        printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
        printf("  pushq  %%rax\n");
        rsp_allign += 8;
    }    
    else if(ast->op == AST_STRUCT_ID){
        struct_var_en = 1;
        Type_t* struct_type;
        Type_t* member_type;
        char* member_name = ast->rhs->name;
        char* struct_name;
        codegen_lval(ast->lhs);
        sym = ast->sym;
        if(struct_var_flag){
            struct_name = get_ptr_name(sym->type);
        }
        else{
            struct_name = ast->lhs->name;
        }
        if((struct_type = map_search(struct_dec, struct_name)) == NULL){
                    fprintf(stderr, "Error : struct %s was not declared\n", struct_name);
                    assert(0);
        }
        if((member_type = struct_search(struct_type, member_name)) == NULL){
                    fprintf(stderr, "Error : struct %s does not have %s\n", struct_name, member_name);
                    assert(0);
        }
        printf("  pop %%rax\n");
        rsp_allign -= 8;

        int offset;
        offset = member_type->offset;
        printf("  addq $%d, %%rax\n", offset);
        printf("  pushq  %%rax\n");
        rsp_allign += 8;
        struct_var_flag = 0;
    }
    else{
        fprintf(stderr, "not ID lvalue\n");
        assert(0);
    }
}

int codegen_arg(Node_t* ast, int num){
    if(ast->op != AST_ARG_LIST){
        codegen(ast);
        num++;
    }
    else{
        if(ast->lhs != NULL){
            num = codegen_arg(ast->lhs, num);
        }
        if(ast->rhs != NULL){
            num = codegen_arg(ast->rhs, num);
        }
    }

    return num;
}

int is_ptr(Type_t* type){
    if(type->ty == TYPE_PTR || (type->ty == TYPE_ARRAY && type->ptrof->ty == TYPE_PTR))
        return 1;
    else
        return 0;
}

int allign(int x, int t){
    int n = 0;
    while(x > n){
        n += t;
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
        Type_t* ast_type = ast->type;
        char *name = vector_get(global_init->key, i);
        printf("  .data\n");
        printf("  .global %s\n", name);
        ast = ast->rhs;
        if(ast->op == AST_UNARY_MINUS){
            ast = ast->lhs;
            ast->val *= -1;
        }

        if(ast->op == AST_STRING){
            char* label = map_search(strlabel, ast->name);
            printf("%s:\n", name);
            printf("  .quad %s\n", label);
        }else{
            //Symbol_t* sym = map_search(global, name);
            printf("%s:\n", name);
            if(ast->op != AST_INIT_LIST){
                int size = get_type_size(ast->type);
                if(size == 4){
                    printf("  .long %d\n", ast->val);
                }
                else if(size == 1){
                    printf("  .byte %d\n", ast->val);
                }
            }
            else{
                int offset = 0;
                Node_t* init_list[1024];
                for(int i = 0; i < 1024; i++) init_list[i] = NULL;
                int size = get_type_size(ast->type);
                int array_size = ast_type->array_size;
                while(ast != NULL){
                    Node_t* tmp = ast->lhs;
                    if(tmp->op == AST_DESIG){
                        offset = tmp->lhs->val;
                        init_list[offset] = tmp->rhs;
                    }
                    else init_list[offset] = tmp;
                    offset++;
                    ast = ast->rhs;
                }
                offset = 0;
                int zeros = 0;
                while(offset < array_size){
                    if(init_list[offset] == NULL){
                        zeros++;
                        offset++;
                        continue;
                    }
                    Node_t* init = init_list[offset++];
                    if(init->op == AST_STRING){
                        if(zeros > 0){
                            printf("  .zero %d\n", zeros*size);
                            zeros = 0;
                        }
                        char* label = map_search(strlabel, init->name);
                        printf("  .quad %s\n", label);
                    }
                    else{
                        if(init->val == 0) zeros++;
                        else{
                            if(zeros > 0){
                                printf("  .zero %d\n", zeros*size);
                                zeros = 0;
                            }

                            if(size == 4){
                                printf("  .long %d\n", init->val);
                            }
                            else if(size == 1){
                                printf("  .byte %d\n", init->val);
                            }
                        }
                    }
                }
                if(zeros > 0){
                    printf("  .zero %d\n", zeros*size);
                    zeros = 0;
                }
            }
        }
    }
}

void codegen_global(){
    for(int i = 0; i < map_size(global); i++){
        Symbol_t* sym = vector_get(global->val, i);
        if(sym->ast->ext) continue;
        if(sym->role == FUNC) continue;
        if(map_search(global_init, sym->name) != NULL) continue;
        int size = get_type_size(sym->type);
        if(sym->type->ty == TYPE_ARRAY)
            size *= sym->type->array_size;
        printf("  .comm  %s,%d\n", sym->name, size);
    }
}
