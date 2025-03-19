joinstr:
    ./joinstr.sh

clean:
    rm -fRd ./build
    rm -fRd ./qt_joinstr/target
    rm -fRd ./lib

clear:
    just clean
    rm -fRd ./qt_joinstr
    rm -fRd ./.cache

make:
    just build

build:
    ./build.sh
