#! /bin/sh

OBJS=" 
       parser.o 
       semantic.o 
       tokenizer.o 
       codegen.o 
       main.o 
     "
       #vector_map.o 

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > tmp
    ./mycc tmp > tmp.s
    gcc -c tmp.s

    gcc -o tmp -g tmp.o ./test/lib.o $OBJS
fi

