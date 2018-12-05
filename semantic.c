#include "semantic.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


SymTable_t* sym_table;
Symbol_t* sym;
Map_t* global;
Map_t* global_init;
Vector_t* str_lit;
Type_t* type;
char* func_name;

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

Symbol_t* local_sym_search(SymTable_t* symt, char* name){
    Symbol_t* sym;
    for(int i = symt->local_index; i >= 0; i--){
        sym = map_search(symt->local[i], name);
        if(sym != NULL) break;
    }
    return sym;
}

void sem_analy(Node_t* ast){
    if(ast->op == AST_ID){
        int *offset;
        if((offset = map_search(enum_dec, ast->name)) != NULL){
            ast->type = (Type_t*)malloc(sizeof(Type_t));
            ast->type->ty = TYPE_INT;
            ast->sym = sym_new(ast->name, ast->type, ast, NS_GLOBAL, *offset, VAR);
        }
        else{
            if((sym = local_sym_search(sym_table, ast->name)) == NULL)
                if((sym = map_search(sym_table->arg, ast->name)) == NULL)
                    if((sym = map_search(sym_table->global, ast->name)) == NULL) {
                        fprintf(stderr, "Error : %s was not declared\n", ast->name);
                        assert(0);
                    }
            ast->type = sym->type;
            ast->sym = sym;
        }
    }

    else if(ast->op == AST_STRING){
        ast->type = (Type_t*)malloc(sizeof(Type_t));
        ast->type->ty = TYPE_PTR;
        vector_push(str_lit, ast->name);
    }

    else if(ast->op == AST_POST_INC || ast->op == AST_POST_DEC){
        sem_analy(ast->lhs);
        ast->type = ast->lhs->type;
        ast->ltype = ast->type;
    }

    else if(ast->op == AST_UNARY_PTR){
        sem_analy(ast->lhs);
        type = (Type_t*)ast->lhs->type;
        ast->sym = ast->lhs->sym;
        ast->name = ast->lhs->name;
        if(type->ty == TYPE_PTR || type->ty == TYPE_ARRAY){
            ast->type = type->ptrof;
        }
        else{
            fprintf(stderr, "Error : %s is not a pointer/array\n", ast->lhs->name);
            assert(0);
        }
    }

    else if(ast->op == AST_UNARY_ADR){
        sem_analy(ast->lhs);
        type = (Type_t*)malloc(sizeof(type));
        type->ty = TYPE_PTR;
        type->ptrof = ast->lhs->type;
        ast->type = type;
        ast->name = ast->lhs->name;
    }

    else if(ast->op == AST_UNARY_MINUS){
        sem_analy(ast->lhs);
        ast->type = ast->lhs->type;
        ast->ltype = ast->type;
    }

    else if(ast->op == AST_UNARY_REV){
        sem_analy(ast->lhs);
        ast->type = ast->lhs->type;
        ast->ltype = ast->type;
    }

    else if(ast->op == AST_SIZEOF){
        // sizeof unary_expr
        if(ast->lhs != NULL){
            sem_analy(ast->lhs);
            ast->type = ast->lhs->type;
            ast->ltype = ast->type;
            ast->val = get_type_size(ast->type);
        }
        // sizeof(type)
        else{
            ast->val = get_type_size(ast->type);
        }
        ast->type = (Type_t*)malloc(sizeof(Type_t));
        ast->type->ty = TYPE_INT;
    }

    else if(ast->op == AST_STRUCT_ID){
        //if(ast->lhs->op != AST_ID){
        //    fprintf(stderr, "Error : not yet implemented.\n");
        //    assert(0);
        //}
        sem_analy(ast->lhs);
        ast->ltype = ast->lhs->type;
        int left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
        if((ast->op == AST_STRUCT_ID && left_ptr)){
            fprintf(stderr, "Error : struct type is invalid.\n");
            assert(0);
        }
        char *var_name = ast->lhs->name;
        if((sym = local_sym_search(sym_table, var_name)) == NULL)
            if((sym = map_search(sym_table->arg, var_name)) == NULL)
                if((sym = map_search(sym_table->global, var_name)) == NULL) {
                    fprintf(stderr, "Error : %s was not declared\n", var_name);
                    assert(0);
                }
        Type_t* struct_type;
        Type_t* member_type;
        char* struct_name = (ast->lhs->op == AST_ID) ? sym->type->name : sym->type->ptrof->name;
        if((struct_type = map_search(struct_dec, struct_name)) == NULL){
                    fprintf(stderr, "Error : struct %s was not declared\n", ast->type->name);
                    assert(0);
        }
        char *member_name = ast->rhs->name;
        if((member_type = map_search(struct_type->member, member_name)) == NULL){
                    fprintf(stderr, "Error : struct %s does not have %s\n", ast->type->name, member_name);
                    assert(0);
        }
        ast->type = member_type; 
        ast->sym = sym;
        ast->name = ast->lhs->name;
    }

    else if(ast->op == AST_PRE_INC || ast->op == AST_PRE_DEC){
        sem_analy(ast->lhs);
        ast->type = ast->lhs->type;
        ast->ltype = ast->type;
    }

    else if(ast->op == AST_ADD || ast->op == AST_SUB){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
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

    else if(ast->op == AST_MUL || ast->op == AST_DIV || ast->op == AST_REM){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : mul/div/rem TYPE_PTR.\n");
            assert(0);
        }
        else{
            if(get_type_size(ast->ltype) >= get_type_size(ast->rtype))
                ast->type = ast->ltype;
            else
                ast->type = ast->rtype;
        }
    }

    else if(ast->op == AST_LSHIFT || ast->op == AST_RSHIFT){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : shift TYPE_PTR.\n");
            assert(0);
        }
        else{
            if(get_type_size(ast->ltype) >= get_type_size(ast->rtype))
                ast->type = ast->ltype;
            else
                ast->type = ast->rtype;
        }
    }

    else if(ast->op == AST_LEQ || ast->op == AST_SEQ || ast->op == AST_SMALL || ast->op == AST_LARGE){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : >= / <= / < / > TYPE_PTR.\n");
            assert(0);
        }
        else{
            if(get_type_size(ast->ltype) >= get_type_size(ast->rtype))
                ast->type = ast->ltype;
            else
                ast->type = ast->rtype;
        }
    }

    else if(ast->op == AST_EQ || ast->op == AST_NEQ){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : && / || TYPE_PTR.\n");
            assert(0);
        }
        else{
            ast->type = (Type_t*)malloc(sizeof(Type_t));
            ast->type->ty = TYPE_INT;
        }
    }

    else if(ast->op == AST_AND || ast->op == AST_OR || ast->op == AST_EXOR){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : & / | / ^ TYPE_PTR.\n");
            assert(0);
        }
        else{
            if(get_type_size(ast->ltype) >= get_type_size(ast->rtype))
                ast->type = ast->ltype;
            else
                ast->type = ast->rtype;
        }
    }

    else if(ast->op == AST_LOG_AND || ast->op == AST_LOG_OR){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_PTR || ast->rtype->ty == TYPE_PTR){
            fprintf(stderr, "Error : && / || TYPE_PTR.\n");
            assert(0);
        }
        else{
            ast->type = (Type_t*)malloc(sizeof(Type_t));
            ast->type->ty = TYPE_INT;
        }
    }

    else if(ast->op == AST_COND){
        sem_analy(ast->lhs);
        sem_analy(ast->lcond);
        sem_analy(ast->rcond);
        ast->ltype = ast->lcond->type;
        ast->rtype = ast->rcond->type;
        if(ast->ltype->ty != ast->rtype->ty){
            fprintf(stderr, "Error : conditional expr() type. %s and %s\n", type_name[ast->ltype->ty], type_name[ast->rtype->ty]);
            assert(0);
        }
        ast->type = ast->ltype;
    }

    else if(ast->op == AST_ASSIGN){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_VOID || ast->rtype->ty == TYPE_VOID){
            fprintf(stderr, "Error : cannot assign type void\n");
            assert(0);
        }
        int left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
        int right_ptr = (ast->rtype->ty == TYPE_PTR || ast->rtype->ty == TYPE_ARRAY);
        if(left_ptr != right_ptr){
            fprintf(stderr, "left type(%s) does not match right type(%s)\n", type_name[ast->ltype->ty], type_name[ast->rtype->ty]);
            assert(0);
        }
        ast->type = ast->ltype;
    }

    else if(ast->op == AST_FUNC){
        if(ast->lhs != NULL) sem_analy(ast->lhs);
        func_name = ast->name;
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
        //sym_table->local[0] = map_new();
        if(ast->rhs != NULL) sem_analy(ast->rhs);
    }

    else if(ast->op == AST_INIT_DEC){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        ast->ltype = ast->lhs->type;
        ast->rtype = ast->rhs->type;
        if(ast->ltype->ty == TYPE_VOID || ast->rtype->ty == TYPE_VOID){
            fprintf(stderr, "Error : cannot assign type void\n");
            assert(0);
        }
        int left_ptr = (ast->ltype->ty == TYPE_PTR || ast->ltype->ty == TYPE_ARRAY);
        int right_ptr = (ast->rtype->ty == TYPE_PTR || ast->rtype->ty == TYPE_ARRAY);
        if(left_ptr != right_ptr){
            fprintf(stderr, "left type(%s) does not match right type(%s)\n", type_name[ast->ltype->ty], type_name[ast->rtype->ty]);
            assert(0);
        }
        ast->type = ast->ltype;
        ast->sym = ast->lhs->sym;
        if(ast->global){
            map_push(global_init, ast->lhs->name, ast->rhs);
        }
    }

    else if(ast->op == AST_FUNC_DEC){
        sym = sym_new(ast->name, ast->type, ast, NS_GLOBAL, 0, FUNC);
        map_push(global, ast->name, sym);
    }

    else if(ast->op == AST_FUNC_CALL){
        sem_analy(ast->lhs);
        ast->type = ast->lhs->type;
        if(ast->rhs != NULL) sem_analy(ast->rhs);
    }

    //else if(ast->op == AST_BLOCK){
    //    sem_analy(ast->lhs);
    //    if(ast->rhs != NULL) sem_analy(ast->rhs);
    //    ast->type = ast->lhs->type;
    //}

    else if(ast->op == AST_DEC){
        if(ast->global){
            sym = sym_new(ast->name, ast->type, ast, NS_GLOBAL, 0, VAR);
            map_push(global, ast->name, sym);
            ast->sym = sym;
            return;
        }
        if(map_search(sym_table->local[sym_table->local_index], ast->name) != NULL){
            fprintf(stderr, "Error : %s was already declared.\n", ast->name);
            assert(0);
        }
        sym_table->num_var++;
        int add_offset;
        add_offset = get_type_size(ast->type);
        if(ast->type->ty == TYPE_ARRAY)
            add_offset *= ast->type->array_size;
        sym_table->offset += add_offset;
        sym = sym_new(ast->name, ast->type, ast, NS_LOCAL, sym_table->offset, VAR);
        map_push(sym_table->local[sym_table->local_index], ast->name, sym);
        ast->sym = sym;
    }

    else if(ast->op == AST_COMP_STMT){
        sym_table->local_index++;
        sym_table->local[sym_table->local_index] = map_new();
        if(ast->lhs != NULL) sem_analy(ast->lhs);
        sym_table->local_index--;
    }

    else if(ast->op == AST_COMP_STMT){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
    }

    else if(ast->op == AST_FOR){
        if(ast->lfor != NULL) sem_analy(ast->lfor);
        if(ast->mfor != NULL) sem_analy(ast->mfor);
        if(ast->rfor != NULL) sem_analy(ast->rfor);
        sem_analy(ast->lhs);
    }

    else if(ast->op == AST_IF){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
        if(ast->else_stmt != NULL) sem_analy(ast->else_stmt);
    }

    else if(ast->op == AST_WHILE){
        sem_analy(ast->lhs);
        sem_analy(ast->rhs);
    }

    else if(ast->op == AST_RET){
        sym = map_search(sym_table->global, func_name);
        int type = sym->type->ty;
        if(type == TYPE_VOID){
            if(ast->lhs != NULL){
                fprintf(stderr, "Error : function %s is type void\n", func_name);
                assert(0);
            }
        }
        else{
            if(ast->lhs == NULL){
                fprintf(stderr, "Error : need return expr at function %s\n", func_name);
                assert(0);
            }
            sem_analy(ast->lhs);
            int ret_type = ast->lhs->type->ty;
            if(type != ret_type){
                fprintf(stderr, "Warning : return type %s is not the same as function %s(%s)\n", type_name[ret_type], func_name, type_name[type]);
            }
        }
    }

    else{
        if(ast->lhs != NULL) sem_analy(ast->lhs);
        if(ast->rhs != NULL) sem_analy(ast->rhs);
    }
}


