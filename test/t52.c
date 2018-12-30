
int printf();

int main(){
    char c = 10;
    char *p;
    p = &c;

    if(*p == 10){
        printf("OK\n");
    }
    else{
        printf("NO\n");
    }
    return 0;
}
