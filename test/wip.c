

struct test{
    char a;
    char b;
    int x; 
    int y; 
    char c;
    char* t;
    int z; 
    char* s;
    int w;
    int v;
    int u;
};


int printf();
char* str = "hello";

int main(){
    struct test t0;
    t0.a = 100;
    t0.b = 101;
    t0.x = 102;
    t0.y = 103;
    t0.z = 104;
    t0.s = str;
    t0.w = 105;
    t0.c = 106;
    t0.t = str;
    t0.v = 107;
    t0.u = 108;
    return 0;
}
