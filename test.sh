#! /bin/sh

runtest() {
    echo "$1" | ./mycc > tmp.s

    cc -o tmp tmp.s
    ./tmp

    out=$?
    if [ "$out" != "$2" ]; then
        echo "$1: $2 expected, but got $out"
        exit 1
    fi
    echo "$1 => $2"
    rm tmp tmp.s
}

runtest "3" 3
