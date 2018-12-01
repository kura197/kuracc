
int printf();

int z;

int main(){
    int x;
    int y;
    z = (x = (y = 3));
    printf("%d, %d, %d\n", x, y, z);
    return 0;
}
