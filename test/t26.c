
int printf();

void print(int d){
    printf("%d\n", d);
    return;
}

int main(){
    //int x = print(3); //gen error
    print(10);
    return 0;
}
