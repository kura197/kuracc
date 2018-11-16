#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


SymTable_t* sym_table;
Symbol_t* sym;

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

Symbol_t* sym_new(char* name, int type, Node_t* ast, int name_space, int offset){
    Symbol_t* sym = (Symbol_t*)malloc(sizeof(Symbol_t));
    sym->name = name;
    sym->type = type;
    sym->ast = ast;
    sym->name_space = name_space;
    sym->offset = offset;
    return sym;
}

void sem_analy(Node_t* ast, int level){
    switch(ast->op){
        case AST_FUNC:
            sym_table = sym_table_new();
            for(int i = 0; i < ast->num_arg; i++){
                Node_t* decl = vector_get(ast->args, i);
                sym = sym_new(decl->name, decl->type, ast, NS_ARG, 8+8*sym_table->num_var);
                sym_table->num_var++;
                map_push(sym_table->arg, decl->name, sym);
            }
            break;

        case AST_DEC:
            sym = sym_new(ast->name, ast->type, ast, NS_LOCAL, 8+8*sym_table->num_var);
            sym_table->num_var++;
            map_push(sym_table->local[0], ast->name, sym);
            break;

        case AST_COMP_STMT:
            level++;

        default:
            if(ast->lhs != NULL) sem_analy(ast->lhs, level);
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;
    }
}

