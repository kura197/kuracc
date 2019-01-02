# kuracc

A self-hosted C compiler I made to learn how compilers work.   
There are many unimplemented things.

## Usage
````
make
./kuracc filename.c > tmp.s
gcc -o tmp tmp.s
./tmp
````

## Run test
````
make test
````
