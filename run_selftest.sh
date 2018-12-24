#! /bin/sh

# main : test passed(vprintf error if no input)
# codegen : test error(test2 error)
# tokenizer : compile error(mmap)
# semantic : test error(test1 error)
# vector_map : test error(test1 error)
# parser : compile error(semantic error)

OBJS=" 
       main.o 
       codegen.o 
       tokenizer.o 
       semantic.o 
       vector_map.o 
     "
       #parser.o 

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > self
    ./mycc self > self.s
    gcc -c self.s -g

    #gcc -o self -g self.o ./test/lib.o $OBJS
fi

