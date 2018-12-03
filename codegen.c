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
int case_label;
int break_label;
int cont_label;


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
            printf("  pushq %%rax\n");
            rsp_allign += 8;
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
            printf("  pushq %%rax\n");
            rsp_allign += 8;
        }
    }
    else if(ast->op == AST_STRING){
        char* label = map_search(strlabel, ast->name);
        //printf("  movq $%s, %%rax\n", label);
        printf("  leaq %s(%%rip), %%rax\n", label);
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_POST_INC || ast->op == AST_POST_DEC){
        codegen_lval(ast->lhs);
        printf("  pop %%rax\n");
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
        sym = ast->sym;
        Type_t* struct_type;
        Type_t* member_type;
        char* member_name = ast->rhs->name;
        if((struct_type = map_search(struct_dec, sym->type->name)) == NULL){
            fprintf(stderr, "Error : struct %s was not declared\n", ast->type->name);
            assert(0);
        }
        if((member_type = map_search(struct_type->member, member_name)) == NULL){
            fprintf(stderr, "Error : struct %s does not have %s\n", ast->type->name, member_name);
            assert(0);
        }

        if(sym->name_space == NS_GLOBAL){
            Type_t* first_member = vector_get(struct_type->member->val, 0);
            int offset = member_type->offset - first_member->offset;
            if(ast->type->ty == TYPE_ARRAY){
                printf("  leaq %s+%d(%%rip), %%rax\n", ast->name, offset);
            }
            else{
                if(is_ptr(ast->type))
                    printf("  movq %s+%d(%%rip), %%rax\n", ast->name, offset);
                else{
                    if(get_type_size(ast->type) == 4)
                        printf("  movl %s+%d(%%rip), %%eax\n", ast->name, offset);
                    else if(get_type_size(ast->type) == 1)
                        printf("  movzbl %s+%d(%%rip), %%eax\n", ast->name, offset);
                    else
                        assert(0);
                }
            }
            printf("  pushq %%rax\n");
            rsp_allign += 8;
        }
        else{
            int offset = sym->offset - get_type_size(sym->type) + member_type->offset;
            if(member_type->ty == TYPE_ARRAY){
                printf("  leaq -%d(%%rbp), %%rax\n", offset);
            }
            else{
                if(is_ptr(member_type))
                    printf("  movq -%d(%%rbp), %%rax\n", offset);
                else{
                    if(get_type_size(member_type) == 4)
                        printf("  movl -%d(%%rbp), %%eax\n", offset);
                    else if(get_type_size(member_type) == 1)
                        printf("  movzbl -%d(%%rbp), %%eax\n", offset);
                    else
                        assert(0);
                }
            }
            printf("  pushq %%rax\n");
            rsp_allign += 8;
        }

    }
    else if(ast->op == AST_UNARY_PTR){
        codegen(ast->lhs);
        printf("  pop %%rbx\n");
        printf("  movq (%%rbx), %%rax\n");
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_UNARY_ADR){
        codegen_lval(ast->lhs);
    }
    else if(ast->op == AST_UNARY_MINUS){
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        printf("  negl %%eax\n");
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_UNARY_REV){
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        printf("  cmpl $0, %%eax\n");
        printf("  sete %%al\n");
        printf("  movzbl %%al, %%eax\n");
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_PRE_INC || ast->op == AST_PRE_DEC){
        codegen_lval(ast->lhs);
        printf("  pop %%rax\n");
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
        printf("  cmpl $0, %%eax\n");
        printf("  je .L%d\n", tmp_num_jmp1);
        codegen(ast->rhs);
        printf("  pop %%rax\n");
        printf("  cmpl $0, %%eax\n");
        printf("  je .L%d\n", tmp_num_jmp1);
        printf("  movl $1, %%eax\n");
        printf("  jmp .L%d\n", tmp_num_jmp2);
        printf(".L%d:\n", tmp_num_jmp1);
        printf("  movl $0, %%eax\n");
        printf(".L%d:\n", tmp_num_jmp2);
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_LOG_OR){
        int tmp_num_jmp1 = num_jmp;
        int tmp_num_jmp2 = num_jmp+1;
        int tmp_num_jmp3 = num_jmp+2;
        num_jmp+=3;
        codegen(ast->lhs);
        printf("  pop %%rax\n");
        printf("  cmpl $0, %%eax\n");
        printf("  jne .L%d\n", tmp_num_jmp1);
        codegen(ast->rhs);
        printf("  pop %%rax\n");
        printf("  cmpl $0, %%eax\n");
        printf("  je .L%d\n", tmp_num_jmp2);
        printf(".L%d:\n", tmp_num_jmp1);
        printf("  movl $1, %%eax\n");
        printf("  jmp .L%d\n", tmp_num_jmp3);
        printf(".L%d:\n", tmp_num_jmp2);
        printf("  movl $0, %%eax\n");
        printf(".L%d:\n", tmp_num_jmp3);
        printf("  pushq %%rax\n");
    }
    else if(ast->op == AST_COND){
        int tmp_num_jmp1 = num_jmp++;
        int tmp_num_jmp2 = num_jmp++;
        codegen(ast->lhs);
        printf("  pop %%rax\n");
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
            else
                assert(0);
        }
        if(!ast->global) {
            printf("  movl (%%rax), %%ebx\n");
            printf("  pushq %%rax\n");
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
    }
    else if(ast->op == AST_FUNC){
        printf("\n%s:\n", ast->name);
        printf("  pushq %%rbp\n");
        printf("  movq %%rsp, %%rbp\n");

        symt = ast->sym_table;
        rsp_allign = 8;
        if(symt->offset > 0){
            printf("  subq $%d, %%rsp\n", allign4(symt->offset));
            rsp_allign += symt->offset;
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
                || lop == AST_ASSIGN || lop == AST_INIT_DEC)
            printf("  pop %%rax\n");
        codegen(ast->rhs);
        if(rop == AST_ID || rop == AST_INT || rop == AST_CHAR || rop == AST_PRE_INC 
                || rop == AST_PRE_DEC || rop == AST_POST_INC || rop == AST_POST_DEC
                || rop == AST_ASSIGN || rop == AST_INIT_DEC)
            printf("  pop %%rax\n");
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
        for(int i = 0; i < ast->num_case; i++){
            if(ast->case_stmt[i]->lhs->op != AST_INT){
                fprintf(stderr, "Error : case label is only constant\n");
                assert(0);
            }
            printf("  cmpl $%d, %%eax\n", ast->case_stmt[i]->lhs->val);
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
        printf("  cmp $0, %%rax\n");
        printf("  jne .L%d\n", tmp_num_jmp0);
        printf(".L%d:\n", tmp_num_jmp3);
    }
    else if(ast->op == AST_RET){
        if(ast->lhs != NULL){
            codegen(ast->lhs);
            printf("  pop %%rax\n");
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
        if(sym->name_space == NS_GLOBAL){
            printf("  leaq %s(%%rip), %%rax\n", ast->name);
            printf("  pushq  %%rax\n");
            rsp_allign += 4;
        }
        else{
            sym = ast->sym;
            printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
            printf("  pushq  %%rax\n");
            rsp_allign += 4;
        }
    }
    else if(ast->op == AST_UNARY_PTR){
        codegen(ast->lhs);
    }
    else if(ast->op == AST_DEC){   //for AST_INIT_DEC
        sym = ast->sym;
        printf("  leaq -%d(%%rbp), %%rax\n", sym->offset);
        printf("  pushq  %%rax\n");
        rsp_allign += 4;
    }    
    else if(ast->op == AST_STRUCT_ID){
        sym = ast->sym;
        Type_t* struct_type;
        Type_t* member_type;
        char* member_name = ast->rhs->name;
        if((struct_type = map_search(struct_dec, sym->type->name)) == NULL){
                    fprintf(stderr, "Error : struct %s was not declared\n", ast->type->name);
                    assert(0);
        }
        if((member_type = map_search(struct_type->member, member_name)) == NULL){
                    fprintf(stderr, "Error : struct %s does not have %s\n", ast->type->name, member_name);
                    assert(0);
        }

        if(sym->name_space == NS_GLOBAL){
            Type_t* first_member = vector_get(struct_type->member->val, 0);
            int offset = member_type->offset - first_member->offset;
            printf("  leaq %s+%d(%%rip), %%rax\n", ast->name, offset);
            printf("  pushq  %%rax\n");
            rsp_allign += 4;
        }
        else{
            int offset = sym->offset - get_type_size(sym->type) + member_type->offset;
            printf("  leaq -%d(%%rbp), %%rax\n", offset);
            printf("  pushq  %%rax\n");
            rsp_allign += 4;
        }
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

        printf("  .data\n");
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

