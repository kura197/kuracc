
int printf();

int main(){
    int i = 3;
    if(i != 3){
        printf("Error 0 : %d\n", i);
        return -1;
    }

    int a = 0;
    for(int i = 0; i < 10; i++){
        a++;
    }

    int b;
    for(b = 0; b < 10; b++){
        a++;
    }

    if(i != 3){
        printf("Error 1 : %d\n", i);
        return -1;
    }
    if(a != 20){
        printf("Error 2 : %d\n", a);
        return -1;
    }
    printf("OK\n");

    return 0;
}
