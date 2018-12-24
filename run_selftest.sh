#! /bin/sh

# main : passed(vprintf error)
OBJS=" 
       main.o 
       parser.o 
       vector_map.o 
       tokenizer.o 
       semantic.o 
     "
       #codegen.o 

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > self
    ./mycc self > self.s
    gcc -c self.s -g

    gcc -o self -g self.o ./test/lib.o $OBJS
fi

