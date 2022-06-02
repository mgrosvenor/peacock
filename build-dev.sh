#! /usr/bin/env bash


mkdir -p bin

cd device
mkdir -p build
cd build
cmake ..
cmake --build .
cp peacock_dev.uf2 ../../bin/
cd ../../

