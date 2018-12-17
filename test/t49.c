
int printf();

struct test2{
    int z;
};

struct test{
    int x;
    int y;
    struct test2 tz;
    struct test2* tz_ptr;
};

int main(){
    struct test t0;
    struct test* t1;
    struct test2 t2;

    t0.tz.z = 10;
    if(t0.tz.z != 10){
        printf("Error 0 : %d\n", t0.tz.z);
        return -1;
    }

    t1 = &t0;
    t1->tz_ptr = &t2;
    t1->tz_ptr->z = 30;
    if(t1->tz_ptr->z != 30){
        printf("Error 1 : %d\n", t1->tz_ptr->z);
        return -1;
    }

    t1->tz.z = 50;
    if(t1->tz.z != 50){
        printf("Error 2 : %d\n", t1->tz.z);
        return -1;
    }

    printf("OK\n");
    return 0;
}
