
#include <stdio.h>
#include "./test/t45.h"

int main(){
    int x = TEST;
    INTPTR y = &x;
    ST_TEST t0;
    t0.y = *y;
    printf("%d\n", t0.y);
    return 0;
}
