//#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "mycc.h"


Node_t** case_stmt;
int num_case;
Node_t* default_stmt;
Map_t* struct_dec;
Map_t* enum_dec;
Map_t* typedef_dec;
int ext_flag;

char *ast_name[128] = {
    "AST_INT",
    "AST_CHAR",
    "AST_ID",
    "AST_STRING",
    "AST_UNARY_ADR",
    "AST_UNARY_PTR",
    "AST_UNARY_MINUS",
    "AST_UNARY_REV",
    "AST_CAST",
    "AST_POST_INC",
    "AST_POST_DEC",
    "AST_STRUCT_ID",
    "AST_PRE_INC",
    "AST_PRE_DEC",
    "AST_SIZEOF",
    "AST_ADD",
    "AST_SUB",
    "AST_MUL",
    "AST_DIV",
    "AST_REM",
    "AST_LSHIFT",
    "AST_RSHIFT",
    "AST_LEQ", 
    "AST_SEQ", 
    "AST_LARGE", 
    "AST_SMALL", 
    "AST_EQ",
    "AST_NEQ",
    "AST_AND",
    "AST_EXOR",
    "AST_OR",
    "AST_LOG_OR",
    "AST_LOG_AND",
    "AST_ASSIGN",
    "AST_COND",
    "AST_EXPR",
    "AST_FUNC_CALL",
    "AST_ARG_LIST",
    "AST_FUNC",
    "AST_PARA_LIST",
    "AST_COMP_STMT",
    "AST_BLOCK",
    "AST_DECLN",
    "AST_DEC",
    "AST_INIT_DEC",
    //"AST_STRUCT",
    "AST_FUNC_DEC",
    "AST_WHILE",
    "AST_IF",
    "AST_ELSE",
    "AST_SWITCH",
    "AST_CASE",
    "AST_DEFAULT",
    "AST_DO",
    "AST_FOR",
    "AST_RET",
    "AST_CONT",
    "AST_BREAK",
    "AST_INIT_LIST",
    "AST_DESIG"
};

char *type_name[16] = {
    "TYPE_UNKNOWN",
    "TYPE_VOID",
    "TYPE_CHAR",
    "TYPE_INT",
    "TYPE_LONG",
    "TYPE_PTR",
    "TYPE_STRUCT",
    "TYPE_ENUM",
    "TYPE_ARRAY"
};

Node_t* make_node(){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = 0;
    node->lhs = NULL;
    node->rhs = NULL;
    node->val = 0;
    node->name = NULL;
    node->num_arg = 0;
    node->args = NULL;
    node->lfor = NULL;
    node->mfor = NULL;
    node->rfor = NULL;
    node->else_stmt = NULL;
    node->lcond = NULL;
    node->rcond = NULL;
    node->type = NULL;
    node->sym_table = NULL;
    node->sym = NULL;
    node->ltype = NULL;
    node->rtype = NULL;
    node->global = 0;
    node->ext = 0;
    node->case_stmt = NULL;
    node->default_stmt = NULL;
    node->num_case = 0;
    return node;
}

Type_t* make_type(){
    Type_t* type = (Type_t*)malloc(sizeof(Type_t));
    type->ty = 0;
    type->ptrof = NULL;
    type->array_size = 0;
    type->name = NULL;
    type->member = NULL;
    type->offset = 0;
    type->typeof = NULL;
    return type;
}

Node_t* new_node(int op, Node_t* lhs, Node_t* rhs){
    Node_t* node = make_node();
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node_t* new_node_num(int val, int type){
    Node_t* node = make_node();
    node->op = AST_INT;
    node->val = val;
    node->type = make_type();
    node->type->ty = type;
    return node;
}

Node_t* new_node_ID(char* ID){
    Node_t* node = make_node();
    node->op = AST_ID;
    node->name = ID;
    return node;
}

Node_t* new_node_STRING(char* str){
    Node_t* node = make_node();
    node->op = AST_STRING;
    node->name = str;
    return node;
}

Node_t* new_node_DEC(char* name){
    Node_t* node = make_node();
    node->op = AST_DEC;
    node->name = name;
    return node;
}

Node_t* new_node_name(int op, char* name){
    Node_t* node = make_node();
    node->op = op;
    node->name = name;
    return node;
}

Node_t* new_node_assign(int op, Node_t* lval, Node_t* rval){
    // x += y -> x = x+y; x;
    Node_t* tmp = new_node(op, lval, rval);
    Node_t* node = new_node(AST_ASSIGN, lval, tmp);
    return node;
}

Node_t* conv2ptr(Node_t* node){
    Node_t* array_at = expr();
    Node_t* add = new_node(AST_ADD, node, array_at);
    Node_t* unary_ptr = new_node(AST_UNARY_PTR, add, NULL);
    return unary_ptr;
}

void error(Token_t* tk){
    fprintf(stderr, "read token at line %d : %s\n", tk->row, token_name[tk->kind]);
    //assert(0);
}

void add_type(Node_t* node, Type_t* type){
    if(node->type == NULL){
        node->type = type;
    }
    else{
        Type_t* tmp = node->type;
        while(tmp->ptrof != NULL)
            tmp = tmp->ptrof;
        tmp->ptrof = type;
    }
}

void dump_node(Node_t* node, int num){
    if(node->lhs != NULL){
        dump_node(node->lhs, num+1);
    }

    if(node->op == AST_INT)
        printf("%d : %s(%d)\n", num, ast_name[node->op], node->val);
    else if(node->op == AST_ID)
        printf("%d : %s(%s)\n", num, ast_name[node->op], node->name);
    else if(node->op == AST_DEC)
        printf("%d : %s(%s)\n", num, ast_name[node->op], node->name);
    else if(node->op == AST_FUNC_DEC)
        printf("%d : %s(%s)\n", num, ast_name[node->op], node->name);
    else if(node->op == AST_STRING)
        printf("%d : %s(%s)\n", num, ast_name[node->op], node->name);
    else if(node->op == AST_CHAR)
        printf("%d : %s(%s)\n", num, ast_name[node->op], node->name);
    else
        printf("%d : %s\n", num, ast_name[node->op]);

    if(node->rhs != NULL){
        dump_node(node->rhs, num+1);
    }
}

int get_type_size(Type_t* type){
    int size;
    Type_t* st_dec;
    switch(type->ty){
        case TYPE_VOID: size = 1; break;
        case TYPE_CHAR: size = 1; break;
        case TYPE_INT: size = 4; break;
        case TYPE_PTR: size = 8; break;
        case TYPE_ARRAY: size = get_type_size(type->ptrof); break;
        case TYPE_STRUCT: 
                         if((st_dec = map_search(struct_dec, type->name)) == NULL){
                             fprintf(stderr, "struct %s was not declarerd.\n", type->name);
                             assert(0);
                         }
                         size = st_dec->offset;
                         break;
        default: size = 0; break;
    }
    return size;
}

int get_type_size_char(char* str){
    int size;
    char* tmp = str;
    char last = 0;
    while(*tmp != '\0'){
        last = *tmp;
        tmp++;
    }
    if(last == '*')
        size = 8;
    else if(!strcmp(str, "char"))
        size = 1;
    else if(!strcmp(str, "void"))
        size = 1;
    else if(!strcmp(str, "int"))
        size = 4;

    return size;
}

///////////////////////////////////
////////////expression/////////////
///////////////////////////////////

Node_t* primary_expr(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_INT){
        consume_token(TK_INT);
        node =  new_node_num(next->value, TYPE_INT);
    }
    else if(next->kind == TK_NULL){
        consume_token(TK_NULL);
        node =  new_node_num(0, TYPE_PTR);
    }
    else if(next->kind == TK_CHAR){
        consume_token(TK_CHAR);
        node = new_node_name(AST_CHAR, next->name);
        node->type = make_type();
        node->type->ty = TYPE_CHAR;
        node->val = next->value;
    }
    else if(next->kind == TK_ID){
        consume_token(TK_ID);
        node = new_node_ID(next->name);
    }
    else if(next->kind == TK_STRING){
        consume_token(TK_STRING);
        node = new_node_STRING(next->name);
    }
    else if(next->kind == '('){
        consume_token('(');
        node = expr();
        if(read_token(0)->kind != ')'){
            error(read_token(0));
            assert(0);
        }
        if(consume_token(')') < 0) assert(0);
    }
    else{
        error(next);
        assert(0);
    }
    return node;
}

Node_t* postfix_expr(){
    Node_t* node = primary_expr();
    Token_t* next = read_token(0);
    while(1){
        if(next->kind == '('){
            consume_token('(');
            next = read_token(0);
            if(next->kind != ')'){
                node = new_node(AST_FUNC_CALL, node, arg_expr_list());
            }
            else{
                node = new_node(AST_FUNC_CALL, node, NULL);
            }
            if(consume_token(')') < 0) assert(0);
        }
        else if(next->kind == '['){
            consume_token('[');
            node = conv2ptr(node);
            if(consume_token(']') < 0) assert(0);
        }
        else if(next->kind == '+'){
            next = read_token(1);
            if(next->kind == '+'){
                consume_token('+');
                consume_token('+');
                node = new_node(AST_POST_INC, node, NULL);
            }
            else break;
        }
        else if(next->kind == '-'){
            next = read_token(1);
            if(next->kind == '-'){
                consume_token('-');
                consume_token('-');
                node = new_node(AST_POST_DEC, node, NULL);
            }
            else break;
        }
        else if(next->kind == '.'){
            consume_token('.');
            node = new_node(AST_STRUCT_ID, node, new_node_ID(read_token(0)->name));
            if(consume_token(TK_ID) < 0) assert(0);
        }
        else if(next->kind == TK_ARROW){
            consume_token(TK_ARROW);
            //node = new_node(AST_STRUCT_ID_ARR, node, new_node_ID(read_token(0)->name));
            Node_t* st = new_node(AST_UNARY_PTR, node, NULL);
            node = new_node(AST_STRUCT_ID, st, new_node_ID(read_token(0)->name));
            if(consume_token(TK_ID) < 0) assert(0);
        }
        else break;
        next = read_token(0);
    }
    return node;
}

Node_t* arg_expr_list(){
    Node_t* node = assign_expr();
    Token_t* next = read_token(0);
    while(1){
        if(next->kind == ','){
            consume_token(',');
            node = new_node(AST_ARG_LIST, node, assign_expr());
        }
        else{
            node = new_node(AST_ARG_LIST, node, NULL);
            break;
        }
        next = read_token(0);
    }
    return node;
}

Node_t* unary_expr(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == '&'){
        consume_token('&');
        node = new_node(AST_UNARY_ADR, cast_expr(), NULL);
    }
    else if(next->kind == '*'){
        consume_token('*');
        node = new_node(AST_UNARY_PTR, cast_expr(), NULL);
    }
    else if(next->kind == '-'){
        consume_token('-');
        next = read_token(0);
        if(next->kind == '-'){
            consume_token('-');
            node = new_node(AST_PRE_DEC, cast_expr(), NULL);
        }
        else
            node = new_node(AST_UNARY_MINUS, cast_expr(), NULL);
    }
    else if(next->kind == '+'){
        consume_token('+');
        if(consume_token('+') < 0) assert(0);
        node = new_node(AST_PRE_INC, cast_expr(), NULL);
    }
    else if(next->kind == '!'){
        consume_token('!');
        node = new_node(AST_UNARY_REV, cast_expr(), NULL);
    }
    else if(next->kind == TK_SIZEOF){
        consume_token(TK_SIZEOF);
        next = read_token(0);
        if(next->kind == '('){
            consume_token('(');
            node = new_node(AST_SIZEOF, NULL, NULL);
            node->name = next->name;
            Type_t* root = type_specifier();
            next = read_token(0);
            while(next->kind == '*'){
                consume_token('*');
                Type_t* tmp = make_type();
                tmp->ty = TYPE_PTR;
                tmp->ptrof = root;
                root = tmp;
                next = read_token(0);
            }
            node->type = root;
            if(consume_token(')') < 0) assert(0);
        }
        else{
            node = new_node(AST_SIZEOF, unary_expr(), NULL);
        }
    }
    else
        node = postfix_expr();
    return node;
}

Node_t* cast_expr(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind != '('){
        node = unary_expr();
    }
    else{
        next = read_token(1);
        if(next->kind == TK_KW_INT || next->kind == TK_KW_CHAR || next->kind == TK_KW_VOID 
                || next->kind == TK_STRUCT || (next->kind == TK_ID && map_search(typedef_dec, next->name) != NULL)){
            consume_token('(');
            Type_t* root = type_specifier();
            next = read_token(0);
            while(next->kind == '*'){
                consume_token('*');
                Type_t* tmp = make_type();
                tmp->ty = TYPE_PTR;
                tmp->ptrof = root;
                root = tmp;
                next = read_token(0);
            }
            if(consume_token(')') < 0) assert(0);
            node = new_node(AST_CAST, cast_expr(), NULL);
            node->type = root;
        }
        else{
            node = unary_expr();
        }
    }
    return node;
}

Node_t* mul_expr(){
    Node_t* node = cast_expr();
    Token_t* next = read_token(0);
    if(read_token(1)->kind == '=') return node;
    while(next->kind == '*' || next->kind == '/' || next->kind == '%'){
        if(next->kind == '*'){
            consume_token('*');
            node = new_node(AST_MUL, node, cast_expr());

        }
        else if(next->kind == '/'){
            consume_token('/');
            node = new_node(AST_DIV, node, cast_expr());
        }
        else if(next->kind == '%'){
            consume_token('%');
            node = new_node(AST_REM, node, cast_expr());
        }
        next = read_token(0);
    }
    return node;
}

Node_t* add_expr(){
    Node_t *node = mul_expr();
    Token_t* next = read_token(0);
    if(read_token(1)->kind == '=') return node;
    while(next->kind == '+' || next->kind == '-'){
        if(next->kind == '+'){
            consume_token('+');
            node = new_node(AST_ADD, node, mul_expr());

        }
        else if(next->kind == '-'){
            consume_token('-');
            node = new_node(AST_SUB, node, mul_expr());
        }
        next = read_token(0);
    }
    return node;
}

Node_t* shift_expr(){
    Node_t *node = add_expr();
    Token_t* next = read_token(0);
    if(read_token(1)->kind == '=') return node;
    while(next->kind == TK_LSHIFT || next->kind == TK_RSHIFT){
        if(next->kind == TK_LSHIFT){
            consume_token(TK_LSHIFT);
            node = new_node(AST_LSHIFT, node, add_expr());

        }
        else if(next->kind == TK_RSHIFT){
            consume_token(TK_RSHIFT);
            node = new_node(AST_RSHIFT, node, add_expr());
        }
        next = read_token(0);
    }
    return node;
}

Node_t* relatinal_expr(){
    Node_t *node = shift_expr();
    Token_t* next = read_token(0);
    while(next->kind == TK_LEQ || next->kind == TK_SEQ || next->kind == '<' || next->kind == '>'){
        if(next->kind == TK_LEQ){
            consume_token(TK_LEQ);
            node = new_node(AST_LEQ, node, shift_expr());

        }
        else if(next->kind == TK_SEQ){
            consume_token(TK_SEQ);
            node = new_node(AST_SEQ, node, shift_expr());
        }
        else if(next->kind == '<'){
            consume_token('<');
            node = new_node(AST_LARGE, node, shift_expr());
        }
        else if(next->kind == '>'){
            consume_token('>');
            node = new_node(AST_SMALL, node, shift_expr());
        }
        next = read_token(0);
    }
    return node;
}

Node_t* equ_expr(){
    Node_t* node = relatinal_expr();
    Token_t* next = read_token(0);
    while(next->kind == TK_EQ || next->kind == TK_NEQ){
        if(next->kind == TK_EQ){
            consume_token(TK_EQ);
            node = new_node(AST_EQ, node, relatinal_expr());
        }
        else if(next->kind == TK_NEQ){
            consume_token(TK_NEQ);
            node = new_node(AST_NEQ, node, relatinal_expr());
        }
        next = read_token(0);
    }
    return node;
}

Node_t* and_expr(){
    Node_t* node = equ_expr();
    Token_t* next = read_token(0);
    if(read_token(1)->kind == '=') return node;
    while(next->kind == '&'){
        consume_token('&');
        node = new_node(AST_AND, node, equ_expr());
        next = read_token(0);
    }
    return node;
}

Node_t* exor_expr(){
    Node_t* node = and_expr();
    Token_t* next = read_token(0);
    if(read_token(1)->kind == '=') return node;
    while(next->kind == '^'){
        consume_token('^');
        node = new_node(AST_EXOR, node, and_expr());
        next = read_token(0);
    }
    return node;
}

Node_t* or_expr(){
    Node_t* node = exor_expr();
    Token_t* next = read_token(0);
    if(read_token(1)->kind == '=') return node;
    while(next->kind == '|'){
        consume_token('|');
        node = new_node(AST_OR, node, exor_expr());
        next = read_token(0);
    }
    return node;
}

Node_t* logical_and_expr(){
    Node_t* node = or_expr();
    Token_t* next = read_token(0);
    while(next->kind == TK_LOG_AND){
        consume_token(TK_LOG_AND);
        node = new_node(AST_LOG_AND, node, or_expr());
        next = read_token(0);
    }
    return node;
}

Node_t* logical_or_expr(){
    Node_t* node = logical_and_expr();
    Token_t* next = read_token(0);
    while(next->kind == TK_LOG_OR){
        consume_token(TK_LOG_OR);
        node = new_node(AST_LOG_OR, node, logical_and_expr());
        next = read_token(0);
    }
    return node;
}

Node_t* conditional_expr(){
    Node_t* node = logical_or_expr();
    Token_t* next = read_token(0);
    if(next->kind == '?'){
        consume_token('?');
        Node_t* lcond = expr();
        if(consume_token(':') < 0) assert(0);
        Node_t* rcond = conditional_expr();
        node = new_node(AST_COND, node, NULL);
        node->lcond = lcond;
        node->rcond = rcond;
    }
    return node;
}

Node_t* assign_expr(){
    Node_t* node = conditional_expr();
    Token_t* next1 = read_token(0);
    Token_t* next2 = read_token(1);
    if(next1->kind == '='){
        consume_token('=');
        node = new_node(AST_ASSIGN, node, assign_expr());
    }
    else if(next2->kind == '='){
        if(next1->kind == '*'){
            consume_token('*');
            consume_token('=');
            node = new_node_assign(AST_MUL, node, assign_expr());
        }
        else if(next1->kind == '/'){
            consume_token('/');
            consume_token('=');
            node = new_node_assign(AST_DIV, node, assign_expr());
        }
        else if(next1->kind == '%'){
            consume_token('%');
            consume_token('=');
            node = new_node_assign(AST_REM, node, assign_expr());
        }
        else if(next1->kind == '+'){
            consume_token('+');
            consume_token('=');
            node = new_node_assign(AST_ADD, node, assign_expr());
        }
        else if(next1->kind == '-'){
            consume_token('-');
            consume_token('=');
            node = new_node_assign(AST_SUB, node, assign_expr());
        }
        else if(next1->kind == TK_LSHIFT){
            consume_token(TK_LSHIFT);
            consume_token('=');
            node = new_node_assign(AST_LSHIFT, node, assign_expr());
        }
        else if(next1->kind == TK_RSHIFT){
            consume_token(TK_RSHIFT);
            consume_token('=');
            node = new_node_assign(AST_RSHIFT, node, assign_expr());
        }
        else if(next1->kind == '&'){
            consume_token('&');
            consume_token('=');
            node = new_node_assign(AST_AND, node, assign_expr());
        }
        else if(next1->kind == '^'){
            consume_token('^');
            consume_token('=');
            node = new_node_assign(AST_EXOR, node, assign_expr());
        }
        else if(next1->kind == '|'){
            consume_token('|');
            consume_token('=');
            node = new_node_assign(AST_OR, node, assign_expr());
        }
        //else{
        //    error(next1);
        //    assert(0);
        //}
    }
    return node;
}

Node_t* expr(){
    Node_t* node = assign_expr();
    Token_t* next = read_token(0);
    while(next->kind == ','){
        consume_token(',');
        node = new_node(AST_EXPR, node, assign_expr());
        next = read_token(0);
    }
    return node;
}

////////////////////////////////////
////////////declaration/////////////
////////////////////////////////////

Node_t* declaration(){
    Node_t* node;
    Type_t* type = declaration_specifier();
    if(read_token(0)->kind == ';'){
        node = new_node(AST_DECLN, NULL, NULL);
        node->type = type;
        consume_token(';');
    }
    else{
        node = init_declarator(type);
        consume_token(';');
    }
    return node;
}

Type_t* declaration_specifier(){
    Type_t* type;
    Token_t* next = read_token(0);
    if(next->kind == TK_TYPEDEF){
        consume_token(TK_TYPEDEF);
        Type_t* tmp = type_specifier();
        type = make_type();
        type->ty = TYPE_TYPEDEF;
        type->typeof = tmp;
        next = read_token(0);
        if(consume_token(TK_ID) < 0) assert(0);
        type->name = next->name;
        map_push(typedef_dec, type->name, type);
    }
    else if(next->kind == TK_EXTERN){
        consume_token(TK_EXTERN);
        type = type_specifier();
        //type->ext = 1;
        ext_flag = 1;
    }
    else{
        type = type_specifier();
    }
    return type;
}

Node_t* init_declarator(Type_t* type){
    Node_t* node = declarator(type);
    Token_t* next = read_token(0);
    if(next->kind == '='){
        consume_token('=');
        node = new_node(AST_INIT_DEC, node, assign_expr());
    }
    return node;
}

Type_t* type_specifier(){
    Type_t* type = make_type();
    Token_t* next = read_token(0);
    if(next->kind == TK_KW_INT){
        consume_token(TK_KW_INT);
        type->ty = TYPE_INT;
    }
    else if(next->kind == TK_KW_CHAR){
        consume_token(TK_KW_CHAR);
        type->ty = TYPE_CHAR;
    }
    else if(next->kind == TK_KW_VOID){
        consume_token(TK_KW_VOID);
        type->ty = TYPE_VOID;
    }
    else if(next->kind == TK_STRUCT){
        consume_token(TK_STRUCT);
        type->ty = TYPE_STRUCT;
        next = read_token(0);
        if(next->kind == '{'){
            fprintf(stderr, "not yet implemented at line %d\n", next->row);
            assert(0);
        }
        else if(next->kind == TK_ID){
            consume_token(TK_ID);
            type->name = next->name;
            next = read_token(0);
            if(next->kind == '{'){
                consume_token('{');
                next = read_token(0);
                int offset = 0;
                type->member = map_new();
                while(next->kind != '}'){
                    Node_t* st_dec = struct_declaration();
                    int is_array = (st_dec->type->ty == TYPE_ARRAY);
                    int array_size;
                    if(is_array) array_size = st_dec->type->array_size;
                    else array_size = 1;
                    int add_offset = get_type_size(st_dec->type);
                    if(add_offset == 8 && offset % 8 != 0){
                        offset += (8 - offset%8);
                    }
                    else if(add_offset == 4 && offset % 4 != 0){
                        offset += (4 - offset%4);
                    }
                    st_dec->type->offset = offset;
                    offset += array_size*add_offset;    
                    map_push(type->member, st_dec->name, st_dec->type);
                    next = read_token(0);
                }
                if(consume_token('}') < 0) assert(0);
                map_push(struct_dec, type->name, type);
                type->offset = offset;
            }
        }
        else{
            error(next);
            assert(0);
        }
    }
    else if(next->kind == TK_ENUM){
        consume_token(TK_ENUM);
        next = read_token(0);
        if(next->kind == '{'){
            consume_token('{');
            fprintf(stderr, "not yet implemented at line %d.\n", next->row);
            if(consume_token('}') < 0) assert(0);
        }
        else if(next->kind == TK_ID){
            consume_token(TK_ID);
            if(consume_token('{') < 0) assert(0);
            int offset = 0;
            while(1){
                next = read_token(0);
                if(consume_token(TK_ID) < 0) assert(0);
                char* id_name = next->name;
                next = read_token(0);
                if(next->kind == '='){
                    consume_token('=');
                    offset = read_token(0)->value;
                    if(consume_token(TK_INT) < 0) assert(0);
                    next = read_token(0);
                }
                if(map_search(enum_dec, id_name) != NULL){
                    fprintf(stderr, "Error : redeclaration of enumrator %s at line %d.\n", id_name, next->row);
                    assert(0);
                }
                int* val = (int*)malloc(sizeof(int));
                *val = offset;
                map_push(enum_dec, id_name, val);
                offset++;
                if(next->kind == '}'){
                    consume_token('}');
                    break;
                }
                else if(next->kind == ','){
                    consume_token(',');
                }
                else{
                    error(next);
                    assert(0);
                }
            }
        }
    }
    else if(next->kind == TK_ID){
        //typedef
        consume_token(TK_ID);
        Type_t* tmp;
        if((tmp = map_search(typedef_dec, next->name)) == NULL){
            fprintf(stderr, "Error : unknown type name '%s' at line %d.\n", next->name, next->row);
            assert(0);
        }
        type = tmp->typeof;
    }
    else{
        fprintf(stderr, "not yet implemented at line %d\n", next->row);
        assert(0);
    }
    return type;
}

Node_t* struct_declaration(){
    Type_t* type = type_specifier();
    Node_t* node = declarator(type);
    if(consume_token(';') < 0) assert(0);
    return node;
}

Node_t* declarator(Type_t* type){
    Token_t* next = read_token(0);
    Type_t* root = type;
    while(next->kind == '*'){
        consume_token('*');
        Type_t* tmp = make_type();
        tmp->ty = TYPE_PTR;
        tmp->ptrof = root;
        root = tmp;
        next = read_token(0);
    }
    Node_t* node = direct_declarator(&root);
    node->type = root;
    return node;
}

Node_t* direct_declarator(Type_t** root){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_ID){
        consume_token(TK_ID);
        node = new_node_DEC(next->name);
        next = read_token(0);
        if(next->kind == '('){
            consume_token('(');
            node->op = AST_FUNC_DEC;
            next = read_token(0);
            if(next->kind != ')') {
                node->args = get_paras();
                node->num_arg = (int)vector_size(node->args);
            }
            if(consume_token(')') < 0) assert(0);
        }
        else if(next->kind == '['){
            consume_token('[');
            int size = read_token(0)->value;
            Type_t* tmp = make_type();
            tmp->ty = TYPE_ARRAY;
            tmp->array_size = size;
            tmp->ptrof = *root;
            *root = tmp;
            if(consume_token(TK_INT) < 0) assert(0);
            if(consume_token(']') < 0) assert(0);
        }
    }
    else if(next->kind == '('){
        consume_token('(');
        fprintf(stderr, "not yet implemented at line %d\n",next->row);
        assert(0);
        //node = declarator();
        if(consume_token(')') < 0) assert(0);
    }
    else{
        error(next);
        assert(0);
    }
    return node;
}

Vector_t* get_paras(){
    Vector_t* paras = (Vector_t*)malloc(sizeof(Vector_t));
    paras = vector_new();
    Node_t* node = para_declaration();
    vector_push(paras, (Node_t*)node);
    Token_t* next = read_token(0);
    while(next->kind == ','){
        consume_token(',');
        node = para_declaration();
        vector_push(paras, (Node_t*)node);
        next = read_token(0);
    }
    return paras;
}

Node_t* para_declaration(){
    Type_t* type = type_specifier();
    Node_t* node = declarator(type);
    return node;
}

//////////////////////////////////
////////////statement/////////////
//////////////////////////////////

Node_t* stmt(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == '{'){
        node = compound_stmt();
    }
    else if(next->kind == TK_CASE || next->kind == TK_DEFAULT){
        node = labeled_stmt();
    }
    else if(next->kind == TK_WHILE || next->kind == TK_DO || next->kind == TK_FOR){
        node = iter_stmt();
    }
    else if(next->kind == TK_IF || next->kind == TK_SWITCH){
        node = sel_stmt();
    }
    else if(next->kind == TK_RETURN || next->kind == TK_CONT || next->kind == TK_BREAK){
        node = jump_stmt();
    }
    else{
        node = expr_stmt();
    }
    return node;
}

Node_t* labeled_stmt(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_CASE){
        consume_token(TK_CASE);
        Node_t* lhs = conditional_expr();
        if(consume_token(':') < 0) assert(0);
        node = new_node(AST_CASE, lhs, stmt());
        case_stmt[num_case++] = node;
        case_stmt = realloc(case_stmt, (1+num_case)*sizeof(Node_t*));
    }
    else if(next->kind == TK_DEFAULT){
        consume_token(TK_DEFAULT);
        if(consume_token(':') < 0) assert(0);
        node = new_node(AST_DEFAULT, NULL, stmt());
        default_stmt = node;
    }
    return node;
}

Node_t* compound_stmt(){
    Token_t* next = read_token(0);
    if(next->kind == '{'){
        consume_token('{');
        Node_t* node;
        next = read_token(0);
        if(next->kind == '}'){
            consume_token('}');
            node = new_node(AST_COMP_STMT, NULL, NULL);
            return node;
        }

        node = block_item();
        while(1){
            next = read_token(0);
            if(next->kind == '}') {
                node = new_node(AST_COMP_STMT, node, NULL);
                break;
            }
            else if(next->kind == TK_EOF){
                error(next);
                assert(0);
            }
            node = new_node(AST_BLOCK, node, block_item());
        }
        if(consume_token('}') < 0) assert(0);
        return node;
    }
    else{
        error(next);
        assert(0);
    }
    return NULL;
}

Node_t* block_item(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_KW_INT || next->kind == TK_KW_CHAR || next->kind == TK_KW_VOID 
            || next->kind == TK_STRUCT || (next->kind == TK_ID && map_search(typedef_dec, next->name) != NULL)){
        node = declaration();
    }
    else{
        node = stmt();
    }
    return node;
}

Node_t* expr_stmt(){
    Node_t* node = expr();
    Token_t* next = read_token(0);
    if(next->kind != ';'){
        error(next);
        assert(0);
    }
    if(consume_token(';') < 0) assert(0);
    return node;
}

Node_t* sel_stmt(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_IF){
        consume_token(TK_IF);
        if(consume_token('(') < 0) assert(0);
        Node_t* lhs = expr();
        if(consume_token(')') < 0) assert(0);
        Node_t* rhs = stmt();
        node = new_node(AST_IF, lhs, rhs);
        next = read_token(0);
        if(next->kind == TK_ELSE){
            consume_token(TK_ELSE);
            node->else_stmt = stmt();
        }
    }
    else if(next->kind == TK_SWITCH){
        consume_token(TK_SWITCH);
        if(consume_token('(') < 0) assert(0);
        Node_t* lhs = expr();
        if(consume_token(')') < 0) assert(0);
        case_stmt = (Node_t**)malloc(sizeof(Node_t*));
        num_case = 0;
        Node_t* rhs = stmt();
        node = new_node(AST_SWITCH, lhs, rhs);
        node->case_stmt = case_stmt;
        node->default_stmt = default_stmt;
        node->num_case = num_case;
    }
    else{
        error(next);
        assert(0);
    }
    return node;
}

Node_t* iter_stmt(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_WHILE){
        consume_token(TK_WHILE);
        if(consume_token('(') < 0) assert(0);
        Node_t* lhs = expr();
        if(consume_token(')') < 0) assert(0);
        Node_t* rhs = stmt();
        node = new_node(AST_WHILE, lhs, rhs);
    }
    else if(next->kind == TK_FOR){
        consume_token(TK_FOR);
        if(consume_token('(') < 0) assert(0);
        Node_t* tmp[3] = {NULL};
        for(int i = 0; i < 2; i++){
            next = read_token(0);
            if(next->kind == ';')
                consume_token(';');
            else{
                if(i == 0 && (next->kind == TK_KW_INT || next->kind == TK_KW_CHAR || next->kind == TK_KW_VOID 
                            || next->kind == TK_STRUCT || (next->kind == TK_ID && map_search(typedef_dec, next->name) != NULL)))
                        tmp[i] = declaration();
                else{
                    tmp[i] = expr();
                    if(consume_token(';') < 0) assert(0);
                }
            }
        }
        next = read_token(0);
        if(next->kind != ')')
            tmp[2] = expr();
        consume_token(')');
        node = new_node(AST_FOR, stmt(), NULL);
        node->lfor = tmp[0];
        node->mfor = tmp[1];
        node->rfor = tmp[2];
    }
    else if(next->kind == TK_DO){
        printf("not yet implemented at line %d\n", next->row);
        assert(0);
    }
    else{
        error(next);
        assert(0);
    }
    return node;
}

Node_t* jump_stmt(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_RETURN){
        consume_token(TK_RETURN);
        next = read_token(0);
        if(next->kind == ';')
            node = new_node(AST_RET, NULL, NULL);
        else
            node = new_node(AST_RET, expr(), NULL);
        if(consume_token(';') < 0) assert(0);
    }
    //onlt parse
    else if(next->kind == TK_CONT){
        consume_token(TK_CONT);
        if(consume_token(';') < 0) assert(0);
        node = new_node(AST_CONT, NULL, NULL);
    }
    else if(next->kind == TK_BREAK){
        consume_token(TK_BREAK);
        if(consume_token(';') < 0) assert(0);
        node = new_node(AST_BREAK, NULL, NULL);
    }
    return node;
}

//////////////////////////////////////
////////////ex-definition/////////////
//////////////////////////////////////

Node_t* translation_unit(){
    Node_t* node = function_definition();   
    node->global = 1;
    //if(node->type != NULL && node->type->ext) node->ext = 1;
    if(ext_flag) node->ext = 1;
    ext_flag = 0;
    return node;
}

Node_t* function_definition(){
    Node_t* node;
    Type_t* type = declaration_specifier();
    Token_t* next = read_token(0);
    if(next->kind == ';'){
        node = new_node(AST_DECLN, NULL, NULL);
        node->type = type;
        if(consume_token(';') < 0) assert(0);
        return node;
    }

    node = declarator(type);
    next = read_token(0);
    if(next->kind == '{'){
        Node_t* tmp = new_node(AST_FUNC, node, compound_stmt());
        tmp->name = node->name;
        tmp->type = node->type;
        tmp->args = node->args;
        tmp->num_arg = node->num_arg;
        node = tmp;

    }
    else if(next->kind == ';'){
        consume_token(';');
    }
    else if(next->kind == '='){
        consume_token('=');
        node->global = 1;
        next = read_token(0);
        node = new_node(AST_INIT_DEC, node, initializer());
        if(consume_token(';') < 0) assert(0);
    }
    else{
        error(next);
        assert(0);
    }

    return node;
}

Node_t* initializer(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == '{'){
        int first = 1;
        Node_t* node2;
        consume_token('{');
        next = read_token(0);
        while(1){
            Node_t* tmp;
            if(next->kind == '['){
                consume_token('[');
                next = read_token(0);
                Node_t* lhs; 
                if(next->kind == TK_INT){
                    consume_token(TK_INT);
                    lhs = new_node_num(next->value, TYPE_INT);
                }
                else if(next->kind == TK_CHAR){
                    consume_token(TK_CHAR);
                    lhs = new_node_name(AST_CHAR, next->name);
                    lhs->type = make_type();
                    lhs->type->ty = TYPE_CHAR;
                    lhs->val = next->value;
                }
                else{
                    error(next);
                    assert(0);
                }
                if(consume_token(']') < 0) assert(0);
                if(consume_token('=') < 0) assert(0);
                tmp = new_node(AST_DESIG, lhs, assign_expr());
            }
            else{
                tmp = assign_expr();
            }
            if(first){
                first = 0;
                node = new_node(AST_INIT_LIST, tmp, NULL);
                node2 = node;
            }
            else{
                Node_t* tmp2 = new_node(AST_INIT_LIST, tmp, NULL);
                node2->rhs = tmp2;
                node2 = tmp2;
            }
            //node = new_node(AST_INIT_LIST, tmp, NULL);
            next = read_token(0);
            if(next->kind != ',') break;
            consume_token(',');
            next = read_token(0);
        }
        if(consume_token('}') < 0) assert(0);
    }
    else{
        node = assign_expr();
    }
    return node;
}
