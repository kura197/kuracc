int printf();
int myassert();
int* malloc();
int* realloc();
int fprintf();
int sprintf();
int strcmp();
int free();
int getopt();

#define stderr 2

//vector_map
typedef struct Vector{
    void** item;     
    int len;
    int cap;
}Vector_t;

typedef struct Map{
    //char -> int
    Vector_t* key;
    Vector_t* val;
}Map_t;

Vector_t* vector_new();
void vector_push(Vector_t *vec, void *item);
void* vector_get(Vector_t *vec, int idx);
int vector_size(Vector_t *vec);
void vector_pop(Vector_t *vec);
void vector_delete(Vector_t *vec, int idx);

Map_t* map_new();
void map_push(Map_t* map, char* key, void* val);
void* map_search(Map_t* map, char* key);
int map_size(Map_t* map);
void map_pop(Map_t* map);


void test_vector();
void test_map();




//tokenizer

#define NUM_TK 200000

typedef struct Token{
    int kind;
    int value;
    char* name;
    int row;
}Token_t;

enum token_kind{
    TK_INT,  
    TK_CHAR,  
    TK_KW_INT,  
    TK_KW_CHAR,  
    TK_KW_VOID,  
    TK_STRING,  
    TK_EQ,
    TK_NEQ,
    TK_ID,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_DO,
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_RETURN,
    TK_CONT,
    TK_BREAK,
    TK_LSHIFT,
    TK_RSHIFT,
    TK_LOG_AND, //&&
    TK_LOG_OR,  //||
    TK_LEQ,     //<=
    TK_SEQ,     //>=
    TK_SIZEOF,
    TK_STRUCT,
    TK_ENUM,
    TK_ARROW,   //->
    TK_TYPEDEF,   
    TK_EXTERN,   
    TK_NULL,   
    TK_EOF
};

void tokenize(char *p);
int get_ident(char *id, char **p);
struct Token* read_token(int n);
struct Token* get_token();
int end_tokens();
void dump_tokens();
int consume_token(char token);
char *map_file(char *filename);

extern Token_t tokens[NUM_TK];
extern int token_idx;
extern int num_tokens;
extern char *token_name[256];
extern Map_t* define;



//parser

enum type{
    TYPE_UNKNOWN,
    TYPE_VOID,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_LONG,
    TYPE_PTR,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_TYPEDEF,
    TYPE_ARRAY
};

typedef struct Type{
    int ty;

    // for pointer
    struct Type *ptrof;
    int array_size;

    //for struct
    char* name;
    Map_t* member;
    int offset;

    //for typedef
    struct Type* typeof;
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
    int ext;

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
    AST_CAST,
    AST_POST_INC,
    AST_POST_DEC,
    AST_STRUCT_ID,
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
    AST_BREAK,
    AST_INIT_LIST,
    AST_DESIG
};


Node_t* make_node();
Type_t* make_type();
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
Type_t* declaration_specifier();
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
Node_t* initializer();


extern char *ast_name[128];
extern char *type_name[16];
extern Map_t* struct_dec;
extern Map_t* enum_dec;
extern Map_t* typedef_dec;



//semantic
#define MAX_BLOCK_DEPTH 256

enum NameSpace{
    NS_GLOBAL,
    NS_LOCAL,
    NS_ARG,
    NS_LABEL
};

enum role{VAR, FUNC};
typedef struct Symbol{
    char *name;
    struct Type *type;
    struct Node *ast;
    int name_space;
    int offset;
    int role;
}Symbol_t;

typedef struct SymTable{
    Map_t *global;
    Map_t *arg;
    Map_t *label;
    Map_t *string;
    int local_index;
    int num_var;
    int offset;
    Map_t *local[MAX_BLOCK_DEPTH];
}SymTable_t;


SymTable_t* sym_table_new();
Symbol_t* sym_new(char* name, struct Type* type, struct Node* ast, int name_space, int num_var, int role);
void sem_analy(struct Node* ast);
Type_t* struct_search_wrapper(Type_t* st, char* name);
Type_t* struct_search(Type_t* st, char* name);
Symbol_t* local_sym_search(SymTable_t* symt, char* name);
char* get_ptr_name(Type_t* type);

extern Map_t* global;
extern Map_t* global_init;
extern Vector_t* str_lit;



//codegen
void codegen(Node_t *node);
void codegen_lval(Node_t *node);
int codegen_arg(Node_t* node, int num);
int is_ptr(Type_t* type);
int allign8(int x);
void codegen_str();
void codegen_global_init();
void codegen_global();

extern Map_t* strlabel;




