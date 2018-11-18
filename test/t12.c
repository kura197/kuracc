
int print(int x);

int main(){
    int a[3];
    *a = 1;
    *(a+1) = 2;
    *(a+2) = 10;
    int *p;
    p = a;
    int z;
    z = *p + *(p+1) * *(p+2);
    print(z);
}
