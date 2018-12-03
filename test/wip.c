
int printf();

struct test{
    int x;
    int y;
    int z;
    int v;
    int w;
    char* name;
};

char* str = "Hello.";
struct test *t0;
struct test t2;

int main(){
    struct test t1;

    t0 = &t1;
    (*t0).x = 2;
    (*t0).y = 4;
    (*t0).z = 6;
    (*t0).name = str;
    printf("%d\n", t1.x);
    printf("%d\n", t1.y);
    printf("%d\n", t1.z);
    printf("%s\n", t1.name);
    int a = (*t0).x;
    int b = (*t0).y;
    int c = (*t0).z;
    char* d = (*t0).name;
    printf("%d\n", a);
    printf("%d\n", b);
    printf("%d\n", c);
    printf("%s\n", d);

    t2.x = 2;
    t2.y = 4;
    t2.z = 6;
    t2.name = str;
    printf("%d\n", t2.x);
    printf("%d\n", t2.y);
    printf("%d\n", t2.z);
    printf("%s\n", t2.name);
    int e = t2.x;
    int f = t2.y;
    int g = t2.z;
    char* h = t2.name;
    printf("%d\n", e);
    printf("%d\n", f);
    printf("%d\n", g);
    printf("%s\n", h);
    return 0;
}
