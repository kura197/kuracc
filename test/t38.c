
int printf();

int main(){
    int i = -1;
    while(++i < 10){
        if(i % 2) continue;
        if(i == 6) break;
        printf("%d ", i);
    }

    int x;
    for(x = 0; x < 10; x++){
        if(x % 2 == 0) continue;
        if(x == 7) break;
        printf("%d ", x);
    }
    printf("\n");
    return 0;
}
