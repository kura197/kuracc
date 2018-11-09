#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "token.h"

void read_add_sub();
void read_mul_div();
void read_term();

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
            fprintf(stderr, "Error(read_add_sub). read token : %d.", tk->kind);
            assert(0);
        }
    }
}

void read_mul_div(){
    struct Token* tk1 = get_token();
    if(tk1->kind != TK_INT){
        fprintf(stderr, "Error(read_mul_div). read token : %d\n", tk1->kind);
        assert(0);
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
            fprintf(stderr, "Error(read_mul_div). read token : %d.", tk2->kind);
            assert(0);
        }
    }
}

void read_term(){
    struct Token* tk1 = get_token();
    //if(tk1->kind == )
    if(tk1->kind != TK_INT){
        fprintf(stderr, "Error(read_term). read token : %d.", tk1->kind);
        assert(0);
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
