#include "semantic.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


SymTable_t* sym_table;
Symbol_t* sym;
Map_t* global;
Map_t* global_init;
Vector_t* str_lit;
Type_t* type;

SymTable_t* sym_table_new(){
    SymTable_t* symt = (SymTable_t*)malloc(sizeof(SymTable_t));
    symt->global = map_new();
    symt->arg = map_new();
    symt->label = map_new(); //not implemented
    symt->string = map_new();
    symt->local_index = -1;
    symt->num_var = 0;
    return symt;
}

Symbol_t* sym_new(char* name, struct Type* type, Node_t* ast, int name_space, int offset, int role){
    Symbol_t* sym = (Symbol_t*)malloc(sizeof(Symbol_t));
    sym->name = name;
    sym->type = type;
    sym->ast = ast;
    sym->name_space = name_space;
    sym->offset = offset;
    sym->role = role;
    return sym;
}

void sem_analy(Node_t* ast, int level){
    if(ast->op == AST_ID){
        if((sym = map_search(sym_table->local[0], ast->name)) == NULL)
            if((sym = map_search(sym_table->arg, ast->name)) == NULL)
                if((sym = map_search(sym_table->global, ast->name)) == NULL) {
                    fprintf(stderr, "Error : %s was not declared\n", ast->name);
                    assert(0);
                }
        Type_t* tmp = sym->type;
        ast->type = tmp;
    }

    else if(ast->op == AST_STRING){
        ast->type = (Type_t*)malloc(sizeof(Type_t));
        ast->type->ty = TYPE_PTR;
        vector_push(str_lit, ast->name);
    }

    else if(ast->op == AST_UNARY_PTR){
        sem_analy(ast->lhs, level);
        type = (Type_t*)ast->lhs->type;
        if(type->ty == TYPE_PTR || type->ty == TYPE_ARRAY){
            ast->type = type->ptrof;
        }
        else{
            fprintf(stderr, "Error : %s is not a pointer/array\n", ast->lhs->name);
            assert(0);
        }
    }

    else if(ast->op == AST_UNARY_ADR){
        sem_analy(ast->lhs, level);
        type = (Type_t*)malloc(sizeof(type));
        type->ty = TYPE_PTR;
        type->ptrof = ast->lhs->type;
        ast->type = type;
    }

    else if(ast->op == AST_UNARY_MINUS){
        sem_analy(ast->lhs, level);
        ast->type = ast->lhs->type;
    }

    else if(ast->op == AST_ADD || ast->op == AST_SUB){
        sem_analy(ast->lhs, level);
        sem_analy(ast->rhs, level);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        int left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
        int right_ptr = (ast->rtype->ty == TYPE_PTR || ast->rtype->ty == TYPE_ARRAY);
        if(left_ptr && right_ptr){
            fprintf(stderr, "Error : add/sub TYPE_PTR to TYPE_PTR.\n");
            assert(0);
        }
        else if(left_ptr)
            ast->type = ast->ltype;
        else if(right_ptr)
            ast->type = ast->rtype;
        else{
            if(get_type_size(ast->ltype) >= get_type_size(ast->rtype))
                ast->type = ast->ltype;
            else
                ast->type = ast->rtype;
        }
    }

    else if(ast->op == AST_MUL || ast->op == AST_DIV){
        sem_analy(ast->lhs, level);
        sem_analy(ast->rhs, level);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        //ast->type = (Type_t*)malloc(sizeof(Type_t));
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : mul/div TYPE_PTR.\n");
            assert(0);
        }
        else{
            if(get_type_size(ast->ltype) >= get_type_size(ast->rtype))
                ast->type = ast->ltype;
            else
                ast->type = ast->rtype;
        }
    }

    else if(ast->op == AST_AND){
        sem_analy(ast->lhs, level);
        sem_analy(ast->rhs, level);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : and TYPE_PTR.\n");
            assert(0);
        }
        else{
            if(get_type_size(ast->ltype) >= get_type_size(ast->rtype))
                ast->type = ast->ltype;
            else
                ast->type = ast->rtype;
        }
    }

    else if(ast->op == AST_ASSIGN){
        sem_analy(ast->lhs, level);
        sem_analy(ast->rhs, level);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        int left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
        int right_ptr = (ast->rtype->ty == TYPE_PTR || ast->rtype->ty == TYPE_ARRAY);
        if(left_ptr != right_ptr){
            fprintf(stderr, "left type(%s) does not match right type(%s)\n", type_name[ast->ltype->ty], type_name[ast->rtype->ty]);
            assert(0);
        }
        ast->type = ast->ltype;
    }

    else if(ast->op == AST_FUNC){
        if(ast->lhs != NULL) sem_analy(ast->lhs, level);
        sym_table = sym_table_new();
        ast->sym_table = sym_table;
        for(int i = 0; i < (int)map_size(global); i++){
            map_push(sym_table->global, vector_get(global->key, i), vector_get(global->val, i));
        }
        for(int i = 0; i < ast->num_arg; i++){
            Node_t* decl = vector_get(ast->args, i);
            sym_table->num_var++;
            sym_table->offset += get_type_size(decl->type);
            sym = sym_new(decl->name, decl->type, ast, NS_ARG, sym_table->offset, VAR);
            map_push(sym_table->arg, decl->name, sym);
        }
        sym_table->local[0] = map_new();
        if(ast->rhs != NULL) sem_analy(ast->rhs, level);
    }

    else if(ast->op == AST_INIT_DEC){
        sem_analy(ast->lhs, level);
        sem_analy(ast->rhs, level);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        int left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
        int right_ptr = (ast->rtype->ty == TYPE_PTR || ast->rtype->ty == TYPE_ARRAY);
        if(left_ptr != right_ptr){
            fprintf(stderr, "left type(%s) does not match right type(%s)\n", type_name[ast->ltype->ty], type_name[ast->rtype->ty]);
            assert(0);
        }
        ast->type = ast->ltype;
        if(ast->global){
            map_push(global_init, ast->lhs->name, ast->rhs);
        }
    }

    else if(ast->op == AST_FUNC_DEC){
        sym = sym_new(ast->name, ast->type, ast, NS_GLOBAL, 0, FUNC);
        map_push(global, ast->name, sym);
    }

    else if(ast->op == AST_FUNC_CALL){
        sem_analy(ast->lhs, level);
        ast->type = ast->lhs->type;
        if(ast->rhs != NULL) sem_analy(ast->rhs, level);
    }

    else if(ast->op == AST_DEC){
        if(ast->global){
            sym = sym_new(ast->name, ast->type, ast, NS_GLOBAL, 0, VAR);
            map_push(global, ast->name, sym);
            return;
        }
        sym_table->num_var++;
        int add_offset;
        add_offset = get_type_size(ast->type);
        if(ast->type->ty == TYPE_ARRAY)
            add_offset *= ast->type->array_size;
        sym_table->offset += add_offset;
        sym = sym_new(ast->name, ast->type, ast, NS_LOCAL, sym_table->offset, VAR);
        map_push(sym_table->local[0], ast->name, sym);
    }

    else if(ast->op == AST_COMP_STMT){
        level++;
        if(ast->lhs != NULL) sem_analy(ast->lhs, level);
        if(ast->rhs != NULL) sem_analy(ast->rhs, level);
    }

    else if(ast->op == AST_FOR){
        if(ast->lfor != NULL) sem_analy(ast->lfor, level);
        if(ast->mfor != NULL) sem_analy(ast->mfor, level);
        if(ast->rfor != NULL) sem_analy(ast->rfor, level);
        sem_analy(ast->lhs, level);
    }

    else if(ast->op == AST_IF){
        sem_analy(ast->lhs, level);
        sem_analy(ast->rhs, level);
        if(ast->else_stmt != NULL) sem_analy(ast->else_stmt, level);
    }

    else if(ast->op == AST_WHILE){
        sem_analy(ast->lhs, level);
        sem_analy(ast->rhs, level);
    }

    else{
        if(ast->lhs != NULL) sem_analy(ast->lhs, level);
        if(ast->rhs != NULL) sem_analy(ast->rhs, level);
    }
}


