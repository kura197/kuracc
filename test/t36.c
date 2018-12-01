
int printf();

int main(){
    int x;
    int y = 28;
    int z = 15;
    x = (y == z) ? 100 : (y > z) ? y : z;
    printf("%d, ", x);

    x = (y > z) ? (y > z+10) ? 1 : 2 : 0;
    printf("%d\n", x);
    return 0;
}
