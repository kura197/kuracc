#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "kuracc.h"

Token_t tokens[NUM_TK];
int token_idx;
int num_tokens;
Map_t* define;
int idx;
int row;

//for debug purpose
char *token_name[256] = {
    "TK_INT",
    "TK_CHAR",
    "TK_KW_INT",
    "TK_KW_CHAR",
    "TK_KW_VOID",
    "TK_STRING",
    "TK_EQ",
    "TK_NEQ",
    "TK_ID",
    "TK_IF",
    "TK_ELSE",
    "TK_WHILE",
    "TK_FOR",
    "TK_DO",
    "TK_SWITCH",
    "TK_CASE",
    "TK_DEFAULT",
    "TK_RETURN",
    "TK_CONT",
    "TK_BREAK",
    "TK_LSHIFT",
    "TK_RSHIFT",
    "TK_LOG_AND",
    "TK_LOG_OR",
    "TK_LEQ",     
    "TK_SEQ",     
    "TK_SIZEOF",
    "TK_STRUCT",
    "TK_ENUM",
    "TK_ARROW",
    "TK_TYPEDEF",
    "TK_EXTERN",
    "TK_NULL",
    "TK_EOF",
    ['+'] = "+",
    ['-'] = "-",
    ['*'] = "*",
    ['/'] = "/",
    ['^'] = "^",
    ['>'] = ">",
    ['<'] = "<",
    ['|'] = "|",
    ['('] = "(",
    [')'] = ")",
    [';'] = ";",
    [','] = ",",
    ['{'] = "{",
    ['}'] = "}",
    ['&'] = "&",
    ['['] = "[",
    [']'] = "]",
    ['!'] = "!",
    [':'] = ":",
    ['?'] = "?",
    ['%'] = "%",
    ['.'] = ".",
    ['='] = "="
};

void tokenize(char* p){
    while(*p){
        if('0' <= *p && *p <= '9'){
            char values[32];
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
            tokens[idx].row = row;
            tokens[idx++].kind = TK_INT;
        }
        else if(*p == '/'){
            if(*(p+1) == '/'){
                while(*p != '\n'){
                    p++;
                }
                row++;
            }
            else if(*(p+1) == '*'){
                p++;
                while(!(*p == '*' && *(p+1) == '/')){
                    if(*p == '\n'){
                        row++;
                    }
                    p++;
                }
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '/';
            }
        }
        else if(*p == '"'){
            p++;
            int n = 0;
            char tmp[64];
            while(*p != '"'){
                if(*p == '\\' && *(p+1) == '\"'){
                    p += 2;
                    tmp[n++] = '\\';
                    tmp[n++] = '\"';
                }
                else{
                    tmp[n++] = *p;
                    p++;
                }
            }
            tmp[n] = '\0';
            tokens[idx].kind = TK_STRING;
            tokens[idx].name = malloc((n+1)*sizeof(char));
            strcpy(tokens[idx].name, tmp);
            tokens[idx].row = row;
            idx++;
        }
        else if(*p == ' '){
        }
        else if(*p == '\n'){
            row++;
        }
        else if(*p == '\t'){
        }
        else if(*p == '\''){
            p++;
            tokens[idx].kind = TK_CHAR;
            tokens[idx].row = row;
            tokens[idx].name = malloc(2*sizeof(char));
            tokens[idx].name[0] = *p;
            if(*p == '\\'){
                p++;
                tokens[idx].name[1] = *p;
                tokens[idx].value = get_special_char(*p);
            }
            else
                tokens[idx].value = *p;
            //printf("token:%d (%c)\n", tokens[idx].value, tokens[idx].value);
            p++;
            if(*p != '\''){
                fprintf(stderr, "invalid character-constant\n");
                assert(0);
            }
            tokens[idx].row = row;
            idx++;
        }
        else if(*p == '+'){
            tokens[idx].row = row;
            tokens[idx++].kind = '+';
        }
        else if(*p == '-'){
            if(*(p+1) == '>'){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_ARROW;
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '-';
            }
        }
        else if(*p == '*'){
            tokens[idx].row = row;
            tokens[idx++].kind = '*';
        }
        else if(*p == '<'){
            if(*(p+1) == '<'){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_LSHIFT;
                p++;
            }
            else if(*(p+1) == '='){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_LEQ;
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '<';
            }
        }
        else if(*p == '>'){
            if(*(p+1) == '>'){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_RSHIFT;
                p++;
            }
            else if(*(p+1) == '='){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_SEQ;
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '>';
            }
        }
        else if(*p == '^'){
            tokens[idx].row = row;
            tokens[idx++].kind = '^';
        }
        else if(*p == '|'){
            if(*(p+1) == '|'){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_LOG_OR;
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '|';
            }
        }
        else if(*p == '('){
            tokens[idx].row = row;
            tokens[idx++].kind = '(';
        }
        else if(*p == ')'){
            tokens[idx].row = row;
            tokens[idx++].kind = ')';
        }
        else if(*p == '='){
            char next = *(p+1);
            if(next == '='){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_EQ;
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '=';
            }
        }
        else if(*p == '!'){
            char next = *(p+1);
            if(next == '='){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_NEQ;
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '!';
            }
        }
        else if(*p == ';'){
            tokens[idx].row = row;
            tokens[idx++].kind = ';';
        }
        else if(*p == ','){
            tokens[idx].row = row;
            tokens[idx++].kind = ',';
        }
        else if(*p == '{'){
            tokens[idx].row = row;
            tokens[idx++].kind = '{';
        }
        else if(*p == '}'){
            tokens[idx].row = row;
            tokens[idx++].kind = '}';
        }
        else if(*p == '&'){
            if(*(p+1) == '&'){
                tokens[idx].row = row;
                tokens[idx++].kind = TK_LOG_AND;
                p++;
            }
            else{
                tokens[idx].row = row;
                tokens[idx++].kind = '&';
            }
        }
        else if(*p == '['){
            tokens[idx].row = row;
            tokens[idx++].kind = '[';
        }
        else if(*p == ']'){
            tokens[idx].row = row;
            tokens[idx++].kind = ']';
        }
        else if(*p == ':'){
            tokens[idx].row = row;
            tokens[idx++].kind = ':';
        }
        else if(*p == '?'){
            tokens[idx].row = row;
            tokens[idx++].kind = '?';
        }
        else if(*p == '%'){
            tokens[idx].row = row;
            tokens[idx++].kind = '%';
        }
        else if(*p == '.'){
            tokens[idx].row = row;
            tokens[idx++].kind = '.';
        }
        else if(*p == '#'){
            p++;
            char tmp[256];
            get_ident(tmp, &p);
            p++;
            if(!strcmp(tmp, "include")){
                while(*p != '<' && *p != '\"') p++;
                int libc = 1;
                if(*p == '\"') libc = 0;
                p++;
                int n = 0;
                while(*p != '\"' && *p != '>'){
                    tmp[n++] = *p;
                    p++;
                }
                tmp[n] = '\0';
                if(!libc){
                    char* header = map_file(tmp);
                    int tmp = row;
                    tokenize(header);
                    row = tmp;
                    //idx--;
                }
                p++;
            }
            else if(!strcmp(tmp, "define")){
                while(*p == ' ') p++;
                char* def_l = malloc(128*sizeof(char));
                int n = 0;
                while(*p != ' ' && *p != '\n'){
                    def_l[n++] = *p;
                    p++;
                }
                def_l[n] = '\0';

                while(*p == ' ') p++;
                char* def_r = malloc(128*sizeof(char));
                n = 0;
                while(*p != ' ' && *p != '\n'){
                    def_r[n++] = *p;
                    p++;
                }
                def_r[n] = '\0';
                map_push(define, def_l, def_r);
            }
            row++;
        }
        else{
            char tmp[256];
            int num = get_ident(tmp, &p);
            p--;
            char* def;
            tokens[idx].row = row;
            if(!strcmp(tmp, "if")){
                tokens[idx++].kind = TK_IF;
            }
            else if(!strcmp(tmp, "else")){
                tokens[idx++].kind = TK_ELSE;
            }
            else if(!strcmp(tmp, "while")){
                tokens[idx++].kind = TK_WHILE;
            }
            else if(!strcmp(tmp, "for")){
                tokens[idx++].kind = TK_FOR;
            }
            else if(!strcmp(tmp, "do")){
                tokens[idx++].kind = TK_DO;
            }
            else if(!strcmp(tmp, "switch")){
                tokens[idx++].kind = TK_SWITCH;
            }
            else if(!strcmp(tmp, "case")){
                tokens[idx++].kind = TK_CASE;
            }
            else if(!strcmp(tmp, "default")){
                tokens[idx++].kind = TK_DEFAULT;
            }
            else if(!strcmp(tmp, "int")){
                tokens[idx++].kind = TK_KW_INT;
            }
            else if(!strcmp(tmp, "char")){
                tokens[idx++].kind = TK_KW_CHAR;
            }
            else if(!strcmp(tmp, "void")){
                tokens[idx++].kind = TK_KW_VOID;
            }
            else if(!strcmp(tmp, "return")){
                tokens[idx++].kind = TK_RETURN;
            }
            else if(!strcmp(tmp, "continue")){
                tokens[idx++].kind = TK_CONT;
            }
            else if(!strcmp(tmp, "break")){
                tokens[idx++].kind = TK_BREAK;
            }
            else if(!strcmp(tmp, "sizeof")){
                tokens[idx++].kind = TK_SIZEOF;
            }
            else if(!strcmp(tmp, "struct")){
                tokens[idx++].kind = TK_STRUCT;
            }
            else if(!strcmp(tmp, "enum")){
                tokens[idx++].kind = TK_ENUM;
            }
            else if(!strcmp(tmp, "typedef")){
                tokens[idx++].kind = TK_TYPEDEF;
            }
            else if(!strcmp(tmp, "extern")){
                tokens[idx++].kind = TK_EXTERN;
            }
            else if(!strcmp(tmp, "NULL")){
                tokens[idx++].kind = TK_NULL;
            }
            else if((def = map_search(define, tmp)) != NULL){
                tokenize(def);
                //idx--;
            }
            else{
                tokens[idx].kind = TK_ID;
                tokens[idx].name = malloc((num+1)*sizeof(char));
                strcpy(tokens[idx].name, tmp);
                idx++;
            }
        }
        p++;
    }
    tokens[idx].kind = TK_EOF;
    num_tokens = idx;
}

int get_ident(char *id, char **p){
    int num = 0;
    int out = 0;
    while(1){
        switch(**p){
            case ' ':
            case '+':
            case '-':
            case '*':
            case '<':
            case '>':
            case '^':
            case '/':
            case '(':
            case ')':
            case '=':
            case '!':
            case ',':
            case '{':
            case '}':
            case '[':
            case ']':
            case ':':
            case '?':
            case '%':
            case '.':
            case '\n':
            case '\t':
            case '\0':
            case ';': out = 1; break;
        }
        if(out) break;
        id[num] = **p;
        (*p)++;
        num++;
    }
    //strncpy(id, *p-num, num);
    //(*p)--;
    id[num] = '\0';
    return num;
}

Token_t* read_token(int n){
    Token_t* token;
    token = &tokens[token_idx+n];
    return token;
}

int consume_token(char token){
    if(tokens[token_idx].kind == token){
        token_idx++;
        return 1;
    }
    else{
        //fprintf(stderr, "does not match consume_token at line %d.\ntoken : %d\tneed : %s\n" 
        //                       , tokens[token_idx].row, token, token_name[tokens[token_idx].kind]);
        fprintf(stderr, "does not match consume_token at line %d", tokens[token_idx].row);
        return -1;
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
        else if(tk.kind == TK_STRING){
            fprintf(stderr, "%d : %s (%s)\n", i, token_name[tk.kind], tk.name);
        }
        else if(tk.kind == TK_CHAR){
            fprintf(stderr, "%d : %s (%s)\n", i, token_name[tk.kind], tk.name);
        }
        else{
            fprintf(stderr, "%d : %s \n", i, token_name[tk.kind]);
        }
    }
}

char *map_file(char *filename){
    FILE* fp;
    if((fp = fopen(filename, "r")) == NULL){
        fprintf(stderr, "File open error at line %d.\n", row);
        assert(0);
    }

    char *ptr = malloc(8*65536 * sizeof(char));
    int idx = 0;
    char ch;
    while((ch = getc(fp)) != EOF){
        ptr[idx++] = ch;
    }
    ptr[idx] = '\0';
    fclose(fp);
    return ptr;
}

char get_special_char(char p){
    char ret = 0;
    switch(p){
        case '\'': ret = '\''; break;
        case '\"': ret = '\"'; break;
        case '?' : ret = '\?'; break;
        case '\\': ret = '\\'; break;
        case 'a' : ret = '\a'; break;
        case 'b' : ret = '\b'; break;
        case 'f' : ret = '\f'; break;
        case 'n' : ret = '\n'; break;
        case 'r' : ret = '\r'; break;
        case 't' : ret = '\t'; break;
        case 'v' : ret = '\v'; break;
        case '0' : ret = '\0'; break;
        default : 
                   fprintf(stderr, "invalid character-constant at line %d.\n", row);
                   assert(0);
                   break;
    }
    return ret;
}
