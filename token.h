#ifndef TOKEN_H_
#define TOKEN_H_

#define NUM_TK 1000

struct Token{
    int kind;
    int value;
};

enum token_kind{
    TK_INT = 0,  
    TK_PLUS = 1,  
    TK_MINUS = 2,  
    TK_DIV = 3,  
    TK_MUL = 4,  
    TK_EOF = 5  
};

void tokenize();
struct Token* read_token();
struct Token* get_token();
int end_tokens();

struct Token tokens[NUM_TK];
int token_idx;
int num_tokens;

#endif
