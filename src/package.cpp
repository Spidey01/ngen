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

#include "Bundle.hpp"
#include "Statement.hpp"
#include "path.hpp"
#include "util.hpp"

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

        generateChildProject(source);

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


bool package::generateChildProject(const string& name)
{
    if (debug())
        log() << "generateChildProject(): name: " << name << endl;

    /*
     * Create a new bundle to represent the child project, based on our own details.
     */

    Bundle child;

    child.debug = bundle().debug;
    child.argv = bundle().argv;
    child.sourcedir = bundle().sourcedir + "/" + name;
    child.builddir = bundle().builddir + "/" + name;
    child.distdir = bundle().distdir;
    child.directory = bundle().directory;

    child.data = {
        { "distribution", bundle().data.at("distribution") },
        { "projects", json::array({}) }
    };

    /*
     * This should probably be an environment variable that defaults to
     * ngen.json. Rather than reusing -f foo.
     */

    child.inputpath = child.sourcedir + "/" + filename(bundle().inputpath);
    child.outputpath = child.sourcedir + "/build.ninja";

    int rc = parse(child);
    if (rc >= 0) {
        std::clog << child.argv[0] << ": error parsing " << child.inputpath << endl;
        return false;
    }

    logBundle(log(), child, "DEBUG CHILD BUNDLE FOR: " + name);

    child.generatorname = defaultGenerator(child);
    child.generator = makeGenerator(child.generatorname, child);

    return child.generator->generate();
}

