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
    ./mycc "$1" > tmp.s

    gcc -o tmp tmp.s ./test/lib.o -g
    ./tmp
    out=`./tmp`

    if [ "$out" != "$2" ]; then
        echo "$1: $2 expected, but got $out"
        exit 1
    fi
    #echo "$1 => $2"
    rm tmp 
}

runtest_print test/wip.c 2
runtest_print test/t1.c 10
runtest_print test/t2.c 3
runtest_print test/t3.c 1024
runtest_print test/t4.c 75
runtest_print test/t5.c 3628800
runtest_print test/t6.c 3
runtest_print test/t7.c 10
runtest_print test/t8.c 5
runtest_print test/t9.c 8
runtest_print test/t10.c 5
runtest_print test/t11.c 1
runtest_print test/t12.c 21
runtest_print test/t13.c 51
runtest_print test/t14.c 3
runtest_print test/t15.c 35
runtest_print test/t16.c 9
runtest_print test/t17.c 10
runtest_print test/t18.c 7
runtest_print test/t19.c 10
runtest_print test/t20.c "hello, world : 51"
runtest_print test/t21.c 1

echo 'Success!!'

