#! /bin/sh

# main : test passed(vprintf error if no input)
# codegen : test error(test2 error)
# tokenizer : test error(test1 error)
# semantic : test error(test1 error)
# vector_map : test error(test1 error)
# parser : test error(test1 error)

OBJS=" 
       main.o 
       vector_map.o 
       semantic.o 
       tokenizer.o 
       parser.o 
     "
       #codegen.o 

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > self
    ./mycc self > self.s
    gcc -c self.s -g

    gcc -o self -g $OBJS self.o ./test/lib.o 
fi

