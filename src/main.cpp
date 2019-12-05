/*
 * Copyright 2019-current Terry Mathew Poulin <BigBoss1964@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Bundle.hpp"
#include "Shinobi.hpp"

#include "msvc.hpp"
#include "gcc.hpp"
#include "javac.hpp"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

extern "C" {
#if defined(_WIN32)
#include <direct.h>
#ifndef chdir
#define chdir _chdir
#endif
#if defined(_MSC_VER)
/*
 * strerror() -> strerror_s() warning from /W4.
 */
#pragma warning(disable : 4996)
#endif
#else
#include <unistd.h>
#endif
}

using json = nlohmann::json;
using std::endl;
using std::string;
using std::to_string;

string defaultGenerator(const Bundle& bundle)
{
#if defined(_WIN32)
    string cxx_def = "msvc";
#else
    string cxx_def = "gcc";
#endif

    string gen;

    try {
        string type = bundle.data.at("projects").at(0).at("type");
        if (type.find("c_") == 0)
            gen = cxx_def;
        else if (type.find("cxx_") == 0)
            gen = cxx_def;
        else if (type.find("java_") == 0)
            gen = "javac";
    } catch (...) {
    }

    return gen;
}


json defaultDistribution()
{
    return json
    {
        /* Unix static libs, and Windows import libs. */
        { "archive", "$libdir" },
            /* Unix shared libs. */
            { "library", "$libdir" },
            /* executables, and Windows shared libs. */
            { "runtime", "$bindir" },

            /* GNU autoconf style. */
            { "prefix", "" },
            { "exec_prefix", "$prefix" },
            { "bindir", "$exec_prefix/bin" },
            { "sbindir", "$exec_prefix/sbin" },
            { "libexec", "$exec_prefix/libexec" },
            { "datarootdir", "$prefix/share" },
            { "datadir", "$prefix/share" },
            { "sysconfdir", "$prefix/etc" },
            { "sharedstatedir", "$prefix/com" },
            { "localstatedir", "$prefix/var" },
            { "runstatedir", "$localstatedir/run" },
            { "includedir", "$prefix/include" },
            { "docdir", "$datarootdir/doc/ngen" },
            { "infodir", "$datarootdir/info" },
            { "htmldir", "$docdir" },
            { "dvidir", "$docdir" },
            { "psdir", "$psdir" },
            { "libdir", "$exec_prefix/lib" },
            { "lispdir", "$datarootdir/emacs/site-lisp" },
            { "localedir", "$datarootdir/locale" },
    };
}

/*
 * Return next string in argv, or nullptr + print error.
 */
char* next(int& index, int argc, char**argv)
{
    index++;

    if (index >= argc) {
        std::clog << argv[0] << ": missing argument for " << argv[index - 1] << endl;
        return nullptr;
    }

    return argv[index];
}


void usage(const char* name)
{
    std::cout
        << "usage: " << name << " [options]" << endl
        << endl
        << "-h, --help this help." << endl
        << "-S DIR, --sourcedir DIR     Set sourcedir=DIR. Default is ." << endl
        << "-B DIR, --builddir DIR      Set builddir=DIR. Default is build" << endl
        << "-D DIR, --distdir DIR       Set distdir=DIR. Default is dist" << endl
        << "-f FILE, --file FILE        Set input to FILE. Default ngen.json" << endl
        << "-o FILE, --output FILE      Set output to FILE. Default build.ninja" << endl
        << "-C DIR, --directory DIR     Set directory to DIR before generating." << endl
        << "-v, --verbose               Turn on verbose mode" << endl
        << "-q, --quiet                 Turn off verbose mode" << endl
        << endl
        ;
}


int main(int argc, char* argv[])
{
    /*
     * Hard coding for now. Decide syntax later.
     */

    Bundle b;

    b.debug = true;

    b.sourcedir = ".";
    b.distdir = "dist";
    b.builddir = "build";
    b.data = {
        { "distribution", defaultDistribution() },
        { "projects", json::array({}) }
    };

    b.inputpath = "ngen.json";
    b.outputpath = "build.ninja";

    /* Like sysexits.h on BSD. */
    constexpr int Ex_Usage = 64;
    constexpr int Ex_DataErr = 65;
    constexpr int Ex_NoInput = 66;
    constexpr int Ex_CantCreate = 73;

    for (int i=0; i < argc; ++i) {
        string arg = argv[i];
        b.argv.push_back(arg);

        if (arg == "-h" || arg == "--help" || arg == "-help") {
            usage(argv[0]);
            return 0;
        }
        else if (arg == "-S" || arg == "--sourcedir") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            b.sourcedir = value;
        }
        else if (arg == "-B" || arg == "--builddir") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            b.builddir = value;
        }
        else if (arg == "-D" || arg == "--distdir") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            b.distdir = value;
        }
        else if (arg == "-f" || arg == "--file" || arg == "--ngen-file") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            b.inputpath = value;
        }
        else if (arg == "-o" || arg == "--output") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            b.outputpath = value;
        }
        else if (arg == "-G" || arg == "--generator") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            b.generatorname = value;
        }
        else if (arg == "-C" || arg == "--directory") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            b.directory = value;
        }
        else if (arg == "-v" || arg == "--verbose") {
            b.debug = true;
        }
        else if (arg == "-q" || arg == "--quiet") {
            b.debug = false;
        }
        else if (arg == "--") {
            break;
        }
    }

    if (!b.directory.empty()) {
        bool ok = chdir(b.directory.c_str()) == 0;
        if (!ok) {
            std::clog << b.argv[0] << ": failed to change directory to " << b.directory << std::strerror(errno) << endl;
        } else if (b.debug) {
            std::clog << "chdir " << b.directory << endl;
        }
    }


    if (b.inputpath == "-") {
        // XXX using cin would be nice
    } else {
        b.input.open(b.inputpath);
    }
    if (!b.input) {
        std::clog << b.argv[0] << ": cannot open input: " << b.inputpath << endl;
        return Ex_NoInput;
    }
    try {
        json temp;
        b.input >> temp;

        // for now assume one project object.
        b.data.at("projects").push_back(temp);
    } catch (std::exception& ex) {
        std::clog << b.argv[0] << ":error:" << b.inputpath << ": " << ex.what();
        return Ex_DataErr;
    }


    if (b.outputpath == "-") {
        // XXX using cout would be nice.
    } else {
        b.output.open(b.outputpath, b.output.out | b.output.trunc);
    }
    if (!b.output) {
        std::clog << b.argv[0] << ": cannot create " << b.outputpath << endl;
        return Ex_CantCreate;
    }

    b.generatorname = defaultGenerator(b);

    if (b.generatorname == "msvc") {
        b.generator = std::make_unique<msvc>(b);
    } else if (b.generatorname == "gcc") {
        b.generator = std::make_unique<gcc>(b);
    } else if (b.generatorname == "javac") {
        b.generator = std::make_unique<javac>(b);
    }

    if (b.debug) {
        std::clog << "DEBUG:" << endl;
        std::clog << "argv:" << endl;
        for (size_t i=0; i < b.argv.size(); ++i)
            std::clog << '\t' << "argv[" << i << "]: " << std::quoted(b.argv[i]) << endl;
        std::clog
            << "sourcedir: " << b.sourcedir << endl
            << "builddir: " << b.builddir << endl
            << "distdir: " << b.distdir << endl
            << "input: " << b.inputpath << endl
            << "output: " << b.outputpath << endl
            << "directory: " << b.directory << endl
            << "generator: " << b.generatorname << endl
            << "data: " << b.data.dump(4) << endl
            << endl;

        if (b.data.at("projects").empty()) {
            std::cout << b.argv[0] << ": nothing to do." << endl;
            return 0;
        }
    }

    try {
        if (!b.generator->generate()) {
            b.generator->failure(std::clog);
            std::clog << "What a Terrible Failure we has here." << endl;
        }
    } catch(std::exception& ex) {
        std::clog << b.argv[0] << ": " << b.generator->generatorName() << ": unhandled exception: " << ex.what() <<endl;
        return 1;
    }

    return 0;
}
