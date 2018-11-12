#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token_t tokens[NUM_TK];
int token_idx;
int num_tokens;

//for debug purpose
char *token_name[] = {
    "TK_INT",
    "TK_EQ",
    "TK_NEQ",
    "TK_ID",
    "TK_EOF",
    ['+'] = "+",
    ['-'] = "-",
    ['*'] = "*",
    ['/'] = "/",
    ['('] = "(",
    [')'] = ")",
    [';'] = ";",
    [','] = ",",
    ['='] = "="
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
        else if(*p == ' '){
            ;
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
            else{
                tokens[idx++].kind = '=';
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
        else if(*p == ','){
            tokens[idx++].kind = ',';
        }
        else{
            tokens[idx].kind = TK_ID;
            int num = 0;
            while(1){
                p++;
                int out = 0;
                switch(*p){
                    case ' ':
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    case '(':
                    case ')':
                    case '=':
                    case '!':
                    case ';': out = 1; break;
                }
                if(out) break;
                else num++;
            }
            p--;
            tokens[idx].name = (char*)malloc((num+1)*sizeof(char));
            strncpy(tokens[idx].name, p-num, num+1);
            tokens[idx].name[num+1] = '\0';
            idx++;
        }
        p++;
    }
    tokens[idx].kind = TK_EOF;
    num_tokens += idx;
}

Token_t* read_token(int n){
    Token_t* token;
    token = &tokens[token_idx+n];
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
        else if(tk.kind == TK_ID){
            fprintf(stderr, "%d : %s (%s)\n", i, token_name[tk.kind], tk.name);
        }
        else{
            fprintf(stderr, "%d : %s \n", i, token_name[tk.kind]);
        }
    }
}

