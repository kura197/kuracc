

int printf();

int main(){
    int a = sizeof(int) + sizeof(char) + sizeof(void) + sizeof(int*);
    printf("%d\n", a);
    return 0;
}
