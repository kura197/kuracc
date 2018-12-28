#! /bin/bash

# main : test passed(vprintf error if no input)
# codegen : test error(test40? error)
# tokenizer : compile error()
# semantic : test error(test1 error)
# vector_map : test error(test1 error)
# parser : compile error()

OBJS=(main.o  
      vector_map.o  
      semantic.o  
      tokenizer.o  
      parser.o  
      codegen.o  
     )

SRCS=${OBJS[@]/.o/.c}

NEWOBJS=${OBJS[@]/.o/_self.o}

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > self
    ./mycc self > self.s
    gcc -c self.s -g

    #TARGET=${1/.c/.o}
    #OTHERS=${OBJS[@]#$TARGET}
    #gcc -o self -g $OTHERS self.o ./test/lib.o 

else
    for item in ${SRCS[@]}; do
        #echo $item
        ./self_test/replace.sh $item > self.c
        ASM=${item/.c/_self.s}
        ./mycc self.c > $ASM
        rm self.c
        gcc -c $ASM -g 
    done
    gcc -o self -g ${NEWOBJS[@]} ./test/lib.o 

fi
