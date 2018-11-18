
int print(int x);

int main(){
    int *p;
    int x;
    x = 10;
    p = &x;
    *p = 5;
    print(*p);
}
