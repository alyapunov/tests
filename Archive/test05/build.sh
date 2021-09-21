#!/bin/sh

g++ -o a1.out ./test.cpp
g++ -o a2.out -msse ./test.cpp
g++ -o a3.out -mno-sse ./test.cpp
g++ -O3 -o a4.out ./test.cpp
g++ -O3 -o a5.out -msse ./test.cpp
g++ -O3 -o a6.out -mno-sse ./test.cpp
