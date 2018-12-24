
struct gchild{
    int a;
};

struct child{
    int x; 
    struct gchild* next;
};


struct parent{
    struct child* next[20];
};


int printf();

int main(){
    struct parent t0;
    struct child t1;
    struct gchild t2;

    t0.next[10] = &t1;
    t0.next[10]->next = &t2;
    t0.next[10]->next->a = 10;

    printf("OK\n");
    return 0;
}
