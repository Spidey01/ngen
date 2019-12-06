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

extern "C" {
#if defined(_WIN32)
#include <direct.h>
#ifndef chdir
#define chdir _chdir
#endif
#ifndef getcwd
#define getcwd _getcwd
#endif
#if defined(_MSC_VER)
/*
 * strerror() -> strerror_s() warning from /W4.
 */
#pragma warning(disable : 4996)
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


void logBundle(std::ostream& log, const Bundle& b)
{
    if (!b.debug)
        return;
    log << "DEBUG:" << endl;
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


