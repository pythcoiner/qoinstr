#!/bin/bash

set -e

JOINSTR_REPO="https://github.com/pythcoiner/joinstr.git"
JOINSTR_BRANCH="interface"

if ! [ -d "./lib" ]; then
    mkdir lib
fi

if [ -f "./lib/joinstr.cpp" ]; then
    rm ./lib/joinstr.cpp
fi

if [ -f "./lib/joinstr.h" ]; then
    rm ./lib/joinstr.h
fi

if [ -f "./lib/libjoinstr.a" ]; then
    rm ./lib/libjoinstr.a
fi

if [ -d "joinstr" ]; then 
    cd joinstr
    git fetch --all
    git pull origin "$JOINSTR_BRANCH" --force --rebase
    git checkout "$JOINSTR_BRANCH"
else
    git clone "$JOINSTR_REPO"
    cd joinstr
    git fetch --all
    git pull origin "$JOINSTR_BRANCH" --force --rebase
    git checkout "$JOINSTR_BRANCH"
fi

cargo build --release
cp target/release/libjoinstr.a ../lib/libjoinstr.a
cp include/cpp/joinstr.cpp ../lib/joinstr.cpp
cp include/cpp/joinstr.h ../lib/joinstr.h
