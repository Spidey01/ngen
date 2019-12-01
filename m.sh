#!/bin/sh
set -e
set -v

git submodule init
git submodule update

mkdir -p dist build

cxx="g++"
ngen_cxx_std="c++14"
ngen_flags="-Wall -std=$ngen_cxx_std -Isrc -Ijson/single_include"

for source in src/*.cpp
do
    object=build/$(basename $source | sed -e 's/.cpp$/.o/')
    $cxx $ngen_flags -o $object -c $source
done

$cxx -o build/ngen build/*.o

cp -v build/ngen dist/ngen
cp -v dist/ngen ./

echo TESTING
cd examples/c_helloworld
../../ngen
