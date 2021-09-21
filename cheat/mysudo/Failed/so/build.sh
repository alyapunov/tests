#!/bin/sh
g++ -g -ggdb -shared -fPIC -fpic -Wl,-init,init -o test.so test.so.cpp
g++ -g -ggdb test.cpp -ldl -rdynamic