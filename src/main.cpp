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

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using json = nlohmann::json;
using std::string;
using std::to_string;


int main(int argc, char* argv[])
{
    /*
     * Hard coding for now. Decide syntax later.
     */

    Bundle b;

    b.debug = true;

    for (int i=0; i < argc; ++i)
        b.argv.push_back(argv[i]);

    b.sourcedir = ".";
    b.builddir = "build";
    b.distdir = "dist";
    b.data = {
        { "projects", json::array({}) }
    };

    string recipe = "ngen.json";
    std::ifstream input(recipe);
    try {
        json temp;
        input >> temp;

        // for now assume one project object.
        b.data.at("projects").push_back(temp);
    } catch (std::exception& ex) {
        std::clog << b.argv[0] << ":error:" << recipe << ": " << ex.what();
    }

    if (b.debug) {
        std::clog << "DEBUG:" << std::endl;
        std::clog << "argv:" << std::endl;
        for (size_t i=0; i < b.argv.size(); ++i)
            std::clog << '\t' << "argv[" << i << "]: " << std::quoted(b.argv[i]) << std::endl;
        std::clog
            << '\t' << "sourcedir: " << b.sourcedir << std::endl
            << '\t' << "builddir: " << b.builddir << std::endl
            << '\t' << "distdir: " << b.distdir << std::endl
            << '\t' << "data: " << b.data.dump(4) << std::endl
            << std::endl;

        if (b.data.at("projects").empty()) {
            std::cout << b.argv[0] << ": nothing to do." << std::endl;
            return 0;
        }
    }

    /* Think this over, later. */
#if defined(_WIN32) || defined(__WIN64)
    b.generator = std::make_unique<msvc>(b);
#else
    b.generator = std::make_unique<gcc>(b);
#endif

    b.outputpath = "build.ninja";
    b.output.open(b.outputpath, b.output.out | b.output.trunc);

    if (!b.generator->generate()) {
        b.generator->failure(std::clog);
        std::clog << "What a Terrible Failure we has here." << std::endl;
    }

    return 0;
}
