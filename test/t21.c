
int printf(int x);

int judge(int x){
    if(x == 1){
        return 0;
    }
    else if(x == 2){
        return 1;
    }
    else if(x == 3){
        return 2;
    }
    else{
        return -1;
    }
}

int main(){
    printf("%d\n", judge(2));
}
