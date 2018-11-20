#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "tokenizer.h"
#include "vector_map.h"
#include "parser.h"
#include "codegen.h"
#include "semantic.h"

char *map_file(char *filename);

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

    tokenize(p);
    if(dump_tk) {
        printf("\ndump tokens\n");
        dump_tokens();
    }

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
    str_lit = map_new();
    for(int i = 0; i < (int)vector_size(vec); i++){
        sem_analy(vector_get(vec, i), -1);
    }

    for(int i = 0; i < map_size(str_lit); i++){
        Node_t* ast = vector_get(str_lit->val, i);
        char *name = vector_get(str_lit->key, i);
        if(ast->op == AST_UNARY_MINUS){
            ast = ast->lhs;
            ast->val *= -1;
        }

        if(ast->op == AST_STRING){
            printf("  .globl %s\n", name);
            printf("  .section .rodata\n");
            printf("%s:\n", ".LC0");
            printf(".string \"%s\"", ast->name);
            printf("  .data\n");
            printf("  .size %s, %d\n", name, 8);
            printf("%s:\n", name);
            printf("  .quad %s\n", ".LC0");
        }else{
            Symbol_t* sym = map_search(global, name);
            int size = get_type_size(sym->type);
            printf("  .globl %s\n", name);
            printf("  .data\n");
            printf("  .size %s, %d\n", name, size);
            printf("%s:\n", name);
            if(size == 4){
                printf("  .long %d\n", ast->val);
            }
            else if(size == 1){
                printf("  .byte %d\n", ast->val);
            }
        }
    }

    for(int i = 0; i < map_size(global); i++){
        Symbol_t* sym = vector_get(global->val, i);
        if(sym->role == FUNC) continue;
        if(map_search(str_lit, sym->name) != NULL) continue;
        int size = get_type_size(sym->type);
        if(sym->type->ty == TYPE_ARRAY)
            size *= sym->type->array_size;
        printf("  .comm  %s,%d\n", sym->name, size);
    }
    printf("  .text\n");
    printf("  .global main\n");

    for(int i = 0; i < vector_size(vec); i++){
        codegen((Node_t*)vector_get(vec, i));
    }
    return 0;
}

char *map_file(char *filename){
    struct stat sbuf;
    char* ptr;
    int fd;
    if((fd = open(filename, O_RDWR)) < 0){
        printf("File open error.\n");
        assert(0);
    }
    fstat(fd, &sbuf);
    ptr = mmap(NULL, sbuf.st_size+1, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    ptr[sbuf.st_size] = '\0';
    return ptr;
}
