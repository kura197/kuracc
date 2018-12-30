#! /bin/sh

if [ "$1" != "" ]
then
    #sed -e 's/assert/myassert/g' $1 > `basename "$1" .c`_new.c
    cat $1 | sed -e 's/assert/myassert/g' -e 's/mycc.h/self_test\/header.h/g' -e 's/fprintf(stderr,/printf(/g'
fi
