
int print();

int main(){
    int x;
    int *y;
    x = 5;
    y = &x;
    x = x + *y;
    print(x);
}
