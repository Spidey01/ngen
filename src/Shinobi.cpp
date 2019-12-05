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

#include "Shinobi.hpp"
#include "Bundle.hpp"

#include <iostream>

using std::endl;
using std::quoted;

Shinobi::Shinobi(Bundle& bundle)
    : mBundle(bundle)
    , mProjectIndex(SIZE_MAX)
    , mCompileRules({
        { "c_application", "c_compile" },
        { "c_library", "c_compile" },
        { "cxx_application", "cxx_compile" },
        { "cxx_library", "c_compile" },
        { "cs_application", "cs_compile" },
        { "java_application", "java_compile" },
        { "java_library", "java_compile" },
    })
    , mLinkRules({
        { "c_application", "c_application" },
        { "c_library", "c_library" },
        { "cxx_application", "cxx_application" },
        { "cxx_library", "cxx_library" },
        { "cs_application", "cs_application" },
        { "cs_library", "cs_library" },
        { "java_application", "java_application" },
        { "java_library", "java_library" },
    })
{
}


bool Shinobi::generate()
{
    for (const json& project : projects()) {
        mProjectIndex += 1; // unsigned ftw.

        if (!generateProject(project)) {
            log() << "generateProject failed for project " << project.at("project") << endl;
            return false;
        }
    }

    return true;
}


bool Shinobi::generateProject(const json& project)
{
    if (debug()) {
        log() << "project: ";
        if (has(project, "project")) {
            log() << " name: " << project["project"];
            if (has(project, "version"))
                log() << " version: " << project["version"];
            log() << endl;
        }
    }

    if (!has(project, "type")) {
        error() << "project has no type set." << endl;
        return false;
    }
    if (debug())
        log() << "project " << project.at("project") << " has type " << project.at("type") << endl;

    if (!has(project, "sources")) {
        if (debug())
            log() << "nothing to do for " << project.at("project") << endl;
        return true;
    }

    /*
     * builddir is a ninja variable that defaults to .
     *
     * The rest our Terry Poulin's taste in solving the source -> build -> dist
     * relationship.
     */

    output()
        << "# Generated by ngen {TODO have a version} on {TODO give date/time}" << endl
        << endl
        ;

    if (!generateVariables(project)) {
        error() << "failed to generate variables" << endl;
        return false;
    }

    if (!generateRules()) {
        error() << "failed to generate rules" << endl;
        return false;
    }

    string type = project.at("type");

    /*
     * Configure the rule for making object files.
     */

    string rule = compileRule(type);

    if (rule.empty()) {
        warning() << "unsupported type: " << type << endl;
    }
    if (!generateBuildStatementsForObjects(project, type, rule)) {
        error() << "failed to generate build statements for objects." << endl;
        return false;
    }

    rule = linkRule(type);
    
    if (type.rfind("_application") != string::npos) {
        if (!generateBuildStatementsForApplication(project, type, rule)) {
            error() << "failed to generate build statements for applications." << endl;
        }
    } else if (type.rfind("_library") != string::npos) {
        if (!generateBuildStatementsForLibrary(project, type, rule)) {
            error() << "failed to generate build statements for libraries." << endl;
        }
    } else {
        log() << "TODO: " << type << endl;
    }

    return true;
}


bool Shinobi::generateVariables(const json& project)
{
    if (debug())
        log() << "generateVariables(): project: " << project.at("project") << endl;

    output()
        << "# where the sources can be found." << endl
        << "sourcedir = " << mBundle.sourcedir << endl
        << endl
        << "# where build artifacts go." << endl
        << "builddir = " << mBundle.builddir << endl
        << endl
        << "# where redistributable artifacts go." << endl
        << "distdir = " << mBundle.distdir << endl
        << endl
        ;

    output() << "# vars controlling builddir/distdir structure" << endl;
    if (has(mBundle.data, "distribution")) {
        if (debug())
            log() << "Setting distribution vars" << endl;
        const json& distribution = mBundle.data.at("distribution");
        for (auto it=distribution.cbegin(); it != distribution.cend(); ++it) {
            /*
             * /project/distribution can override any of these from the defaults in the bundle.
             */
            string value = it.value();

            if (has(project, "distribution") && has(project.at("distribution"), it.key()))
                value = project.at("distribution").at(it.key());

            output() << it.key() << " = " << value << endl;
        }
    } else if (debug()) {
            log() << "NOT Setting distribution vars" << endl;
    }
    output() << endl;

    if (has(project, generatorName())) {
        const json& flags = project.at(generatorName());

        for(auto it=flags.cbegin(); it != flags.cend(); ++it) {
            output() << generatorName() << "_" << it.key() << " =";

            if (flags.at(it.key()).is_array()) {
                for (const string& word : it.value()) {
                    output() << " " << word;
                }
            } else  {
                // assume string.
                output() << " " << it.value().get<string>();
            }

            output() << endl;
        }
    }
    output() << endl;

    return true;
}


bool Shinobi::generateRules()
{
    if (debug())
        log() << "generateRules()" << endl;

    output()
        << "# run ninja -C $(dirname $in) -f $(basename $in)" << endl
        << "rule ninja" << endl
        << "    description = ninja $in" << endl
        ;
#if defined(_WIN32) || defined(__WIN64)
    output() << "    command = cmd /C FOR /F " << quoted("delims=") << " %i IN ("
        << quoted("$in") << ") DO ( FOR /F " << quoted("delims=") << " %j IN ("
        << quoted("%i") << ") DO ( ninja -C %~pi -f %~nxj ) )"
        << endl
        ;
#else
    output() << "    command = ninja -C $$(dirname $in) -f $$(basename $in)" << endl;
#endif

    output() << endl ;

    return true;
}


bool Shinobi::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    if (debug())
        log() << "generateBuildStatementsForObjects(): project: " << project.at("project") << " type: " << type << " rule: " << rule << endl;
    return true;
}


bool Shinobi::generateBuildStatementsForApplication(const json& project, const string& type, const string& rule)
{
    if (debug())
        log() << "generateBuildStatementsForApplication(): project: " << project.at("project") << " type: " << type << " rule: " << rule << endl;
    return true;
}


bool Shinobi::generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule)
{
    if (debug())
        log() << "generateBuildStatementsForLibrary(): project: " << project.at("project") << " type: " << type << " rule: " << rule << endl;
    return true;
}


Shinobi::string Shinobi::generatorName() const
{
    return "Shinobi";
}


void Shinobi::failure(std::ostream& log)
{
    log << "error in shuriken avoidance algorithm." << endl;
}


std::ostream& Shinobi::log() const
{
    return std::clog;
}


const Shinobi::json& Shinobi::projects() const
{
    return mBundle.data.at("projects");
}


bool Shinobi::has(const json& obj, const string& field)
{
    return obj.find(field) != obj.cend();
}


bool Shinobi::debug() const
{
    return mBundle.debug;
}


const Shinobi::json& Shinobi::data() const
{
    return mBundle.data;
}


std::ostream& Shinobi::error() const
{
    log() << "error: ";

    if (mProjectIndex != SIZE_MAX)
        log() << "project:" << projects().at(mProjectIndex).at("project") << ": ";

    return log();
}


std::ostream& Shinobi::warning() const
{
    log() << "warning: ";

    if (mProjectIndex != SIZE_MAX)
        log() << "project:" << projects().at(mProjectIndex).at("project") << ": ";

    return log();
}

std::ostream& Shinobi::output()
{
    return mBundle.output;
}


Shinobi::string Shinobi::compileRule(const string& type) const
{
    auto it = mCompileRules.find(type);

    if (it == mCompileRules.cend())
        return "";

    return it->second;
}


Shinobi::string Shinobi::linkRule(const string& type) const
{
    auto it = mLinkRules.find(type);

    if (it == mLinkRules.cend())
        return "";

    return it->second;
}
