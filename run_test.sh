#! /bin/sh

runtest_return() {
    ./mycc "$1" > tmp.s

    gcc -o tmp tmp.s
    ./tmp

    out=$?
    if [ "$out" != "$2" ]; then
        echo "$1: $2 expected, but got $out"
        exit 1
    fi
    echo "$1 => $2"
    rm tmp 
}

runtest_print() {
    ./mycc "$1" > tmp.s

    gcc -o tmp tmp.s ./test/lib.o
    ./tmp
    out=`./tmp`

    if [ "$out" != "$2" ]; then
        echo "$1: $2 expected, but got $out"
        exit 1
    fi
    #echo "$1 => $2"
    rm tmp 
}

#runtest_print test/t1.c 75
#runtest_print test/t2.c 3
#runtest_print test/t3.c 1024
runtest_print test/t4.c 10

#runtest_return "main(){3+2;}" 5
#runtest_return "main(){3+4+1;}" 8
#runtest_return "main(){80-30;}" 50
#runtest_return "main(){8-5+3;}" 6
#runtest_return "main(){6*4;}" 24
#runtest_return "main(){4/2;}" 2
#runtest_return "main(){6*3/2;}" 9
#runtest_return "main(){5+2*4-8;}" 5
#runtest_return "main(){2*(4+6);}" 20
#runtest_return "main(){(4+6)*2;}" 20
#runtest_return "main(){(3+6)/(1+2);}" 3
#runtest_return "main(){(3+6)*((1+2+12)/(4+5-6));}" 45
#runtest_return "main(){6==3;}" 0
#runtest_return "main(){2==2;}" 1
#runtest_return "main(){10!=10;}" 0
#runtest_return "main(){10!=7;}" 1
#runtest_return "main(){(3+6) == (3*3);}" 1
#runtest_return "main(){5+4;  8-3;}" 5
#runtest_return "main(){5+4*3;  8-3; 10*2-5;}" 15
#runtest_return "main(){5+4*3;  8*3; 4*3 == 8+4*2/2;}" 1
#runtest_return "main(){a = 4; a;}" 4
#runtest_return "main(){a = 4; a = a + 1; a;}" 5
#runtest_return "main(){a = 4; b = 5; c = a + b; c;}" 9
#runtest_return "main(){foo = 5; hoge = 2; foo;}" 5
#
#runtest_print "main(){foo();}" "OK" "test.o"
#runtest_print "main(){a = 10; print(a);}" 10 "test.o"
#runtest_print "main(){a = 10; print(a/2*5);}" 25 "test.o"
#runtest_print "main(){add(5,8,20);}" 33 "test.o"
#runtest_print "main(){a = 5; b = 8; c = a*b; add(a,b,c);}" 53 "test.o"
#runtest_print "main(){a = 5; b = 8; c = a*b; add(a*b,b,c);}" 88 "test.o"
#
#runtest_return "foo(a){a;} main(){foo(5);}" 5
#runtest_return "add(c,d){c+d;} main(){add(2,3);}" 5
#runtest_return "add(a,b){a+b;} main(){a=5;b=10;add(a,b);}" 15
#runtest_return "add(a,b){a+b;} main(){a=5;b=10;add(3,2);}" 5
#runtest_return "add(a,b){a+b;} main(){a=5;b=10;c=50;d=100;add(add(a,b),add(c,d));}" 165
#runtest_return "main(){a=0; while(a != 3){a = a+1;} a;}" 3
#runtest_return "main(){a=1; if(a == 3){a = a+1;} if(a == 1){a = a*10;} a;}" 10
#runtest_return "main(){a=1; if(a == 3){a = a+1;}else{a = a*10;} a;}" 10
#runtest_return "main(){a=1; if(a == 3){a = a+1;}else if(a == 1){a = a*10;}else{a = 0;} a;}" 10
#runtest_return "main(){a=0; for(a=0; a!=3; a=a+1){1;} a;}" 3
#runtest_return "main(){a=0; b=1; for(a=0; a!=3; a=a+1){b = b*2;} b;}" 8
#runtest_return "main(){a=0; b=1; for(;a!=3; a=a+1){b = b*2;} b;}" 8

echo 'Success!!'

