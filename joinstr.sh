#!/bin/bash

echo "OFFLINE=$OFFLINE"

set -e

JOINSTR_REPO="https://github.com/pythcoiner/cpp_joinstr.git"
JOINSTR_BRANCH="master"

# create ./lib if not existing
if ! [ -d "./lib" ]; then
    mkdir lib
else
    # cleanup lib
    rm -fRd ./lib/*
fi

mkdir ./lib/include

# if cpp_joinstr exists update or skip if offline
if [ -d "cpp_joinstr" ]; then 
    cd cpp_joinstr
    if [ -z "$OFFLINE" ]; then
        OFFLINE=false
    fi
    if [ "$OFFLINE" = false ]; then
        git fetch --all
        git pull origin "$JOINSTR_BRANCH" --force --rebase
        git checkout "$JOINSTR_BRANCH"
    else
        echo "Running in offline mode. Skipping fetch."
    fi
# if cpp_joinstr not exists clone
else
    git clone "$JOINSTR_REPO"
    cd cpp_joinstr
    git fetch --all
    git pull origin "$JOINSTR_BRANCH" --force --rebase
    git checkout "$JOINSTR_BRANCH"
fi
# cargo will:
#   - first generate c++ bindings
#   - then build library
if [ "$OFFLINE" = false ]; then
    cargo build --release
else
    cargo build --release --offline
fi

cd ..
# copy bindings into ./lib/include/
cp -L ./cpp_joinstr/target/cxxbridge/cpp_joinstr/src/lib.rs.h ./lib/include/cpp_joinstr.h
cp -L ./cpp_joinstr/target/cxxbridge/rust/cxx.h ./lib/include/cxx.h

# copy libraries into ./lib/
cp ./cpp_joinstr/target/release/libcpp_joinstr.a ./lib/libcpp_joinstr.a
cp ./cpp_joinstr/target/release/libcpp_joinstr.rlib ./lib/libcpp_joinstr.rlib
cp ./cpp_joinstr/target/release/libcpp_joinstr.so ./lib/libcpp_joinstr.so
cp ./cpp_joinstr/target/release/libcpp_joinstr.d ./lib/libcpp_joinstr.d
