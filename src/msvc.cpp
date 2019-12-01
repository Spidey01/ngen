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

#include "msvc.hpp"

#include "Statement.hpp"
#include "path.hpp"

using std::endl;

msvc::msvc(Bundle& bundle)
    : Shinobi(bundle)
{
}


bool msvc::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForObjects(project, type, rule)) {
        return false;
    }

    /*
     * MSVC is *.cpp -> *.obj.
     *
     * Technically, also PDB file: but there should be one of those for
     * target incorporating this object.
     */

    for (const string& source : project.at("sources")) {
        Statement build(rule);

        build.appendInput(source);
        build.appendOutput(replace_extension(source, ".obj"));

        output() << build << endl;
    }

    return true;
}


bool msvc::generateBuildStatementsForApplication(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForApplication(project, type, rule)) {
        return false;
    }

    Statement build(rule);

    for (const string& source : project.at("sources")) {
        build.appendInput(replace_extension(source, ".obj"));
    }

    string exe = project.at("project").get<string>() + ".exe";
    build.appendOutput(exe);

    output() << build << endl;

    log() << build << endl;

    return true;
}


bool msvc::isSupportedType(const string& type) const
{
    if (type.find("c_") != 0 && type.find("cxx_") != 0) {
        warning() << "msvc backend does not support " << type;
        return false;
    }

    return true;
}

