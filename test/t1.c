
add(a,b){
    int c = a+b;
    c;
}

sub(a,b){
    int c = a-b;
    c;
}

mul(a,b){
    int c = a*b;
    c;
}

main(){
    int a;
    int b;
    int c;
    a = 10;
    b = 5;
    c = mul(add(a,b), sub(a,b));
    print(c);
}
