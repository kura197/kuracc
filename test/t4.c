
int print();

int add(int a,int b){
    int c;
    c = a+b;
    c;
}

int sub(int a,int b){
    int c;
    c = a-b;
    c;
}

int mul(int a,int b){
    int c;
    c = a*b;
    c;
}

int main(){
    int a;
    int b;
    int c;
    a = 10;
    b = 5;
    c = mul(add(a,b), sub(a,b));
    print(c);
}
