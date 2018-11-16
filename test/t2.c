
int print();

int main(){
    int a;
    int b;
    a = 0;
    b = 0;
    for(a = 0; a != 5; a=a+1){
        if(a == 1){
            b = b-1;
        }
        else{
            b = b+1;
        }
    }
    print(b);
}
