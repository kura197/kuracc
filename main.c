#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tokenizer.h"
#include "vector_map.h"
#include "parser.h"
#include "codegen.h"


int main(int argc, char* argv[]){
    if(argc == 0){
        fprintf(stderr, "usage : ./mycc code [test]");
        return -1;
    }
    if(argc == 3 && !strcmp(argv[2], "test")){
        test_vector(); 
        test_map(); 
        return 0;
    }
    tokenize(argv[1]);
    //dump_tokens();

    Vector_t *vec = vector_new();
    Token_t *next = read_token();
    while(next->kind != TK_EOF){
        Node_t *node = assign_expr();
        vector_push(vec, (Node_t*)node);
        //dump_node(node, 0);
        consume_token(';');
        next = read_token();
    }

    printf(".global main\n");
    printf("main:\n");
    printf("  pushq %%rbp\n");
    printf("  movq %%rsp, %%rbp\n");
    if(var != NULL)
        printf("  subq $%d, %%rsp\n", 8*(1+(int)map_size(var)));

    for(int i = 0; i < vector_size(vec); i++){
        codegen((Node_t*)vector_get(vec, i));
        printf("  pop %%rax\n");
    }
    printf("  movq %%rbp, %%rsp\n");
    printf("  pop %%rbp\n");
    printf("  ret\n");

    return 0;
}

