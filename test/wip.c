
int printf();

int main(){
    int x = 3;

    int i = 0;
    switch(i){
        case 0:
            if(x == 3) x = 1;
            break;
        //default:
        //    x = -1;
    }
    printf("%d\n", x);
    return 0;
}
