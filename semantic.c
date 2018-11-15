#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


SymTable_t* sym_table;

SymTable_t* sym_table_new(){
    SymTable_t* symt = (SymTable_t*)malloc(sizeof(SymTable_t));
    symt->global = vector_new();
    symt->arg = vector_new();
    symt->label = NULL; //not implemented
    symt->string = vector_new();
    symt->local_index = -1;
    return symt;
}

Symbol_t* sym_new(char* name, int type, Node_t* ast, int name_space){
    Symbol_t* sym = (Symbol_t*)malloc(sizeof(Symbol_t));
    sym->name = name;
    sym->type = type;
    sym->ast = ast;
    sym->name_space = name_space;
    return sym;
}

void sem_analy(Node_t* ast, int level){
    switch(ast->op){
        case AST_FUNC:
            sym_table = sym_table_new(ast);
            break;

        case AST_DEC:
            //Symbol_t* sym = sym_new(ast->name, ast->type, ast, );
            break;

        case AST_COMP_STMT:
            level++;

        default:
            if(ast->lhs != NULL) sem_analy(ast->lhs, level);
            if(ast->rhs != NULL) sem_analy(ast->rhs, level);
            break;
    }
}

