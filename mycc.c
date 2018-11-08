#include <stdio.h>
#include <stdlib.h>

#define NUM_TK 1000

struct Token{
    int kind;
    int value;
};

enum token_kind{
    TK_EOF = 0,  
    TK_INT = 1,  
    TK_PLUS = 2,  
    TK_MINUS = 3  
};

struct Token tokens[NUM_TK];
int token_idx;
int num_tokens;

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

int main(){
    tokenize();
    printf(".global main\n");
    printf("main:\n");

    struct Token *tk = get_token();
    if(tk->kind != TK_INT){
        printf("Error. need INT token.\n");
    }
    printf("  movl $%d, %%eax\n", tk->value);

    while(!end_tokens()){
        struct Token *tk2 = get_token();
        struct Token *tk3 = get_token();

        if(tk3->kind != TK_INT){
            printf("Error. need INT token.\n");
            return -1;
        }

        if(tk2->kind == TK_PLUS){
            printf("  addl $%d, %%eax\n", tk3->value);
        }
        else if(tk2->kind == TK_MINUS){
            printf("  subl $%d, %%eax\n", tk3->value);
        }
        else{
            printf("Error. need PLUS/MINUS token.\n");
            return -1;
        }
    }
    printf("  ret\n");
    return 0;
}
