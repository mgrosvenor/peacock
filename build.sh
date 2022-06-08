#! /usr/bin/env bash
shift 1
mkdir -p build
cd build
cmake $@ ..
cmake --build . --parallel --config Release
