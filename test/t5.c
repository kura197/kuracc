
int print();

int factor(int x){
    int y;
    int z;
    y = x-1;
    if(x != 1){
        z = x*factor(y);
    }
    else{
        z = x;
    }
    return z;
}

int main(){
    int z;
    int a;
    a = 10;
    z = factor(a);
    print(z);
}
