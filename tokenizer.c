#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>

Token_t tokens[NUM_TK];
int token_idx;
int num_tokens;

//for debug purpose
char *token_name[] = {
    "TK_INT",
    "TK_EQ",
    "TK_NEQ",
    "TK_EOF",
    ['+'] = "+",
    ['-'] = "-",
    ['*'] = "*",
    ['/'] = "/",
    ['('] = "(",
    [')'] = ")",
    [';'] = ";"
};

void tokenize(char* p){
    int idx = 0;
    while(*p){
        if('0' <= *p && *p <= '9'){
            char values[16];
            int num_values = 0;
            values[num_values++] = *p;
            while(1){
                char next = *(p+1);
                if('0' <= next && next <= '9')
                    values[num_values++] = next;
                else 
                    break;
                p++;
            }
            values[num_values] = '\0';
            tokens[idx].value = strtol(values, NULL, 10);
            tokens[idx++].kind = TK_INT;
        }
        else if(*p == '+'){
            tokens[idx++].kind = '+';
        }
        else if(*p == '-'){
            tokens[idx++].kind = '-';
        }
        else if(*p == '*'){
            tokens[idx++].kind = '*';
        }
        else if(*p == '/'){
            tokens[idx++].kind = '/';
        }
        else if(*p == '('){
            tokens[idx++].kind = '(';
        }
        else if(*p == ')'){
            tokens[idx++].kind = ')';
        }
        else if(*p == '='){
            char next = *(p+1);
            if(next == '='){
                tokens[idx++].kind = TK_EQ;
                p++;
            }
        }
        else if(*p == '!'){
            char next = *(p+1);
            if(next == '='){
                tokens[idx++].kind = TK_NEQ;
                p++;
            }
        }
        else if(*p == ';'){
            tokens[idx++].kind = ';';
        }
        p++;
    }
    tokens[idx].kind = TK_EOF;
    num_tokens += idx;
}

Token_t* read_token(){
    Token_t* token;
    token = &tokens[token_idx];
    return token;
}

void consume_token(char token){
    if(tokens[token_idx].kind == token)
        token_idx++;
    else{
        fprintf(stderr, "does not match consume_token\n");
    }
}

Token_t* get_token(){
    Token_t* token;
    token = &tokens[token_idx++];
    return token;
}

int end_tokens(){
    int rem = num_tokens - token_idx;
    if(rem == 0) return 1;
    else return 0;
}

void dump_tokens(){
    for(int i = 0; i < num_tokens; i++){
        Token_t tk = tokens[i];
        if(tk.kind == TK_INT){
            fprintf(stderr, "%d : %s (%d)\n", i, token_name[tk.kind], tk.value);
        }
        else{
            fprintf(stderr, "%d : %s \n", i, token_name[tk.kind]);
        }
    }
}

