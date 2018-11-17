#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


SymTable_t* sym_table;
Symbol_t* sym;
Map_t* global;

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
    int *unary;
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
                sym = sym_new(decl->name, decl->type, ast, NS_ARG, 8+8*sym_table->num_var);
                sym_table->num_var++;
                map_push(sym_table->arg, decl->name, sym);
            }
            sym_table->local[0] = map_new();
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;

        case AST_FUNC_DEC:
            sym = sym_new(ast->name, ast->type, ast, NS_GLOBAL, 0);
            map_push(global, ast->name, sym);
            //map_push(sym_table->global, ast->name, sym);
            break;

        case AST_DEC:
            sym = sym_new(ast->name, ast->type, ast, NS_LOCAL, 8+8*sym_table->num_var);
            sym_table->num_var++;
            map_push(sym_table->local[0], ast->name, sym);
            break;

        case AST_ID:
            if(map_search(sym_table->local[0], ast->name) != NULL) break;
            if(map_search(sym_table->arg, ast->name) != NULL) break;
            if(map_search(sym_table->global, ast->name) != NULL) break;
            fprintf(stderr, "Error : %s was not declared\n", ast->name);
            assert(0);

        case AST_COMP_STMT:
            level++;
            if(ast->lhs != NULL) sem_analy(ast->lhs, level);
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;

        case AST_UNARY_EXPR:
            unary = (int*)vector_get(ast->lhs->unary, 0);
            Node_t* tmp = ast;
            while(tmp->op == AST_UNARY_EXPR)
                tmp = tmp->rhs;
            if(tmp->op == AST_ID){
                if(tmp->unary == NULL) tmp->unary = vector_new();
                vector_push(tmp->unary, unary);
            }
            else{
                if(tmp->lhs->unary == NULL) tmp->lhs->unary = vector_new();
                vector_push(tmp->lhs->unary, unary);
            }
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;

        default:
            if(ast->lhs != NULL) sem_analy(ast->lhs, level);
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;
    }
}

