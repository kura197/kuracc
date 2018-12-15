
int printf();

int test[20] = {
    2, 
    3, 
    4, 
    6, 
    [10] = 30,
    12, 
    100
};

char* test2[5] = {
    "Hello",
    "World",
    "Thanks"
};

int main(){
    int x = test[11];
    if(x != 12){
        printf("Error 0: %d\n", x);
        return -1;
    }
    printf("OK\n");
    return 0;
}
