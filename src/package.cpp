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

#include "package.hpp"

#include "Statement.hpp"
#include "path.hpp"

using std::endl;
using std::quoted;

package::package(Bundle& bundle)
    : Shinobi(bundle)
{
}


package::string package::generatorName() const
{
    return "package";
}

bool package::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForObjects(project, type, rule)) {
        return false;
    }

    /*
     * package type uses "sources" to list child project directories, rather
     * than source files.
     */

    for (const string& source : project.at("sources")) {

        /*
         * It's expected that each of these will generate a phony for 'source'.
         */

        output() << "subninja " << source << "/build.ninja" << endl;
    }

    return true;
}


bool package::generateBuildStatementsForPackage(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForLibrary(project, type, rule)) {
        return false;
    }

    Statement phony(rule);

    phony
        .appendOutput(project.at("project"))
        .appendInputs(project.at("sources"))
        ;

    output() << phony << endl;

    return true;
}
