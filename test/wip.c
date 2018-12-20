

struct child{
    int x; 
};

struct parent{
    struct child next[10];
};


int printf();

int main(){
    struct parent t0;
    t0.next[0].x = 0;
    return 0;
}
