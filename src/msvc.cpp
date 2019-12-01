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


bool msvc::generateProject(const json& project)
{
    if (!Shinobi::generateProject(project))
        return false;

    string type = project.at("type");

    /*
     * Configure the rule for making object files.
     */

    string rule = compileRule(type);

    if (rule.empty()) {
        warning() << "unsupported type: " << type << endl;
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

        log() << build << endl;
        output() << build << endl;
    }

    return false;
}


