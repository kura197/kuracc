
int print(int x);
int printc(int x);

int main(){
    char x[3];
    x[0] = -1;
    x[1] = 2;
    x[2] = 3;
    int y;
    y = 4*x[1]+x[2];
    printc(x[0] + y);
}
