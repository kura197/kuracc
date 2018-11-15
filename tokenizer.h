#ifndef TOKEN_H_
#define TOKEN_H_

#define NUM_TK 1000

typedef struct Token{
    int kind;
    int value;
    char* name;
}Token_t;

enum token_kind{
    TK_INT,  
    TK_KW_INT,  
    TK_EQ,
    TK_NEQ,
    TK_ID,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_DO,
    TK_SWITCH,
    TK_EOF,
};

void tokenize(char *p);
struct Token* read_token(int n);
struct Token* get_token();
int end_tokens();
void dump_tokens();
void consume_token(char token);

extern Token_t tokens[NUM_TK];
extern int token_idx;
extern int num_tokens;
extern char *token_name[];

#endif
