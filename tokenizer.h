#ifndef TOKEN_H_
#define TOKEN_H_

#define NUM_TK 1000

typedef struct Token{
    int kind;
    int value;
    char* name;
}Token_t;

enum token_kind{
    TK_INT = 0,  
    TK_EQ = 1,
    TK_NEQ = 2,
    TK_ID = 3,
    TK_EOF = 4,
};

void tokenize(char *p);
struct Token* read_token();
struct Token* get_token();
int end_tokens();
void dump_tokens();
void consume_token(char token);

extern Token_t tokens[NUM_TK];
extern int token_idx;
extern int num_tokens;
extern char *token_name[];

#endif
