
add(a,b){
    c = a+b;
    c;
}

sub(a,b){
    c = a-b;
    c;
}

mul(a,b){
    c = a*b;
    c;
}

main(){
    a = 10;
    b = 5;
    c = mul(add(a,b), sub(a,b));
    print(c);
}
