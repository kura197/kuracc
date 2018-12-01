
int printf();

int main(){
    int x = 1;
    int y;

    y = x += 5;
    if(y != 6){
        printf("Error%d : %d\n", 1, y);
        return -1;
    }

    y -= 2;
    if(y != 4){
        printf("Error%d : %d\n", 2, y);
        return -1;
    }

    y *= 5;
    if(y != 20){
        printf("Error%d : %d\n", 3, y);
        return -1;
    }

    y /= 4;
    if(y != 5){
        printf("Error%d : %d\n", 4, y);
        return -1;
    }

    y <<= 2;
    if(y != 20){
        printf("Error%d : %d\n", 5, y);
        return -1;
    }

    y >>= 1;
    if(y != 10){
        printf("Error%d : %d\n", 6, y);
        return -1;
    }

    y |= 15;
    if(y != 15){
        printf("Error%d : %d\n", 7, y);
        return -1;
    }

    y &= 12;
    if(y != 12){
        printf("Error%d : %d\n", 8, y);
        return -1;
    }

    y ^= 15;
    if(y != 3){
        printf("Error%d : %d\n", 9, y);
        return -1;
    }

    printf("OK\n");

    return 0;
}
