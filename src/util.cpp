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

#include "util.hpp"

#include "Bundle.hpp"
#include "Shinobi.hpp"
#include "gcc.hpp"
#include "javac.hpp"
#include "msvc.hpp"
#include "package.hpp"


extern "C" {
#if defined(_WIN32)
#include <direct.h>
#ifndef chdir
#define chdir _chdir
#endif
#ifndef getcwd
#define getcwd _getcwd
#endif
#else
#include <unistd.h>
#include <sys/param.h>
#endif
}

using json = nlohmann::json;
using std::endl;
using std::string;
using std::to_string;


bool has(const json& obj, const string& field)
{
    return obj.find(field) != obj.cend();
}


void logBundle(std::ostream& log, const Bundle& b, const string& header)
{
    if (!b.debug)
        return;
    log << header << endl;
    log << "pwd: " << pwd() << endl;
    log << "argv:" << endl;
    for (size_t i=0; i < b.argv.size(); ++i)
        log << '\t' << "argv[" << i << "]: " << std::quoted(b.argv[i]) << endl;
    log
        << "sourcedir: " << b.sourcedir << endl
        << "builddir: " << b.builddir << endl
        << "distdir: " << b.distdir << endl
        << "input: " << b.inputpath << endl
        << "output: " << b.outputpath << endl
        << "directory: " << b.directory << endl
        << "generator: " << b.generatorname << endl
        << "data: " << b.data.dump(4) << endl
        << endl;
}


string pwd()
{
    char* p = getcwd(nullptr, 0);

    string r;
    if (p != nullptr)
        r = p;

    free(p);

    return r;
}


bool cd(const string& where)
{
    return chdir(where.c_str()) == 0;
}


int parse(Bundle& b)
{
    if (b.debug)
        std::clog << "parse() b.inputpath: " << b.inputpath << endl;

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

        if (b.debug)
            std::clog << "projects push_back " << temp.at("project") << endl;
        b.data.at("projects").push_back(temp);
    } catch (std::exception& ex) {
        std::clog << b.argv[0] << ":error:" << b.inputpath << ": " << ex.what() << endl;
        return Ex_DataErr;
    }

    b.input.close();
    if (b.debug)
        std::clog << "parse() b.inputpath: " << b.inputpath << " return -1/ok" << endl;
    return -1;
}


string defaultGenerator(const Bundle& bundle)
{
   string type = bundle.data.at("projects").at(0).at("type");
   return defaultGenerator(type);
}


string defaultGenerator(const string& type)
{
#if defined(_WIN32)
    string cxx_def = "msvc";
#else
    string cxx_def = "gcc";
#endif

    string gen;

    try {
        if (type.find("c_") == 0)
            gen = cxx_def;
        else if (type.find("cxx_") == 0)
            gen = cxx_def;
        else if (type.find("java_") == 0)
            gen = "javac";
        else
            gen = "package";
    } catch (...) {
    }

    return gen;
}


Shinobi::unique_ptr makeGenerator(const string& name, Bundle& bundle)
{
    if (name == "package") {
        return std::make_unique<package>(bundle);
    } else if (name == "msvc") {
        return std::make_unique<msvc>(bundle);
    } else if (name == "gcc") {
        return std::make_unique<gcc>(bundle);
    } else if (name == "javac") {
        return std::make_unique<javac>(bundle);
    }

    return nullptr;
}

