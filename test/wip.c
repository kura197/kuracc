
int print(int x);

int main(){
    int a[2];
    *a = 1;
    *(a+1) = 2;
    int *p;
    p = a;
    int z;
    z = *p + *(p+1);
    print(z);
}
