
int print(int x);

int main(){
    int x = 2;
    int *y = &x;
    x = 5 + *y;
    char t = 1;
    print(x-t+4);
}
