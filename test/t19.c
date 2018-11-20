
int print(int x);
char s = 4;
int t = -1;
int u;

int main(){
    int x = 2;
    int *y = &x;
    x = 5 + *y;
    u = t;
    print(x+u+s);
}
