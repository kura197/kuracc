

int printf();
int scanf();

char* str = "hello\n";

int main(){
    char c;
    int out = 0;
    int idx = 0;
    while(1){
        //scanf("%c", &c);
        c = str[idx++];
        switch(c){
            case '\n': out = 1; break;
        }
        printf("%c", c);
        if(out) break;
    }
    return 0;
}
