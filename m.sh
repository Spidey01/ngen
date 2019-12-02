#!/bin/sh
set -e
set -v

git submodule init
git submodule update

mkdir -p dist build

# set -e = die if no g++.
gcc_version=$(g++ -dumpversion)
cxx="g++"

if [ "$gcc_version" -ge 7 ]; then
    ngen_cxx_std="c++17"

    # cool: https://stackoverflow.com/a/33159746/352455
    if [ "$gcc_version" -lt 9 ]; then
        ngen_libs="-lstdc++fs"
    fi
elif [ "$gcc_version" -ge 5 ]; then
    ngen_cxx_std="c++14"
else
    echo "WARNING: gcc $gcc_version is unsupported."
    # if this fails the compiler is too old to care.
    ngen_cxx_std="c++1y"
fi

ngen_flags="-Wall -std=$ngen_cxx_std -Isrc -Ijson/single_include"

for source in src/*.cpp
do
    object=build/$(basename $source | sed -e 's/.cpp$/.o/')
    echo $cxx $ngen_flags -o $object -c $source
    $cxx $ngen_flags -o $object -c $source
done

echo $cxx -o build/ngen build/*.o  $ngen_libs
$cxx -o build/ngen build/*.o  $ngen_libs

cp -v build/ngen dist/ngen
cp -v dist/ngen ./

echo TESTING
cd examples/c_helloworld
../../ngen
