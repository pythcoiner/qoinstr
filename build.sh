#!/bin/bash

set -e

# create ./build if not existing
if ! [ -d "./build" ]; then
    mkdir build
fi

cd build
cmake ..
make -j 20
cp compile_commands.json ../compile_commands.json
