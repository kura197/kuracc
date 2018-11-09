#! /bin/sh

runtest() {
    echo "$1" | ./mycc > tmp.s

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

runtest "3+2" 5
runtest "3+4+1" 8
runtest "80-30" 50
runtest "8-5+3" 6
runtest "100-40+20-10" 70
runtest "6*4" 24
runtest "4/2" 2
runtest "6*4/2" 12
runtest "5+2*4-8" 5
