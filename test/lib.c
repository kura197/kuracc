#include<stdio.h>
#include<stdlib.h>

void print(int a){
    printf("%d\n", a);
}

void printc(char c){
    printf("%d\n", c);
}

void alloc4(int **p, int a, int b, int c, int d){
    int *x = (int*)malloc(4*sizeof(int));
    x[0] = a;
    x[1] = b;
    x[2] = c;
    x[3] = d;
    *p = x;
}
