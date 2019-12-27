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

#include "cmake.hpp"

#include "Statement.hpp"
#include "path.hpp"

using std::endl;
using std::quoted;

cmake::cmake(Bundle& bundle)
    : Shinobi(bundle)
{
}


cmake::string cmake::generatorName() const
{
    return "cmake";
}


bool cmake::generateVariables(const json& project)
{
    if (!Shinobi::generateVariables(project))
        return false;

    /*
     * Convert /project/cmake/foo into -Dfoo.
     */

    output() << "# extra flags passed to cmake." << endl;
    output() << "cmake_flags =";

    if (has(project, generatorName())) {

        const json& flags = project.at(generatorName());

        for(auto it=flags.cbegin(); it != flags.cend(); ++it) {
            output() << " -D" << it.key() << "=" << quoted(it.value().get<string>());
        }

        output() << endl;
    }

    output() << endl;

    return true;
}


bool cmake::generateRules()
{
    if (!Shinobi::generateRules())
        return false;

    static const char* indent = "    ";

    output()
        << "# run cd $(dirname $in) && nmake -f $(basename $in)" << endl
        << "rule cmake" << endl
        << indent << "description = cmake $in" << endl
        << indent << "generator = true" << endl
        << indent << "pool = console" << endl
        << indent << "command = cmake "
            << " -G Ninja"
            << " -S " << quoted("$sourcedir")
            << " -B " << quoted("$builddir")
            << " " << quoted("-DCMAKE_INSTALL_PREFIX=$distdir")
            << " $cmake_flags"
            << endl
        ;

    output() << endl ;

    /* Shinobi provides a ninja rule by default. So no need to make one here. */

    return true;
}


bool cmake::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForObjects(project, type, rule))
        return false;

    Statement gen(rule);

    gen
        .appendInput(cmakelists_txt())
        .appendOutput(builddir("build.ninja"))
        ;

    output() << gen << endl;

    return true;
}


bool cmake::generateBuildStatementsForTargetName(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForTargetName(project, type, rule))
        return false;

    string ninja = builddir("build.ninja");

    Statement build("ninja");

    build
        .appendInput(ninja)
        .appendOutput(targetName())
        .appendVariable("ninja_targets", "install")
        ;

    output() << build << endl;

    return true;
}


cmake::string cmake::cmakelists_txt() const
{
    /*
     * We check /project/sources because Shinobi::generateProject() requires it
     * to be non empty.  We don't support multiple cmake's from one ngen
     * project. If you want that, you still need a top level cmake like the
     * rest of the world uses.
     */

    const json& project = projectData();

    if (project.at("sources").size() > 1) {
        warning() << "sources defines > 1 CMakeLists.txt files -- only one is supported!" << endl;
    }

    return sourcedir(project.at("sources").at(0).get<string>());
}

