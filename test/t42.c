
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
struct test *t0;
struct test t2;

int judge(struct test *arg){
    if(arg->x != 2) return -1;
    if(arg->y != 4) return -1;
    if(arg->z != 6) return -1;
    if(strcmp(arg->name, str)) return -1;
    return 1;
}

int main(){
    struct test t1;
    struct test* t2;
    struct test t3;

    t0 = &t1;
    t0->x = 2;
    t0->y = 4;
    t0->z = 6;
    t0->name = str;
    if(judge(t0) < 0){
        printf("Error : %d\n", 0);
        return -1;
    }
    if(judge(&t1) < 0){
        printf("Error : %d\n", 1);
        return -1;
    }

    t2 = &t3;
    t2->x = 2;
    t2->y = 4;
    t2->z = 6;
    t2->name = str;
    if(judge(t2) < 0){
        printf("Error : %d\n", 2);
        return -1;
    }
    if(judge(&t3) < 0){
        printf("Error : %d\n", 3);
        return -1;
    }

    printf("OK\n");
    return 0;
}
