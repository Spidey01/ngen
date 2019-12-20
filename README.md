## NGEN -- Ninja build generator ##

I created this because my build.ninja files are often pretty simple, like this:

    build $builddir/dir/foo.o: cxx_compile dir/foo.cpp
    ...
    build $builddir/bin/app: cxx_link $builddir/dir/foo.o ...
    build $distdir/bin/app: install $builddir/bin/app

With the main differences having to do with toolchains (gcc/clang/msvc), common desires (separate .debug files), and other project related targets.

CMake just drives me to cray-cray to want to use it for most things, especially the non .cpp -> .o parts. Yet it's nice to have a build.ninja that isn't unix or visual studio only. And thus ngen was born!


### Build ###

./m.sh or ./m.cmd will bootstrap the application, and then use ngen to build itself. The final result will be dist/ngen[.exe].

You need to have your compiler in path. Use m.sh for gcc and m.cmd for cl.exe.


### Usage ###

    usage: dist\ngen.exe [options]

    -h, --help this help.
    -S DIR, --sourcedir DIR     Set sourcedir=DIR. Default is .
    -B DIR, --builddir DIR      Set builddir=DIR. Default is build
    -D DIR, --distdir DIR       Set distdir=DIR. Default is dist
    -f FILE, --file FILE        Set input to FILE. Default ngen.json
    -o FILE, --output FILE      Set output to FILE. Default build.ninja
    -C DIR, --directory DIR     Set directory to DIR before generating.
    -v, --verbose               Turn on verbose mode
    -q, --quiet                 Turn off verbose mode
    --version                   Display ngen version.

### Examples ###

  - c_helloworld
    + Simple c_application type.
  - cxx_library
    + A cxx_library.
  - cxx_application
    + Using cxx_library.
  - java_helloworld
    + Simple java_application type.
  - java_library
    + Make a .jar for a java_library type.
  - package
    + A project that packages children.
  - package/nested
    + A cxx_application to test nested package type.

#### Linux Examples ####

    $ ./m.sh
    $ ./dist/ngen -C examples
    $ ninja -C examples

#### Windows Examples ####

    > m.cmd
    > .\dist\ngen.exe -C examples
    > ninja -C examples

### ngen.json ###

WIP.

