
int print();

int main(){
    int x;
    int *y;
    int **z;
    x = 5;
    y = &x;
    z = &y;
    print(**z);
}
