joinstr:
    ./joinstr.sh

clean:
    rm -fRd ./build
    rm -fRd ./cpp_joinstr/target
    rm -fRd ./lib

clear:
    just clean
    rm -fRd ./cpp_joinstr
    rm -fRd ./.cache

make:
    just build

build:
    ./build.sh

run:
    ./build/Qoinstr

br:
    just build
    just run
