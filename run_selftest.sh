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
    ./self_test/replace.sh "$1" > self
    ./mycc self > self.s
    gcc -c self.s -g

    gcc -o self -g self.o ./test/lib.o $OBJS
fi

