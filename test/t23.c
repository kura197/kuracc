
int printf(char* x);

int call(int d){
    //printf("called:%d\n", d);
    return 1;
}

int main(){
    int x = 7;
    int y = 3;
    int z = 2;
    if(x == 7 && call(1) == 1){
        if(y == 2 || call(2) != 1){
            printf("%d\n", 1);
        }
        else{
            printf("%d\n", 2);
        }
    }
    else{
        printf("%d\n", 0);
    }
}
