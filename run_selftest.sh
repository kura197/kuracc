#! /bin/sh

# main : passed(vprintf error)
OBJS=" 
       main.o 
       parser.o 
       codegen.o 
       vector_map.o 
       semantic.o 
     "
       #tokenizer.o 

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > self
    ./mycc self > self.s
    #gcc -c self.s -g

    #gcc -o self -g self.o ./test/lib.o $OBJS
fi

