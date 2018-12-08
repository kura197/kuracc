#! /bin/sh

OBJS=" 
       main.o 
       tokenizer.o 
       parser.o 
       semantic.o 
       codegen.o 
     "
       #"vector_map.o 

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > tmp
    ./mycc tmp > tmp.s
    gcc -c tmp.s

    gcc -o tmp -g tmp.o ./test/lib.o $OBJS
fi

