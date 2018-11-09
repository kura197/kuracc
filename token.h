#ifndef TOKEN_H_
#define TOKEN_H_

#define NUM_TK 1000

struct Token{
    int kind;
    int value;
};

enum token_kind{
    TK_INT = 0,  
    TK_EQ = 1,
    TK_NEQ = 2,
    TK_EOF = 3  
};

void tokenize();
struct Token* read_token();
struct Token* get_token();
int end_tokens();
void dump_tokens();

extern struct Token tokens[NUM_TK];
extern int token_idx;
extern int num_tokens;
extern char *token_name[];

#endif
