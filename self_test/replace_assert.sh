#! /bin/sh

if [ "$1" != "" ]
then
    sed -e 's/assert/myassert/g' $1 > `basename "$1" .c`_new.c
fi
