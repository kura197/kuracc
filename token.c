#include "token.h"
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
    [')'] = ")"
};

void tokenize(){
    char input[32];
    int idx = 0;
    scanf("%[^\n]", input);

    int n = 0;
    char ch;
    while((ch = input[n++]) != '\0'){
        if('0' <= ch && ch <= '9'){
            char values[16];
            int num_values = 0;
            values[num_values++] = ch;
            while(1){
                ch = input[n];
                if('0' <= ch && ch <= '9')
                    values[num_values++] = ch;
                else 
                    break;
                n++;
            }
            values[num_values] = '\0';
            tokens[idx].value = strtol(values, NULL, 10);
            tokens[idx++].kind = TK_INT;
        }
        else if(ch == '+'){
            tokens[idx++].kind = '+';
        }
        else if(ch == '-'){
            tokens[idx++].kind = '-';
        }
        else if(ch == '*'){
            tokens[idx++].kind = '*';
        }
        else if(ch == '/'){
            tokens[idx++].kind = '/';
        }
        else if(ch == '('){
            tokens[idx++].kind = '(';
        }
        else if(ch == ')'){
            tokens[idx++].kind = ')';
        }
        else if(ch == '='){
            ch = input[n];
            if(ch == '='){
                tokens[idx++].kind = TK_EQ;
                n++;
            }
        }
        else if(ch == '!'){
            ch = input[n];
            if(ch == '='){
                tokens[idx++].kind = TK_NEQ;
                n++;
            }
        }
    }
    tokens[idx].kind = TK_EOF;
    num_tokens += idx;
}

Token_t* read_token(){
    Token_t* token;
    token = &tokens[token_idx];
    return token;
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

