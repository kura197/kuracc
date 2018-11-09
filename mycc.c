#include <stdio.h>
#include <stdlib.h>

#define NUM_TK 1000

void read_add_sub();
void read_mul_div();

struct Token{
    int kind;
    int value;
};

enum token_kind{
    TK_INT = 0,  
    TK_PLUS = 1,  
    TK_MINUS = 2,
    TK_MUL = 3, 
    TK_DIV = 4,  
    TK_EOF = 5  
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

void read_add_sub(){
    read_mul_div(); 

    while(!end_tokens()){
        printf("  pushq %%rax\n");
        struct Token* tk = get_token();
        read_mul_div();
        printf("  pop %%rbx\n");
        if(tk->kind == TK_PLUS){
            printf("  addl %%ebx, %%eax\n");
        }
        else if(tk->kind == TK_MINUS){
            printf("  subl %%eax, %%ebx\n");
            printf("  movl %%ebx, %%eax\n");
        }
        else{
            printf("Error(read_add_sub). read token : %d.", tk->kind);
        }
    }
}

void read_mul_div(){
    struct Token* tk1 = get_token();
    if(tk1->kind != TK_INT){
        printf("Error(read_mul_div). read token : %d\n", tk1->kind);
    }
    printf("  movl $%d, %%eax\n", tk1->value);

    while(read_token()->kind == TK_MUL || read_token()->kind == TK_DIV){
        struct Token* tk2 = get_token();
        struct Token* tk3 = get_token();

        if(tk2->kind == TK_MUL){
            printf("  movl $%d, %%ebx\n", tk3->value);
            printf("  imul %%ebx, %%eax\n");
        }
        else if(tk2->kind == TK_DIV){
            printf("  movl $%d, %%ebx\n", tk3->value);
            printf("  cltd\n");
            printf("  div %%ebx\n");
        }
        else{
            printf("Error(read_mul_div). read token : %d.", tk2->kind);
        }
    }
}

int main(){
    tokenize();
    printf(".global main\n");
    printf("main:\n");

    read_add_sub();

    printf("  ret\n");

    return 0;
}
