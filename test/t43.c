
enum test{
    MON, TUE, WED, THU, FRI
};

enum test2{
    A, B = 2, C, D = 0, E
};

int printf();
int strcmp();

char* mon = "MONDAY";
char* tue = "TUESDAY";
char* wed = "WEDNES";
char* thu = "THURSDAY";
char* fri = "FRIDAY";

int main(){
    char* week[5];
    week[0] = mon;
    week[1] = tue;
    week[2] = wed;
    week[3] = thu;
    week[4] = fri;

    if(strcmp(week[TUE], tue)){
        printf("Error %d : %s", 0, week[TUE]);
        return -1;
    }

    if(strcmp(week[FRI], fri)){
        printf("Error %d : %s", 1, week[FRI]);
        return -1;
    }

    if(A != 0){
        printf("Error %d : %d", 2, A);
        return -1;
    }
    if(B != 2){
        printf("Error %d : %d", 3, B);
        return -1;
    }
    if(C != 3){
        printf("Error %d : %d", 4, C);
        return -1;
    }
    if(D != 0){
        printf("Error %d : %d", 5, D);
        return -1;
    }
    if(E != 1){
        printf("Error %d : %d", 6, E);
        return -1;
    }

    printf("OK\n");
    return 0;
}
