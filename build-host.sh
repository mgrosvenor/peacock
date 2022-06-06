#! /usr/bin/env bash

mkdir -p bin

cd host
mkdir -p build
cd build
cmake ..
cmake --build .
cp peacock_test ../../bin/
cd ../../



