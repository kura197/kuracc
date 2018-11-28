
int printf();

int main(){
    int a = 3;
    int b = -1;
    switch(a){
        case 1:
            b = 2;
            break;
        case 2:
        case 3:
        case 4:
            b = 3;
            break;
        default:
            b = 10;
            break;
    }

    printf("%d\n", b);
    return b;
}
