
int print();
int alloc4(int** p, int a, int b, int c, int d);

int main(){
    int *p;
    alloc4(&p, 1, 2, 4, 8);
    int *q;
    q = p + 2;
    print(*q);
}
