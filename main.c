#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "token.h"

void read_add_sub();
void read_mul_div();
void read_term();
void read_eq_neq();

void read_add_sub(){
    read_mul_div(); 
    while(read_token()->kind == '+' || read_token()->kind == '-'){
        printf("  pushq %%rax\n");
        struct Token* tk = get_token();
        read_mul_div();
        printf("  pop %%rbx\n");
        if(tk->kind == '+'){
            printf("  addl %%ebx, %%eax\n");
        }
        else if(tk->kind == '-'){
            printf("  subl %%eax, %%ebx\n");
            printf("  movl %%ebx, %%eax\n");
        }
        else{
            fprintf(stderr, "Error(read_add_sub). read token : %s.\n", token_name[tk->kind]);
            assert(0);
        }
    }
}

void read_mul_div(){
    read_term();
    while(read_token()->kind == '*' || read_token()->kind == '/'){
        printf("  pushq %%rax\n");
        struct Token* tk2 = get_token();
        read_term();
        printf("  pushq %%rax\n");
        printf("  pop %%rbx\n");
        printf("  pop %%rax\n");

        if(tk2->kind == '*'){
            printf("  imul %%ebx, %%eax\n");
        }
        else if(tk2->kind == '/'){
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
    if(tk1->kind == '('){
        read_add_sub();
        struct Token* tk2 = get_token();
        if(tk2->kind != ')'){
            fprintf(stderr, "Error(read_term). read token : %s.\n", token_name[tk2->kind]);
            assert(0);
        }
    }
    else if(tk1->kind == TK_INT){
        printf("  movl $%d, %%eax\n", tk1->value);
    }
    else{
        fprintf(stderr, "Error(read_term). read token : %s.\n", token_name[tk1->kind]);
        assert(0);
    }
}

void read_eq_neq(){
    struct Token* tk = get_token();
    if(tk->kind != TK_EQ && tk->kind != TK_NEQ){
        fprintf(stderr, "Error(read_eq_neq). read token : %s.\n", token_name[tk->kind]);
        assert(0);
    }

    printf("  pushq %%rax\n");
    read_add_sub();
    printf("  pushq %%rax\n");

    printf("  popq %%rax\n");
    printf("  popq %%rbx\n");
    printf("  cmpl %%eax, %%ebx\n");
    if(tk->kind == TK_EQ)   
        printf("  sete %%al\n");
    if(tk->kind == TK_NEQ)   
        printf("  setne %%al\n");
    printf("  movzbl %%al, %%eax\n");
}

int main(){
    tokenize();
    //dump_tokens();
    printf(".global main\n");
    printf("main:\n");

    while(1){
        struct Token* tk = read_token();
        if(tk->kind == TK_EQ || tk->kind == TK_NEQ){
            read_eq_neq();
        }
        else if(tk->kind == TK_INT || tk->kind == '('){
            read_add_sub();
        }
        else
            break;
    }

    if(read_token()->kind != TK_EOF){
        assert(0);
    }
    printf("  ret\n");

    return 0;
}
