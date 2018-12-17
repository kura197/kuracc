#! /bin/sh

OBJS=" 
       tokenizer.o 
       main.o 
       semantic.o 
       parser.o 
       vector_map.o 
     "
       #codegen.o 

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > self
    ./mycc self > self.s
    #gcc -c self.s -g

    #gcc -o self -g self.o ./test/lib.o $OBJS
fi

