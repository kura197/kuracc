#! /bin/sh

if [ "$1" != "" ]
then
    cat $1 | sed -e 's/assert/myassert/g' -e 's/kuracc.h/self_test\/header.h/g' -e 's/fprintf(stderr,/printf(/g'
fi
