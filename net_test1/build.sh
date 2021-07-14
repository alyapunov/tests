#!/bin/bash

FLAGS="-std=c++11 -pthread -DNDEBUG -O2"
g++ $FLAGS test.cpp -o a.out
