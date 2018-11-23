
int printf();

int main(){
    int x = 3 << 1;
    int y = 1024 >> x;
    int z = y << (x+1);
    printf("%d\n", z);
    return 0;
}
