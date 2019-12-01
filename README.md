## NGEN -- Ninja build generator ##

I created this because my build.ninja files are often pretty simple, like this:

    build $builddir/dir/foo.o: cxx_compile dir/foo.cpp
    ...
    build $builddir/bin/app: cxx_link $builddir/dir/foo.o ...
    build $distdir/bin/app: install $builddir/bin/app

With the main differences having to do with toolchains (gcc/clang/msvc), common desires (separate .debug files), and other project related targets.

CMake just drives me to cray-cray to want to use it for most things, especially the non .cpp -> .o parts. Yet it's nice to have a build.ninja that isn't unix or visual studio only. And thus ngen was born!


### Build ###

./m.sh or ./m.cmd will bootstrap the application and place the binary in the same directory.

It will then run a quick test over examples to dispense some ninjas.

You need to have your compiler in path. Use m.sh for gcc and m.cmd for cl.exe.

### Usage ###

Hard coded 'cuz not important as getting this to work.

### ngen.json ###

WIP.

