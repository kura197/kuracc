
int printf();

int test[20] = {
    2, 
    3, 
    4, 
    6, 
    [4] = 30,
    12, 
    100
};

int main(){
    int x = test[4];
    printf("%d\n", x);
    return 0;
}
