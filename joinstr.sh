#!/bin/bash

set -e

export CXX_QT_EXPORT_CRATE_qt_joinstr=1
export CXX_QT_EXPORT_DIR="./include"

JOINSTR_REPO="https://github.com/pythcoiner/qt_joinstr.git"
JOINSTR_BRANCH="master"

# create ./lib if not existing
if ! [ -d "./lib" ]; then
    mkdir lib
else
    # cleanup lib
    rm -fRd ./lib/*
fi

mkdir ./lib/include

# if qt_joinstr exists update
if [ -d "qt_joinstr" ]; then 
    cd qt_joinstr
    git fetch --all
    git pull origin "$JOINSTR_BRANCH" --force --rebase
    git checkout "$JOINSTR_BRANCH"
# if qt_joinstr not exists clone
else
    git clone "$JOINSTR_REPO"
    cd qt_joinstr
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
cp -r ./qt_joinstr/include/crates/qt_joinstr/include/qt_joinstr ./lib/include/qt_joinstr
cp -r ./qt_joinstr/include/crates/qt_joinstr/include/cxx-qt ./lib/include/cxx-qt
cp -r ./qt_joinstr/include/crates/qt_joinstr/include/cxx-qt-lib ./lib/include/cxx-qt-lib
cp -r ./qt_joinstr/include/crates/qt_joinstr/include/rust ./lib/include/rust

# modify some path in order to fix compilation error
sed -i 's|#include "cxx-qt/connection.h"|#include "../../cxx-qt/connection.h"|' ./lib/include/qt_joinstr/src/lib.cxx.h
sed -i 's|#include "qt_joinstr/src/lib.cxxqt.h"| |' ./lib/include/qt_joinstr/src/lib.cxx.h
sed -i 's|#include "qt_joinstr/src/lib.cxx.h"|#include "../lib.cxx.h"|' ./lib/include/qt_joinstr/src/lib.cxxqt.h
sed -i 's|#include "rust/cxx.h"|#include "../cxx-qt/connection.h"|' ./lib/include/cxx-qt/connection.h

# copy libraries into ./lib/
ls ./qt_joinstr/target/release
cp ./qt_joinstr/target/release/libqt_joinstr.a ./lib/libqt_joinstr.a
cp ./qt_joinstr/target/release/libqt_joinstr.rlib ./lib/libqt_joinstr.rlib
cp ./qt_joinstr/target/release/libqt_joinstr.so ./lib/libqt_joinstr.so
cp ./qt_joinstr/target/release/libqt_joinstr.d ./lib/libqt_joinstr.d
