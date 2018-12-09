

int printf();

int main(){
    int x;
    char y = 'a';
    x = (int)y;
    y = (char)x;
    int* p = (int*)12345;
    printf("%d %c %d\n", x, y, p);
    return 0;
}
