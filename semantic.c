#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


SymTable_t* sym_table;
Symbol_t* sym;
Map_t* global;
Type_t* type;

SymTable_t* sym_table_new(){
    SymTable_t* symt = (SymTable_t*)malloc(sizeof(SymTable_t));
    symt->global = map_new();
    symt->arg = map_new();
    symt->label = NULL; //not implemented
    symt->string = map_new();
    symt->local_index = -1;
    symt->num_var = 0;
    return symt;
}

Symbol_t* sym_new(char* name, struct Type* type, Node_t* ast, int name_space, int offset){
    Symbol_t* sym = (Symbol_t*)malloc(sizeof(Symbol_t));
    sym->name = name;
    sym->type = type;
    sym->ast = ast;
    sym->name_space = name_space;
    sym->offset = offset;
    return sym;
}

void sem_analy(Node_t* ast, int level){
    int left_ptr;
    int right_ptr;
    switch(ast->op){
        case AST_FUNC:
            if(ast->lhs != NULL) sem_analy(ast->lhs, level);
            sym_table = sym_table_new();
            ast->sym_table = sym_table;
            for(int i = 0; i < (int)map_size(global); i++){
                map_push(sym_table->global, vector_get(global->key, i), vector_get(global->val, i));
            }
            for(int i = 0; i < ast->num_arg; i++){
                Node_t* decl = vector_get(ast->args, i);
                sym_table->num_var++;
                if(decl->type->ty == TYPE_PTR)
                    sym_table->offset += 8;
                else
                    sym_table->offset += 8;
                sym = sym_new(decl->name, decl->type, ast, NS_ARG, sym_table->offset);
                map_push(sym_table->arg, decl->name, sym);
            }
            sym_table->local[0] = map_new();
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;

        case AST_ASSIGN:
            sem_analy(ast->lhs, level);
            sem_analy(ast->rhs, level);
            ast->ltype = ast->lhs->type;
            ast->rtype = ast->rhs->type;
            left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
            right_ptr = (ast->rtype->ty == TYPE_PTR || ast->rtype->ty == TYPE_ARRAY);
            if(left_ptr != right_ptr){
                fprintf(stderr, "left type(%s) does not match right type(%s)\n", type_name[ast->ltype->ty], type_name[ast->rtype->ty]);
                assert(0);
            }
            break;

        case AST_FUNC_DEC:
            sym = sym_new(ast->name, ast->type, ast, NS_GLOBAL, 0);
            map_push(global, ast->name, sym);
            //map_push(sym_table->global, ast->name, sym);
            break;

        case AST_DEC:
            sym_table->num_var++;
            int add_offset;
            if(ast->type->ty == TYPE_PTR)
                add_offset = 8;
            else
                add_offset = 8;
            if(ast->type->ty == TYPE_ARRAY)
                add_offset *= ast->type->array_size;
            sym_table->offset += add_offset;
            sym = sym_new(ast->name, ast->type, ast, NS_LOCAL, sym_table->offset);
            map_push(sym_table->local[0], ast->name, sym);
            break;

        case AST_ID:
            if((sym = map_search(sym_table->local[0], ast->name)) == NULL)
                if((sym = map_search(sym_table->arg, ast->name)) == NULL)
                    if((sym = map_search(sym_table->global, ast->name)) == NULL) {
                        fprintf(stderr, "Error : %s was not declared\n", ast->name);
                        assert(0);
                    }
            Type_t* tmp = sym->type;
            ast->type = tmp;
            break;

        case AST_COMP_STMT:
            level++;
            if(ast->lhs != NULL) sem_analy(ast->lhs, level);
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;

        case AST_ADD:
        case AST_SUB:
            sem_analy(ast->lhs, level);
            sem_analy(ast->rhs, level);
            ast->ltype = ast->lhs->type;
            ast->rtype = ast->rhs->type;
            left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
            right_ptr = (ast->rtype->ty == TYPE_PTR || ast->rtype->ty == TYPE_ARRAY);
            if(left_ptr && right_ptr){
                fprintf(stderr, "Error : add/sub TYPE_PTR to TYPE_PTR.\n");
                assert(0);
            }
            else if(left_ptr)
                ast->type = ast->ltype;
            else if(right_ptr)
                ast->type = ast->rtype;
            else
                ast->type = ast->ltype;
            break;

        case AST_MUL:
        case AST_DIV:
            sem_analy(ast->lhs, level);
            sem_analy(ast->rhs, level);
            ast->ltype = ast->lhs->type;
            ast->rtype = ast->rhs->type;
            ast->type = (Type_t*)malloc(sizeof(Type_t));
            if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
                fprintf(stderr, "Error : mul/div TYPE_PTR.\n");
                assert(0);
            }
            else
                ast->type->ty = TYPE_INT;
            break;

        case AST_FOR:
            if(ast->lfor != NULL) sem_analy(ast->lfor, level);
            if(ast->mfor != NULL) sem_analy(ast->mfor, level);
            if(ast->rfor != NULL) sem_analy(ast->rfor, level);
            sem_analy(ast->lhs, level);
            break;

        case AST_IF:
            sem_analy(ast->lhs, level);
            sem_analy(ast->rhs, level);
            if(ast->else_stmt != NULL) sem_analy(ast->else_stmt, level);
            break;

        case AST_WHILE:
            sem_analy(ast->lhs, level);
            sem_analy(ast->rhs, level);
            break;

        case AST_POST_FIX:
            sem_analy(ast->lhs, level);
            ast->type = ast->lhs->type;
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;

        case AST_UNARY_PTR:
            sem_analy(ast->lhs, level);
            type = (Type_t*)ast->lhs->type;
            if(type->ty == TYPE_PTR || type->ty == TYPE_ARRAY){
                ast->type = type->ptrof;
            }
            else{
                fprintf(stderr, "Error : %s is not a pointer/array\n", ast->lhs->name);
                assert(0);
            }
            break;

        case AST_UNARY_ADR:
            sem_analy(ast->lhs, level);
            type = (Type_t*)malloc(sizeof(type));
            type->ty = TYPE_PTR;
            type->ptrof = ast->lhs->type;
            ast->type = type;
            break;

        default:
            if(ast->lhs != NULL) sem_analy(ast->lhs, level);
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;
    }
}

