
int printf();
int malloc();

typedef struct test{
    char a;
    char b;
    char c;
    char d;
}test_t;

int main(){
    test_t* t0 = (test_t*)malloc(sizeof(test_t));
    t0->a = 20;
    t0->b = 40;
    t0->c = 60;
    t0->d = 80;
    int x = t0->b;
    int y = t0->d;
    //printf("%d %d %d %d\n", t0->a, t0->b, t0->c, t0->d);
    return 0;
}
