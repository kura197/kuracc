#! /bin/sh

runtest_return() {
    ./mycc "$1" > tmp.s

    gcc -o tmp tmp.s -g
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
    ./$1 "$2" > tmp.s

    gcc -o tmp tmp.s ./test/lib.o -g
    ./tmp
    out=`./tmp`

    if [ "$out" != "$3" ]; then
        echo "$2: $3 expected, but got $out"
        exit 1
    fi
    rm tmp 
}

if [ "$1" = "wip" ] ;
then
    ./mycc "test/wip.c" > tmp.s
    gcc -o tmp tmp.s ./test/lib.o -g
    ./tmp
else
    gcc ./test/lib.c -c -o ./test/lib.o
    runtest_print "$1" test/t1.c 10
    runtest_print "$1" test/t2.c 3
    runtest_print "$1" test/t3.c 1024
    runtest_print "$1" test/t4.c 75
    runtest_print "$1" test/t5.c 3628800
    runtest_print "$1" test/t6.c 3
    runtest_print "$1" test/t7.c 10
    runtest_print "$1" test/t8.c 5
    runtest_print "$1" test/t9.c 8
    runtest_print "$1" test/t10.c 5
    runtest_print "$1" test/t11.c 1
    runtest_print "$1" test/t12.c 21
    runtest_print "$1" test/t13.c 51
    runtest_print "$1" test/t14.c 3
    runtest_print "$1" test/t15.c 35
    runtest_print "$1" test/t16.c 9
    runtest_print "$1" test/t17.c 10
    runtest_print "$1" test/t18.c 7
    runtest_print "$1" test/t19.c 10
    runtest_print "$1" test/t20.c "hello, world : 51"
    runtest_print "$1" test/t21.c 1
    runtest_print "$1" test/t22.c 2
    runtest_print "$1" test/t23.c 2
    runtest_print "$1" test/t24.c 2048
    runtest_print "$1" test/t25.c "Yes"
    runtest_print "$1" test/t26.c 10
    runtest_print "$1" test/t27.c "x"
    runtest_print "$1" test/t28.c "OK"
    runtest_print "$1" test/t29.c "OK"
    runtest_print "$1" test/t30.c 3
    runtest_print "$1" test/t31.c "x=3, y=2"
    runtest_print "$1" test/t32.c 7
    runtest_print "$1" test/t33.c "6, 11"
    runtest_print "$1" test/t34.c 14
    runtest_print "$1" test/t35.c "3, 3, 3"
    runtest_print "$1" test/t36.c "28, 1"
    runtest_print "$1" test/t37.c "OK"
    runtest_print "$1" test/t38.c "0 2 4 1 3 5 "
    runtest_print "$1" test/t39.c "Hello, World. 14"
    runtest_print "$1" test/t40.c "HELLOHELLO 7"
    runtest_print "$1" test/t41.c "OK"
    runtest_print "$1" test/t42.c "OK"
    runtest_print "$1" test/t43.c "OK"
    runtest_print "$1" test/t44.c 12
    runtest_print "$1" test/t45.c 10
    runtest_print "$1" test/t46.c "OK"
    runtest_print "$1" test/t47.c "97 a 12345"
    runtest_print "$1" test/t48.c "OK"
    runtest_print "$1" test/t49.c "OK"
    runtest_print "$1" test/t50.c "OK"
    echo 'Success!!'
fi


