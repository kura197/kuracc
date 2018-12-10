
int printf();
int strcmp();

struct test{
    int x;
    int y;
    int z;
    int v;
    int w;
    char* name;
};

char* str = "Hello.";

int main(){
    struct test* t2;
    struct test t3;
    t2 = &t3;
    (*t2).z = 6;
    (*t2).name = str;
    printf("%d\n", (*t2).x);
    return 0;
}
