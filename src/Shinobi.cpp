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
#include "Statement.hpp"
#include "util.hpp"

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
        { "cxx_library", "cxx_compile" },
        { "cs_application", "cs_compile" },
        { "java_application", "java_compile" },
        { "java_library", "java_compile" },
        { "package", "phony" },
        { "external", "exec" },
        { "cmake", "cmake" },
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
        { "package", "phony" },
        { "external", "exec" },
        { "cmake", "ninja" },
    })
{
}


bool Shinobi::generate()
{
    Bundle& b = mBundle;

    if (b.outputpath == "-") {
        // XXX using cout would be nice.
    } else {
        b.output.open(b.outputpath, b.output.out | b.output.trunc);
    }
    if (!b.output) {
        log() << b.argv[0] << ": cannot create " << b.outputpath << endl;
        return false;
    }

    if (!generateProject(b.project)) {
        log() << "generateProject failed for project " << projectName() << endl;
        return false;
    }

    return true;
}


bool Shinobi::generateProject(const json& project)
{
    if (debug()) {
        log() << "generateProject()" << endl;
        log() << "project: ";
        if (has(project, "project")) {
            log() << " name: " << projectName();
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
        log() << "project " << projectName() << " has type " << projectType() << endl;

    if (debug())
        log() << "generatorName: " << generatorName() << endl;

    if (!has(project, "sources")) {
        if (debug())
            log() << "nothing to do for " << projectName() << endl;
        return true;
    }

    /*
     * builddir is a ninja variable that defaults to .
     *
     * The rest our Terry Poulin's taste in solving the source -> build -> dist
     * relationship.
     */

    output()
        << "# Generated by ngen {TODO have a version} backend " << generatorName() << " on {TODO give date/time}" << endl
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

    string type = projectType();

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
    
    if (isApplicationType(type)) {
        if (!generateBuildStatementsForApplication(project, type, rule)) {
            error() << "failed to generate build statements for applications." << endl;
        }
    } else if (isLibraryType(type)) {
        if (!generateBuildStatementsForLibrary(project, type, rule)) {
            error() << "failed to generate build statements for libraries." << endl;
        }
    } else if (type.rfind("external") != string::npos) {
        if (!generateBuildStatementsForExternal(project, type, rule)) {
            error() << "failed to generate build statemetns for externals." << endl;
        }
    } else if (type == "package") {
        if (!generateBuildStatementsForPackage(project, type, rule)) {
            error() << "failed to generate build statements for packages." << endl;
        }
    } else if (type == "cmake") {
        /*
         * No hook needed, yet. *ForCMake() would do what cmake::*ForTargetName() does.
         */
    } else {
        log() << "TODO: " << type << endl;
    }

    rule = "install";
    if (!generateBuildStatementsForInstall(project, type, rule)) {
        error() << "failed to generate build statements for install." << endl;
    }

    rule = "phony";
    if (!generateBuildStatementsForTargetName(project, type, rule)) {
        error() << "failed to generate build statements for targetName." << endl;
    }

    return true;
}


bool Shinobi::generateVariables(const json& project)
{
    if (debug())
        log() << "generateVariables(): project: " << projectName() << endl;

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

    string version = "0";
    if (has(project, "version"))
        version = project.at("version");
    output() << projectName() << "_version = " << version << endl << endl;

    output() << "# vars controlling builddir/distdir structure" << endl;
    if (!mBundle.distribution.empty()) {
        if (debug())
            log() << "Setting distribution vars" << endl;

        /*
         * /project/distribution can override any of these from the defaults in the bundle.
         *
         * The sort order is important because ninja isn't as l^Hcrazy as make.
         */

        const json& dist = mBundle.distribution;
        for (const string& key : sortedDistributionKeys()) {
            string value = dist.at(key);

            if (has(project, "distribution") && has(project.at("distribution"), key))
                value = project.at("distribution").at(key);

            output() << key << " = " << value << endl;
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

        /*
         * Make sure there's a {generatorName}_{targetName} variable.  So
         * regardless of /{backend}/targetName there will be one to initialize
         * a targetName variable below.
         */

        if (!has(flags, "targetName"))
            output() << generatorName() << '_' << "targetName = " << targetName() << endl;
    }
    output() << endl;

    output()
        << "targetName = $" << generatorName() << "_targetName" << endl
        << endl
        ;

    if (has(projectData(), "variables")) {
        output()
            << "# Variables from the /variables block. Exported for children of this package." << endl
            << endl
            ;
        for (const json& line : projectData().at("variables")) {
                output() << line.get<string>() << endl;
        }
    }

    return true;
}


bool Shinobi::generateRules()
{
    if (debug())
        log() << "generateRules()" << endl;

    output()
        << "# run cd $(dirname $in) && ./$(basename $in) ..." << endl
        << "rule exec" << endl
        << "    description = exec $in" << endl
        << "    restat = true" << endl
        << "    pool = console" << endl
        ;
#if defined(_WIN32) || defined(__WIN64)
    output() << "    command = cmd /C @FOR /F " << quoted("delims=") << " %i IN ("
        << quoted("$in") << ") DO ( @FOR /F " << quoted("delims=") << " %j IN ("
        << quoted("%i") << ") DO ( @CD %~pi && %~nxj $args ) )"
        << endl
        ;
#else
    output() << "    command = (cd $$(dirname $in) && ./$$(basename $in) $args)" << endl;
#endif

    output()
        << "# run ninja -C $(dirname $in) -f $(basename $in)" << endl
        << "rule ninja" << endl
        << "    description = ninja $in" << endl
        << "    restat = true" << endl
        << "    pool = console" << endl
        ;
#if defined(_WIN32) || defined(__WIN64)
    output() << "    command = cmd /C FOR /F " << quoted("delims=") << " %i IN ("
        << quoted("$in") << ") DO ( FOR /F " << quoted("delims=") << " %j IN ("
        << quoted("%i") << ") DO ( ninja -C %~pi -f %~nxj $ninja_flags $ninja_targets ) )"
        << endl
        ;
#else
    output() << "    command = ninja -C $$(dirname $in) -f $$(basename $in) $ninja_flags $ninja_targets" << endl;
#endif

    output() << endl ;

    return true;
}


bool Shinobi::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    (void)project;
    if (debug())
        log() << "generateBuildStatementsForObjects(): project: " << projectName() << " type: " << type << " rule: " << rule << endl;
    return true;
}


bool Shinobi::generateBuildStatementsForApplication(const json& project, const string& type, const string& rule)
{
    (void)project;
    if (debug())
        log() << "generateBuildStatementsForApplication(): project: " << projectName() << " type: " << type << " rule: " << rule << endl;
    return true;
}


bool Shinobi::generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule)
{
    (void)project;
    if (debug())
        log() << "generateBuildStatementsForLibrary(): project: " << projectName() << " type: " << type << " rule: " << rule << endl;
    return true;
}


bool Shinobi::generateBuildStatementsForInstall(const json& project, const string& type, const string& rule)
{
    (void)project;
    if (debug())
        log() << "generateBuildStatementsForInstall(): project: " << projectName() << " type: " << type << " rule: " << rule << endl;

    if (has(project, "install_files")) {
        for (const json& obj : project.at("install_files")) {
            string input = sourcedir(obj.at("input"));
            string output = distdir(obj.at("output"));

            bool exe = has(obj, "executable") ? obj.at("executable").get<bool>() : false;

            Statement install_file(exe ? rule : "copy");

            install_file
                .appendInput(input)
                .appendOutput(output)
                ;

            this->output() << endl << install_file << endl;
        }
    }

    return true;
}


bool Shinobi::generateBuildStatementsForTargetName(const json& project, const string& type, const string& rule)
{
    (void)project;
    if (debug())
        log() << "generateBuildStatementsForTargetName(): project: " << projectName() << " type: " << type << " rule: " << rule << endl;
    return true;
}


bool Shinobi::generateBuildStatementsForExternal(const json& project, const string& type, const string& rule)
{
    (void)project;
    if (debug())
        log() << "generateBuildStatementsForExternal(): project: " << projectName() << " type: " << type << " rule: " << rule << endl;
    return true;
}


bool Shinobi::generateBuildStatementsForPackage(const json& project, const string& type, const string& rule)
{
    (void)project;
    if (debug())
        log() << "generateBuildStatementsForPackage(): project: " << projectName() << " type: " << type << " rule: " << rule << endl;
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


Shinobi::string Shinobi::targetName() const
{
    if (has(projectData(), generatorName()) && has(projectData().at(generatorName()), "targetName"))
        return projectData().at(generatorName()).at("targetName");
    return projectName();
}


Shinobi::string Shinobi::projectName() const
{
    return mBundle.project.at("project");
}


Shinobi::string Shinobi::projectType() const
{
    return mBundle.project.at("type");
}


const Shinobi::json& Shinobi::projectData() const
{
    return mBundle.project;
}


bool Shinobi::has(const json& obj, const string& field)
{
    return obj.find(field) != obj.cend();
}


const Bundle& Shinobi::bundle() const
{
    return mBundle;
}


bool Shinobi::debug() const
{
    return mBundle.debug;
}


std::ostream& Shinobi::error() const
{
    log() << "error: project:" << projectName() << ": ";

    return log();
}


std::ostream& Shinobi::warning() const
{
    log() << "warning: project:" << projectName() << ": ";

    return log();
}

std::ostream& Shinobi::output()
{
    return mBundle.output;
}


Shinobi::string Shinobi::sourcedir(const string& source) const
{
    return "$sourcedir/" + source;
}


Shinobi::string Shinobi::builddir(const string& source) const
{
    return "$builddir/" + source;
}


Shinobi::string Shinobi::distdir(const string& source) const
{
    return "$distdir/" + source;
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


bool Shinobi::isApplicationType(const string& type) const
{
    return type.rfind("_application") != string::npos;
}


bool Shinobi::isLibraryType(const string& type) const
{
    return type.rfind("_library") != string::npos;
}


Shinobi::list Shinobi::implicitOutputsForLibrary(const json& project, const string& type, const string& rule)
{
    (void)project;
    (void)type;
    (void)rule;
    return list{};
}


Shinobi::list Shinobi::extraInputsForTargetName(const json& project, const string& type, const string& rule)
{
    (void)type;
    (void)rule;

    if (!has(project, "install_files"))
        return list{};

    list r;

    for (const json& obj : project.at("install_files")) {
        r.push_back(distdir(obj.at("output").get<string>()));
    }

    return r;
}

