#!/usr/bin/env bash

mkdir -p _build && cd _build
rm -rf *
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CONAN=Off -DENABLE_TEST=Off
make -j2
make package