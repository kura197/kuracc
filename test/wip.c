
int printf();

int main(){
    int a = 2;
    int z = 2;
    int b;
    switch(a){
        case 1:
            b = 2;
            break;
        case 3:
        case 2:
            b = 4;
            break;
        default:
            b = 3;
            break;
    }

    printf("%d\n", b);
    return 0;
}
