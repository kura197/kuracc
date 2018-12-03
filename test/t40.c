
int printf();

struct test{
    int x;
    int y;
    char* name;
    int z;
}t2;

int x;
struct test t0;
char* str = "HELLO";

int main(){
    struct test t1;
    t0.x = 3;
    t1.z = t0.x;
    t2.x = t1.z + t0.x + 1;
    t0.y = t2.x--;
    t2.name = str;
    t1.name = t2.name;
    printf("%s%s %d\n", t2.name, t1.name, t0.y);
    return 0;
}   
