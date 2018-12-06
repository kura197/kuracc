
int printf();

typedef struct test{
    int x;
    int y;
}test_t;

typedef int myint;

int main(){
    test_t t0;
    struct test t1;
    myint a = 10;
    t0.x = 2;
    t0.y = a;
    t1.y = 0;
    printf("%d\n", t0.x + t0.y + t1.y);
    return 0;
}


