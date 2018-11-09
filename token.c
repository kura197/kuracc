#include "token.h"
#include <stdio.h>
#include <stdlib.h>

void tokenize(){
    char input[32];
    int idx = 0;
    scanf("%s", input);

    int n = 0;
    char values[16];
    int num_values = 0;
    char ch;
    while((ch = input[n++]) != '\0'){
        if('0' <= ch && ch <= '9'){
            values[num_values++] = ch;
        }else{
            if(num_values > 0){
                values[num_values] = '\0';
                tokens[idx].value = strtol(values, NULL, 10);
                tokens[idx++].kind = TK_INT;
                num_values = 0;
            }

            if(ch == '+'){
                tokens[idx++].kind = TK_PLUS;
            }
            else if(ch == '-'){
                tokens[idx++].kind = TK_MINUS;
            }
            else if(ch == '*'){
                tokens[idx++].kind = TK_MUL;
            }
            else if(ch == '/'){
                tokens[idx++].kind = TK_DIV;
            }
        } 
    }
    if(num_values > 0){
        values[num_values] = '\0';
        tokens[idx].value = strtol(values, NULL, 10);
        tokens[idx++].kind = TK_INT;
        num_values = 0;
    }
    tokens[idx].kind = TK_EOF;
    num_tokens += idx;
}

struct Token* read_token(){
    struct Token* token;
    token = &tokens[token_idx];
    return token;
}

struct Token* get_token(){
    struct Token* token;
    token = &tokens[token_idx++];
    return token;
}

int end_tokens(){
    int rem = num_tokens - token_idx;
    if(rem == 0) return 1;
    else return 0;
}

