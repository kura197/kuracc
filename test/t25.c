
int printf();

int main(){
    int x = 2;
    if(x > 1){
        if(x < 2){
            printf("Hi\n");
        }
        else if(x <= 2){
            printf("Yes\n");
        }
        else{
            printf("No\n");
        }
    }
    else{
        printf("Bye\n");
    }
    return 0;
}
