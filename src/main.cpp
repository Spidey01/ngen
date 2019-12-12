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
#include "path.hpp"
#include "util.hpp"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

#if defined(_MSC_VER)
/*
 * strerror() -> strerror_s() warning from /W4.
 */
#pragma warning(disable : 4996)
#endif

using json = nlohmann::json;
using std::endl;
using std::string;
using std::to_string;

extern string defaultCxxGenerator;
static char* next(int& index, int argc, char**argv);
static void usage(const char* name);
static int options(int argc, char**argv, Bundle& bundle);


/*
 * Return next string in argv, or nullptr + print error.
 */
static char* next(int& index, int argc, char**argv)
{
    index++;

    if (index >= argc) {
        std::clog << argv[0] << ": missing argument for " << argv[index - 1] << endl;
        return nullptr;
    }

    return argv[index];
}


static void usage(const char* name)
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
        << "--default-cxx-generator X   Use X instead of " << defaultCxxGenerator << endl
        << endl
        ;
}


/** Parse main's argv into Bundle.
 *
 * @returns < 0 on success; >= 0 on failure.
 */
static int options(int argc, char** argv, Bundle& b)
{
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
        else if (arg == "--default-cxx-generator") {
            const char* value = next(i, argc, argv);
            if (value == nullptr)
                return Ex_Usage;
            defaultCxxGenerator = value;
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

    return -1;
}


int main(int argc, char* argv[])
{
    Bundle b;

    b.debug = false;

    b.sourcedir = ".";
    b.distdir = "dist";
    b.builddir = "build";
    b.distribution = defaultDistribution();;
    b.project = {};
    b.inputpath = "ngen.json";
    b.outputpath = "build.ninja";

    /* Parse options into bundle. */
    int rc = options(argc, argv, b);
    if (rc >= 0)
        return rc;

    if (!b.directory.empty()) {
        if (!cd(b.directory)) {
            std::clog << b.argv[0] << ": failed to change directory to " << b.directory << std::strerror(errno) << endl;
        } else if (b.debug) {
            std::clog << "chdir " << b.directory << endl;
        }
    }

    rc = parse(b);
    if (rc >= 0) {
        std::clog << b.argv[0] << ": error parsing " << b.inputpath << endl;
        return rc;
    }

    /* Only set if no -G ... */
    if (b.generatorname.empty())
        b.generatorname = defaultGenerator(b);

    logBundle(std::clog, b, "DEBUG");

    if (b.project.empty()) {
        std::cout << b.argv[0] << ": nothing to do." << endl;
        return 0;
    }

    try {
        if (b.debug)
            std::clog << "generating " << b.project.at("project") << endl;

        b.generator = makeGenerator(b.generatorname, b);

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

