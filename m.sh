#!/bin/sh
set -e
set -v

git submodule init
git submodule update

bootstrapdir=bootstrap.gcc
mkdir -p $bootstrapdir

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
    object=$bootstrapdir/$(basename $source | sed -e 's/.cpp$/.o/')
    echo $cxx $ngen_flags -o $object -c $source
    $cxx $ngen_flags -o $object -c $source
done

echo $cxx -o $bootstrapdir/ngen $bootstrapdir/*.o  $ngen_libs
$cxx -o $bootstrapdir/ngen $bootstrapdir/*.o  $ngen_libs

$bootstrapdir/ngen
ninja
cp dist/ngen ./

