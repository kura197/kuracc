
int printf();

struct st{
    int x;
    int y;
    int z;
    struct st* s;
    char s;
    char t;
    char u;
    char* name;
    int w;
};

int main(){
    char* str1 = "Hello, ";
    char* str2 = "World.";
    struct st test1;
    struct st test2;
    test2.w = 3;
    test1.y = 10;
    test2.z = test1.y + test2.w;
    test2.z++;
    test1.name = str1;
    test2.name = str2;
    printf("%s", test1.name);
    printf("%s", test2.name);
    printf(" %d\n", test2.z);
    return 0;
}
