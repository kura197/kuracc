#include "parser.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

Map_t* var;
int num_var;

char *ast_name[] = {
    "AST_INT",
    "AST_EQ",
    "AST_NEQ",
    "AST_ID",
    "AST_POST_FIX",
    "AST_ARG_LIST",
    "AST_FUNC",
    "AST_COMP_STMT",
    "AST_DEC",
    "AST_EXPR",
    "AST_WHILE",
    "AST_IF",
    "AST_ELSE",
    "AST_DO",
    "AST_FOR",
    "AST_PARA_LIST",
    ['+'] = "+",
    ['-'] = "-",
    ['*'] = "*",
    ['/'] = "/",
    ['='] = "="
};


Node_t* new_node(int op, Node_t* lhs, Node_t* rhs){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node_t* new_node_num(int val){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = AST_INT;
    node->val = val;
    return node;
}

Node_t* new_node_ID(char* ID){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = AST_ID;
    node->name = ID;
    return node;
}

Node_t* new_node_DEC(char* name){
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    node->op = AST_DEC;
    node->name = name;
    return node;
}

void error(Token_t* tk){
    fprintf(stderr, "read token : %s\n", token_name[tk->kind]);
    assert(0);
}

Node_t* add_expr(){
    Node_t *node = mul_expr();

    Token_t* next = read_token(0);
    while(next->kind == '+' || next->kind == '-'){
        if(next->kind == '+'){
            consume_token('+');
            node = new_node('+', node, mul_expr());

        }
        else if(next->kind == '-'){
            consume_token('-');
            node = new_node('-', node, mul_expr());
        }
        next = read_token(0);
    }

    return node;
}

Node_t* mul_expr(){
    Node_t* node = postfix_expr();

    Token_t* next = read_token(0);
    while(next->kind == '*' || next->kind == '/'){
        if(next->kind == '*'){
            consume_token('*');
            node = new_node('*', node, postfix_expr());

        }
        else if(next->kind == '/'){
            consume_token('/');
            node = new_node('/', node, postfix_expr());
        }
        next = read_token(0);
    }
    return node;
}

Node_t* primary_expr(){
    Token_t* next = read_token(0);
    if(next->kind == TK_INT){
        consume_token(TK_INT);
        return new_node_num(next->value);
    }
    else if(next->kind == TK_ID){
        consume_token(TK_ID);
        //if(map_search(var, next->name) == NULL){
        //    fprintf(stderr, "%s was not declarated\n", next->name);
        //    assert(0);
        //}
        return new_node_ID(next->name);
    }
    else if(next->kind == '('){
        consume_token('(');
        Node_t* node = add_expr();
        if(read_token(0)->kind != ')'){
            error(read_token(0));
            assert(0);
        }
        consume_token(')');
        return node;
    }
    else{
        error(next);
        assert(0);
    }
    return NULL;
}

Node_t* equ_expr(){
    Node_t* node = add_expr();
    Token_t* next = read_token(0);
    if(next->kind == TK_EQ){
        consume_token(TK_EQ);
        node = new_node(AST_EQ, node, add_expr());
    }
    else if(next->kind == TK_NEQ){
        consume_token(TK_NEQ);
        node = new_node(AST_NEQ, node, add_expr());
    }

    return node;
}

Node_t* assign_expr(){
    Node_t* node = equ_expr();
    Token_t* next = read_token(0);
    if(next->kind == '='){
        consume_token('=');
        if(read_token(0)->kind == TK_ID && read_token(1)->kind == '('){
            node = new_node('=', node, postfix_expr());
        }
        else{
            node = new_node('=', node, equ_expr());
        }
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
                node = new_node(AST_POST_FIX, node, arg_expr_list());
            }
            else{
                node = new_node(AST_POST_FIX, node, NULL);
            }
            consume_token(')');
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

Node_t* translation_unit(){
    var = map_new();
    num_var = 0;
    Node_t* node = function_definition();   
    return node;
}

Node_t* function_definition(){
    num_var = 0;
    var = map_new();
    int type = type_specifier();
    Node_t* node = declarator();
    char* name = node->name;
    Para_t* args = node->args;
    int num_arg = node->num_arg;

    node = new_node(AST_FUNC, node, compound_stmt());
    node->num_var = num_var;
    node->var = var;
    node->name = name;
    node->type = type;
    node->args = args;
    node->num_arg = num_arg;

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
            node = new_node(AST_COMP_STMT, node, block_item());
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

Node_t* stmt(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == '{'){
        node = compound_stmt();
    }
    else if(next->kind == TK_WHILE || next->kind == TK_DO || next->kind == TK_FOR){
        node = iter_stmt();
    }
    else if(next->kind == TK_IF || next->kind == TK_SWITCH){
        node = sel_stmt();
    }
    else{
        node = expr_stmt();
    }
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
        printf("not yet implemented\n");
        assert(0);
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

Node_t* block_item(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_KW_INT){
        node = declaration();
    }
    else{
        node = stmt();
    }
    return node;
}

Node_t* declaration(){
    int type = type_specifier();
    Node_t* node = declarator();
    node->type = type;
    consume_token(';');
    return node;
}

Node_t* declarator(){
    Node_t* node;
    Token_t* next = read_token(0);
    if(next->kind == TK_ID){
        consume_token(TK_ID);
        if(read_token(0)->kind != '(' && map_search(var, next->name) == NULL){
            int* loc = (int*)malloc(sizeof(int));
            *loc = 8*(1+num_var++);
            map_push(var, next->name, loc);
        }
        node = new_node_DEC(next->name);
        next = read_token(0);
        if(next->kind == '('){
            consume_token('(');
            next = read_token(0);
            if(next->kind != ')') {
                node->args = get_paras();
                node->num_arg = (int)vector_size(node->args->decl);
            }
            consume_token(')');
        }
        return node;
    }
    //else if(next->kind == '('){
    //    consume_token('(');
    //    Node_t* node = declarator();
    //}
    else{
        error(next);
        assert(0);
    }
    return NULL;
}

int type_specifier(){
    int type = TYPE_UNKNOWN;
    Token_t* next = read_token(0);
    if(next->kind == TK_KW_INT){
        consume_token(TK_KW_INT);
        type = TYPE_INT;
    }
    else{
        printf("not yet implemented\n");
        assert(0);
    }
    return type;
}

Para_t* get_paras(){
    Para_t* paras = (Para_t*)malloc(sizeof(Para_t));
    paras->decl = vector_new();
    Node_t* node = para_declaration();
    vector_push(paras->decl, (Node_t*)node);
    Token_t* next = read_token(0);
    while(next->kind == ','){
        consume_token(',');
        node = para_declaration();
        vector_push(paras->decl, (Node_t*)node);
        next = read_token(0);
    }
    return paras;
}

Node_t* para_declaration(){
    int type = type_specifier();
    Node_t* node = declarator();
    node->type = type;
    return node;
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
    else
        printf("%d : %s\n", num, ast_name[node->op]);

    if(node->rhs != NULL){
        dump_node(node->rhs, num+1);
    }
}
