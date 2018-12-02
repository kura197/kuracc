#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"
#include "vector_map.h"
#include "semantic.h"

struct SymTable;
struct Symbol;

enum type{
    TYPE_UNKNOWN,
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_LONG,
    TYPE_PTR,
    TYPE_STRUCT,
    TYPE_ARRAY
};

typedef struct Type{
    int ty;
    // for pointer
    struct Type *ptrof;
    size_t array_size;
    //for struct
    char* name;
}Type_t;

typedef struct Node{
    int op;
    struct Node *lhs;
    struct Node *rhs;

    //op == AST_INT
    int val; 

    //op == AST_ID
    char *name;

    //op == AST_FUNC
    int num_arg;

    Vector_t* args;

    //for
    struct Node* lfor;
    struct Node* mfor;
    struct Node* rfor;

    //if
    struct Node* else_stmt;

    // A ? B : C
    struct Node* lcond;
    struct Node* rcond;

    Type_t *type;

    struct SymTable* sym_table;
    struct Symbol* sym;

    //for +/-
    Type_t *ltype;
    Type_t *rtype;

    int global;

    struct Node** case_stmt;
    struct Node* default_stmt;
    int num_case;
}Node_t;

enum ast_kind{
    AST_INT,  
    AST_CHAR,  
    AST_ID,
    AST_STRING,
    AST_UNARY_ADR,  //&
    AST_UNARY_PTR,  //*
    AST_UNARY_MINUS,  //-
    AST_UNARY_REV,  //!
    AST_POST_INC,
    AST_POST_DEC,
    AST_PRE_INC,
    AST_PRE_DEC,
    AST_SIZEOF,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_REM,
    AST_LSHIFT,
    AST_RSHIFT,
    AST_LARGE, 
    AST_SMALL, 
    AST_LEQ, 
    AST_SEQ, 
    AST_EQ, 
    AST_NEQ,
    AST_AND,
    AST_EXOR,
    AST_OR,
    AST_LOG_OR,
    AST_LOG_AND,
    AST_ASSIGN,
    AST_COND,
    AST_EXPR,
    AST_FUNC_CALL,
    AST_ARG_LIST,
    AST_FUNC,
    AST_PARA_LIST,
    AST_COMP_STMT,
    AST_BLOCK,
    AST_DECLN,
    AST_DEC,
    AST_INIT_DEC,
    //AST_STRUCT,
    AST_FUNC_DEC,
    AST_WHILE,
    AST_IF,
    AST_ELSE,
    AST_SWITCH,
    AST_CASE,
    AST_DEFAULT,
    AST_DO,
    AST_FOR,
    AST_RET,
    AST_CONT,
    AST_BREAK
};


Node_t* new_node(int op, Node_t* lhs, Node_t* rhs);
Node_t* new_node_num(int val, int type);
Node_t* new_node_DEC(char* name);
Node_t* new_node_UOP(int* unary);
Node_t* new_node_STRING(char* name);
Node_t* new_node_name(int op, char* name);
Node_t* new_node_assign(int op, Node_t* lval, Node_t* rval);
Node_t* conv2ptr(Node_t* node);
void add_type(Node_t* node, Type_t* type);
void error(Token_t* tk);
void dump_node(Node_t* node, int num);
int get_type_size(Type_t* type);
int get_type_size_char(char* type_name);

Node_t* primary_expr();
Node_t* postfix_expr();
Node_t* arg_expr_list();
Node_t* unary_expr();
Node_t* cast_expr();
Node_t* mul_expr();
Node_t* add_expr();
Node_t* shift_expr();
Node_t* relatinal_expr();
Node_t* equ_expr();
Node_t* and_expr();
Node_t* exor_expr();
Node_t* or_expr();
Node_t* logical_and_expr();
Node_t* logical_or_expr();
Node_t* conditinal_expr();
Node_t* assign_expr();
Node_t* expr();

Node_t* declaration();
Node_t* init_declarator(Type_t* type);
Type_t* type_specifier();
Node_t* struct_declaration();
Node_t* declarator(Type_t* type);
Node_t* direct_declarator(Type_t** root);
Vector_t* get_paras();
Node_t* para_declaration();

Node_t* stmt();
Node_t* labeled_stmt();
Node_t* compound_stmt();
Node_t* block_item();
Node_t* expr_stmt();
Node_t* sel_stmt();
Node_t* iter_stmt();
Node_t* jump_stmt();

Node_t* translation_unit();
Node_t* function_definition();


extern char *ast_name[];
extern char *type_name[];
extern Map_t* struct_dec;

#endif
