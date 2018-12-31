#! /bin/bash

OBJS=(main.o  
      vector_map.o  
      sem_analy.o  
      tokenizer.o  
      parser.o  
      codegen.o  
     )

SRCS=${OBJS[@]/.o/.c}

NEWOBJS=${OBJS[@]/.o/_self.o}

if [ "$1" != "" ]
then
    ./self_test/replace.sh "$1" > self.c
    ./kuracc self.c > self.s
    gcc -c self.s -g
    rm self.c

    TARGET=${1/.c/.o}
    OTHERS=${OBJS[@]#$TARGET}
    gcc -o self -g $OTHERS self.o ./test/lib.o 

else
    for item in ${SRCS[@]}; do
        ./self_test/replace.sh $item > self.c
        TMP=${item/.c/_self.s}
        ./kuracc self.c > $TMP
        rm self.c
        gcc -c $TMP -g 
    done
    gcc -o self -g ${NEWOBJS[@]} ./test/lib.o 

fi
