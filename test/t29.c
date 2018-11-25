
int printf();

int a = 3;

int main(){
    a = 1;
    if(a != 1){
        printf("Fail : %d, a = %d\n", 1, a);
        return -1;
    }

    int a = 5;
    if(a != 5){
        printf("Fail : %d, a = %d\n", 2, a);
        return -1;
    }

    {
        if(a != 5){
            printf("Fail : %d, a = %d\n", 3, a);
            return -1;
        }

        int a = 10; 
        if(a != 10){
            printf("Fail : %d, a = %d\n", 4, a);
            return -1;
        }

    }
    //int a = 2; error

    if(a != 5){
        printf("Fail : %d, a = %d\n", 5, a);
        return -1;
    }

    printf("OK\n");
    return 0;
}
