
int printf();
int strcmp();

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
    [3] = "World",
    "Thanks",
    "Bye"
};

int main(){
    int x = test[11];
    if(x != 12){
        printf("Error 0: %d\n", x);
        return -1;
    }

    char* s = test2[4];
    if(strcmp(s, "Thanks")){
        printf("Error 1: %s\n", s);
        return -1;
    }

    printf("OK\n");
    return 0;
}
