#include <stdio.h>

int main(){
    int in;
    scanf("%d", &in);
    
    printf(".global main\n");
    printf("main:\n");
    printf("  mov $%d, %%eax\n", in);
    printf("  ret\n");
    return 0;
}
