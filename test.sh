#! /bin/sh

runtest() {
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


runtest "3+2;" 5
runtest "3+4+1;" 8
runtest "80-30;" 50
runtest "8-5+3;" 6
runtest "6*4;" 24
runtest "4/2;" 2
runtest "6*3/2;" 9
runtest "5+2*4-8;" 5
runtest "2*(4+6);" 20
runtest "(4+6)*2;" 20
runtest "(3+6)/(1+2);" 3
runtest "(3+6)*((1+2+12)/(4+5-6));" 45
runtest "6==3;" 0
runtest "2==2;" 1
runtest "10!=10;" 0
runtest "10!=7;" 1
runtest "(3+6) == (3*3);" 1
runtest "5+4;  8-3;" 5
runtest "5+4*3;  8-3; 10*2-5;" 15
runtest "5+4*3;  8*3; 4*3 == 8+4*2/2;" 1
runtest "a = 4; a;" 4
runtest "a = 4; a = a + 1; a;" 5
runtest "a = 4; b = 5; c = a + b; c;" 9
runtest "foo = 5; hoge = 2; foo;" 5

echo 'Success!!'

