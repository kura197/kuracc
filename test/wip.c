
typedef struct Symbol{
    int *name;
    int *type;
    int *ast;
    int name_space;
    int offset;
    int role;
}Symbol_t;

int printf();

int main(){
    Symbol_t s;
    int *a = s.name;
    a = s.type;
    a = s.ast;
    int x = s.name_space;
    x = s.offset;
    x = s.role;
    return 0;
}
