#!/bin/bash

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

# if cpp_joinstr exists update
if [ -d "cpp_joinstr" ]; then 
    cd cpp_joinstr
    git fetch --all
    git pull origin "$JOINSTR_BRANCH" --force --rebase
    git checkout "$JOINSTR_BRANCH"
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
cargo build --release

cd ..
# copy bindings into ./lib/include/
cp -L ./cpp_joinstr/target/cxxbridge/cpp_joinstr/src/lib.rs.h ./lib/include/cpp_joinstr.h
cp -L ./cpp_joinstr/target/cxxbridge/rust/cxx.h ./lib/include/cxx.h

# copy libraries into ./lib/
cp ./cpp_joinstr/target/release/libcpp_joinstr.a ./lib/libcpp_joinstr.a
cp ./cpp_joinstr/target/release/libcpp_joinstr.rlib ./lib/libcpp_joinstr.rlib
cp ./cpp_joinstr/target/release/libcpp_joinstr.so ./lib/libcpp_joinstr.so
cp ./cpp_joinstr/target/release/libcpp_joinstr.d ./lib/libcpp_joinstr.d
