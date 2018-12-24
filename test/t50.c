

struct child{
    int x; 
};

struct parent{
    struct child next[10];
    struct child* pnext[20];
};


int printf();

int main(){
    struct parent t0;
    t0.next[2].x = 30;
    int z = t0.next[2].x;

    if(t0.next[2].x != 30 || z != 30){
        printf("Error 0 : ");
        printf("%d %d\n", t0.next[2].x, z);
        return 0;
    }

    t0.pnext[5] = &t0.next[2];
    t0.pnext[5]->x = 10;
    int y = t0.pnext[5]->x;

    if(t0.pnext[5]->x != 10 || y != 10){
        printf("Error 1 : ");
        printf("%d %d\n", t0.pnext[5]->x, y);
        return 0;
    }

    if(t0.next[2].x != 10 || z != 30){
        printf("Error 2 : ");
        printf("%d %d\n", t0.next[2].x, z);
        return 0;
    }

    int i = 5;
    t0.pnext[i]->x = 100;
    int t = t0.pnext[i]->x;
    if(t0.next[2].x != 100 || t != 100){
        printf("Error 3 : ");
        printf("%d %d\n", t0.next[5].x, t);
        return 0;
    }

    printf("OK\n");
    return 0;
}
