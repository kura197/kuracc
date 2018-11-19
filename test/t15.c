
int x;
int y[20];
int *z;

int print(int a);

int main(){
    x = 3;
    y[0] = 10;
    y[5] = x;
    y[10] = x-1;
    *(y+15) = 20;
    z = y;
    print(y[5]+y[10] + *z + y[15]);
}
