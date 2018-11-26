#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

Node_t** case_stmt;
int num_case;
Node_t* default_stmt;

char *ast_name[] = {
    "AST_INT",
    "AST_CHAR",
    "AST_ID",
    "AST_STRING",
    "AST_UNARY_ADR",
    "AST_UNARY_PTR",
    "AST_UNARY_MINUS",
    "AST_UNARY_REV",
    "AST_ADD",
    "AST_SUB",
    "AST_MUL",
    "AST_DIV",
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
    "AST_EXPR",
    "AST_FUNC_CALL",
    "AST_ARG_LIST",
    "AST_FUNC",
    "AST_PARA_LIST",
    "AST_COMP_STMT",
    "AST_BLOCK",
    "AST_DEC",
    "AST_INIT_DEC",
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
    "AST_BREAK"
};

char *type_name[] = {
    "TYPE_UNKNOWN",
    "TYPE_VOID",
    "TYPE_CHAR",
    "TYPE_INT",
    "TYPE_LONG",
    "TYPE_PTR",
    "TYPE_ARRAY"
};


Node_t* new_node(int op, Node_t* lhs, Node_t* rhs){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node_t* new_node_num(int val, int type){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = AST_INT;
    node->val = val;
    node->type = (Type_t*)malloc(sizeof(Type_t));
    node->type->ty = type;
    return node;
}

Node_t* new_node_ID(char* ID){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = AST_ID;
    node->name = ID;
    return node;
}

Node_t* new_node_STRING(char* str){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = AST_STRING;
    node->name = str;
    return node;
}

Node_t* new_node_DEC(char* name){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = AST_DEC;
    node->name = name;
    return node;
}

Node_t* new_node_name(int op, char* name){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = op;
    node->name = name;
    return node;
}

Node_t* conv2ptr(Node_t* node){
    Node_t* array_at = expr();
    Node_t* add = new_node(AST_ADD, node, array_at);
    Node_t* unary_ptr = new_node(AST_UNARY_PTR, add, NULL);
    return unary_ptr;
}

void error(Token_t* tk){
    fprintf(stderr, "read token : %s\n", token_name[tk->kind]);
    assert(0);
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
    switch(type->ty){
        case TYPE_CHAR: size = 1; break;
        case TYPE_INT: size = 4; break;
        case TYPE_PTR: size = 8; break;
        case TYPE_ARRAY: size = get_type_size(type->ptrof); break;
        default: size = 0; break;
    }
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
    else if(next->kind == TK_CHAR){
        consume_token(TK_CHAR);
        node = new_node_name(AST_CHAR, next->name);
        node->type = (Type_t*)malloc(sizeof(Type_t));
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
        consume_token(')');
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
            consume_token(')');
        }
        else if(next->kind == '['){
            consume_token('[');
            node = conv2ptr(node);
            consume_token(']');
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
        node = new_node(AST_UNARY_MINUS, cast_expr(), NULL);
    }
    else if(next->kind == '!'){
        consume_token('!');
        node = new_node(AST_UNARY_REV, cast_expr(), NULL);
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
        Token_t* nnext = read_token(1);
        if(nnext->kind == TK_KW_INT){
            consume_token('(');
            printf("not yet implemented\n");
            assert(0);
            consume_token(')');
        }
        if(nnext->kind == TK_KW_CHAR){
            consume_token('(');
            printf("not yet implemented\n");
            assert(0);
            consume_token(')');
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
    while(next->kind == '*' || next->kind == '/'){
        if(next->kind == '*'){
            consume_token('*');
            node = new_node(AST_MUL, node, cast_expr());

        }
        else if(next->kind == '/'){
            consume_token('/');
            node = new_node(AST_DIV, node, cast_expr());
        }
        next = read_token(0);
    }
    return node;
}

Node_t* add_expr(){
    Node_t *node = mul_expr();
    Token_t* next = read_token(0);
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

Node_t* conditinal_expr(){
    Node_t* node = logical_or_expr();
    return node;
}

Node_t* assign_expr(){
    Node_t* node = logical_or_expr();
    Token_t* next = read_token(0);
    while(next->kind == '='){
        consume_token('=');
        if(read_token(0)->kind == TK_ID && read_token(1)->kind == '('){
            node = new_node(AST_ASSIGN, node, postfix_expr());
        }
        else{
            node = new_node(AST_ASSIGN, node, logical_or_expr());
        }
        next = read_token(0);
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
    Type_t* type = type_specifier();
    Node_t* node = init_declarator(type);
    consume_token(';');
    return node;
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
    Type_t* type = (Type_t*)malloc(sizeof(Type_t));
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
    else{
        printf("not yet implemented\n");
        assert(0);
    }
    return type;
}

Node_t* declarator(Type_t* type){
    Token_t* next = read_token(0);
    Type_t* root = type;
    while(next->kind == '*'){
        consume_token('*');
        Type_t* tmp = (Type_t*)malloc(sizeof(Type_t));
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
            consume_token(')');
        }
        else if(next->kind == '['){
            consume_token('[');
            int size = read_token(0)->value;
            Type_t* tmp = (Type_t*)malloc(sizeof(Type_t));
            tmp->ty = TYPE_ARRAY;
            tmp->array_size = size;
            tmp->ptrof = *root;
            *root = tmp;
            consume_token(TK_INT);
            consume_token(']');
        }
    }
    else if(next->kind == '('){
        consume_token('(');
        fprintf(stderr, "not yet implemented\n");
        assert(0);
        //node = declarator();
        consume_token(')');
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
        Node_t* lhs = conditinal_expr();
        consume_token(':');
        node = new_node(AST_CASE, lhs, stmt());
        case_stmt[num_case++] = node;
        case_stmt = realloc(case_stmt, (1+num_case)*sizeof(Node_t*));
    }
    else if(next->kind == TK_DEFAULT){
        consume_token(TK_DEFAULT);
        consume_token(':');
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
        if(read_token(0)->kind == '}'){
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
        consume_token('}');
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
    if(next->kind == TK_KW_INT || next->kind == TK_KW_CHAR){
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
    consume_token(';');
    return node;
}

Node_t* sel_stmt(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_IF){
        consume_token(TK_IF);
        consume_token('(');
        Node_t* lhs = expr();
        consume_token(')');
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
        consume_token('(');
        Node_t* lhs = expr();
        consume_token(')');
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
        consume_token('(');
        Node_t* lhs = expr();
        consume_token(')');
        Node_t* rhs = stmt();
        node = new_node(AST_WHILE, lhs, rhs);
    }
    else if(next->kind == TK_FOR){
        consume_token(TK_FOR);
        consume_token('(');
        Node_t* tmp[3] = {NULL};
        for(int i = 0; i < 2; i++){
            next = read_token(0);
            if(next->kind == ';')
                consume_token(';');
            else{
                tmp[i] = expr();
                consume_token(';');
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
        printf("not yet implemented\n");
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
        consume_token(';');
    }
    //onlt parse
    else if(next->kind == TK_CONT){
        consume_token(TK_CONT);
        consume_token(';');
        node = new_node(AST_CONT, NULL, NULL);
    }
    else if(next->kind == TK_BREAK){
        consume_token(TK_BREAK);
        consume_token(';');
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
    return node;
}

Node_t* function_definition(){
    Type_t* type = type_specifier();
    Node_t* node = declarator(type);

    Token_t* next = read_token(0);
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
        node = new_node(AST_INIT_DEC, node, assign_expr());
        consume_token(';');
    }
    else{
        error(next);
        assert(0);
    }

    return node;
}


