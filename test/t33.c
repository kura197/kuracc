
int printf();

int main(){
    int x = 5;
    int y = x++ + x++;
    x--;
    printf("%d, %d\n", x++, y++);
    return 0;
}
