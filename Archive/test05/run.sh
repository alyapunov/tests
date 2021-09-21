#!/bin/sh

echo -------------
echo no opts
./a1.out
echo -------------
echo -msse
./a2.out
echo -------------
echo -mno-sse
./a3.out
echo -------------
echo -O3
./a4.out
echo -------------
echo -O3 -msse
./a5.out
echo -------------
echo -O3 -mno-sse
./a6.out
echo -------------
