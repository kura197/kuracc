
int printf(char* x);

int main(){
    int x = 7;
    int z = 2;
    if(((x | 3) & (z ^ 8)) == 2){
        printf("%d\n", 2);
    }
    else{
        printf("%d\n", 0);
    }
}
