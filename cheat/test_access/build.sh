#!/bin/sh
rm -rf CMakeCache.txt CMakeFiles
cmake .
make
rm -rf CMakeCache.txt CMakeFiles
chmod u+s ./a.out
