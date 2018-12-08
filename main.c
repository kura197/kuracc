#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
//#include "tokenizer.h"
//#include "vector_map.h"
//#include "parser.h"
//#include "codegen.h"
//#include "semantic.h"
#include "mycc.h"


int main(int argc, char* argv[]){
    if(argc == 1){
        fprintf(stderr, "usage : ./mycc source [-t] [-a] [-v]\n");
        return -1;
    }

    //char* p = argv[1];
    char* p = map_file(argv[1]);

    int opt;
    int dump_tk = 0;
    int dump_ast = 0;
    int test_vec = 0;
    while((opt = getopt(argc, argv, "tav")) != -1){
        switch(opt){
            case 't':
                dump_tk = 1;
                break;
            case 'a':
                dump_ast = 1;
                break;
            case 'v':
                test_vec = 1;
                break;
            default:
                fprintf(stderr, "usage : ./mycc source [-t] [-a] [-v]\n");
                return -1;
                break;
        }
    }

    if(test_vec){
        test_vector();
        test_map();
        return 0;
    }

    define = map_new();
    tokenize(p);
    if(dump_tk) {
        printf("\ndump tokens\n");
        dump_tokens();
    }

    struct_dec = map_new();
    enum_dec = map_new();
    typedef_dec = map_new();
    Vector_t *vec = vector_new();
    Token_t *next = read_token(0);
    while(next->kind != TK_EOF){
        Node_t *node = translation_unit();
        vector_push(vec, (Node_t*)node);
        if(dump_ast){
            printf("\ndump AST\n");
            dump_node(node, 0);
        }
        next = read_token(0);
    }

    global = map_new();
    global_init = map_new();
    str_lit = vector_new();
    for(int i = 0; i < (int)vector_size(vec); i++){
        sem_analy(vector_get(vec, i));
    }

    strlabel = map_new();
    codegen_str();
    codegen_global_init();
    codegen_global();

    printf("  .text\n");
    //printf("  .global main\n");

    for(int i = 0; i < vector_size(vec); i++){
        codegen((Node_t*)vector_get(vec, i));
    }
    return 0;
}
