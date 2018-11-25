
int print(int x);

int main(){
    int x;
    x = 10;
    int *p = &x;
    //p = &x;
    *p = 5;
    print(*p);
}
