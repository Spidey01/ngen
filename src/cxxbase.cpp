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

#include "cxxbase.hpp"

#include "Bundle.hpp"
#include "Statement.hpp"
#include "path.hpp"
#include "util.hpp"

using std::endl;

cxxbase::cxxbase(Bundle& bundle)
    : Shinobi(bundle)
{
}


bool cxxbase::generateVariables(const json& project)
{
    if (!Shinobi::generateVariables(project))
        return false;

    /*
     * Build the primary vars. E.g. cxxflags = ... $generatornae_cxxflags ...
     */

    string n = generatorName();

    output()
        << "# flags for preprocessing C/C++ sources." << endl
        << "cppflags = $" << n << "_cppflags" << endl
        << "# flags for compiling C objects." << endl
        << "cflags = $" << n << "_cflags" << endl
        << "# flags for compiling C++ objects." << endl
        << "cxxflags = $" << n << "_cxxflags" << endl
        << "# flags for linking C/C++ applications and libraries." << endl
        << "ldflags = $" << n << "_ldflags" << endl
        << "# libraries to link with." << endl
        << "ldlibs = $" << n << "_ldlibs" << endl
        << endl
        ;

    return true;
}


bool cxxbase::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForObjects(project, type, rule)) {
        return false;
    }

    /*
     * If we have deps, add them here. So a dependencies entry for 'some other
     * project' is a req for compiling our objects. Otherwise headers might not
     * be installed. Etc.
     */

    auto deps = json::array({});
    if (has(project, "dependencies")) {
        deps = project.at("dependencies");
    }


    for (const string& source : project.at("sources")) {
        Statement build(rule);

        build.appendInput(sourcedir(source));
        build.appendOutput(object(source));
        build.appendOrderOnlyDependencies(deps);

        output() << build << endl;
    }

    return true;
}


bool cxxbase::generateBuildStatementsForApplication(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForApplication(project, type, rule)) {
        return false;
    }

    Statement build(rule);

    build.appendInputs(objects(project));

    string base_exe = "$bindir/" + targetName() + applicationExtension();
    string build_exe = builddir(base_exe);
    string install_exe = distdir(base_exe);

    build.appendOutput(build_exe);

    output() << build << endl;

    return true;
}


bool cxxbase::generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForLibrary(project, type, rule)) {
        return false;
    }

    Statement build(rule);

    build.appendInputs(objects(project));

    string base_lib = libraryBase();
    string build_lib = builddir(base_lib);

    build.appendOutput(build_lib);

    build.appendImplicitOutputs(implicitOutputsForLibrary(project, type, rule));

    if (has(project, "dependencies")) {
        build.appendDependencies(project.at("dependencies"));
    }

    output() << build << endl;

    return true;
}


bool cxxbase::generateBuildStatementsForInstall(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForInstall(project, type, rule)) {
        return false;
    }

    Statement install("install");

    if (isApplicationType(type)) {
        string base_exe = "$bindir/" + targetName() + applicationExtension();
        string build_exe = builddir(base_exe);
        string install_exe = distdir(base_exe);
        install
            .appendInput(build_exe)
            .appendOutput(install_exe)
            ;
    } else if (isLibraryType(type)) {
        string base_lib = libraryBase();
        string build_lib = builddir(base_lib);
        string dist_lib = distdir(base_lib);
        install
            .appendInput(build_lib)
            .appendOutput(dist_lib)
            ;

        for (const string& hdr : headers()) {
            string out = header(hdr);

            /*
             * rule install = copy and set executable.
             * rule copy = just copy it.
             */
            Statement install_header("copy");

            install_header
                .appendInput(hdr)
                .appendOutput(out)
                ;

            output() << install_header << endl;
        }

    }

    output() << install << endl;



    return true;
}


bool cxxbase::generateBuildStatementsForTargetName(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForTargetName(project, type, rule)) {
        return false;
    }


    Statement all(rule);

    if (isApplicationType(type)) {
        string install_exe = distdir(executableBase());
        all
            .appendInput(install_exe)
            .appendOutput(sourcedir(""))
            ;
    } else if (isLibraryType(type)) {
        string dist_lib = distdir(libraryBase());
        all
            .appendInput(dist_lib)
            .appendOutput(targetName())
            ;
    }
    all.appendInputs(extraInputsForTargetName(project, type, rule));

    output() << endl << all << endl;


    return true;
}


bool cxxbase::isSupportedType(const string& type) const
{
    if (type.find("c_") != 0 && type.find("cxx_") != 0) {
        warning() << "cxxbase backend does not support " << type << endl;
        return false;
    }

    return true;
}


cxxbase::list cxxbase::extraInputsForTargetName(const json& project, const string& type, const string& rule)
{
    list r = Shinobi::extraInputsForTargetName(project, type, rule);

    if (isLibraryType(type)) {
        for (const string& in : headers()) {
            r.push_back(header(in));
        }
    }

    return r;
}


cxxbase::string cxxbase::object(const string& source) const
{
    return builddir(replace_extension(source, objectExtension()));
}


cxxbase::list cxxbase::objects(const json& project) const
{
    cxxbase::list objs;

    for (const string& source : project.at("sources")) {
        objs.push_back(object(source));
    }

    return objs;
}


cxxbase::string cxxbase::executableBase() const
{
    return "$bindir/" + targetName() + applicationExtension();
}


cxxbase::string cxxbase::libraryBase() const
{
    string base = "$libdir/";

    /* targetName 'libfoo' shouldn't become 'liblibfoo.so' */
    if (targetName().find(libraryPrefix()) != 0)
        base.append(libraryPrefix());

    base.append(targetName())
        .append(libraryExtension())
        ;

    return base;
}


cxxbase::list cxxbase::headers() const
{
    const json& project = projectData();

    if (!has(project, "headers"))
        return {};

    list r;

    for (const string& header : project.at("headers")) {
        /* Need the actual for ls, but $sourcedir in the rule. */
        string top = bundle().sourcedir;
        string source = top + "/" + header;

        for (const string& hdr : ls(source, true)) {
            if (debug())
                log() << "hdr: " << hdr << endl;
            string base = hdr.substr(top.size() + 1);
            string input = "$sourcedir/" + base;
            if (debug()) {
                log()
                    << "input: " << input
                    << endl
                    << "base: " << base
                    << endl;
            }

            r.push_back(input);
        }
    }

    return r;
}


cxxbase::string cxxbase::header(const string& hdr) const
{
    const json& project = projectData();

    /*
     * headers_strip_prefix is so you can do headers entries like
     * 'include/myproject' and get $includedir/myproject instead of
     * $includedir/include/myproject.
     */

    string headers_strip_prefix;
    if (has(project, "headers_strip_prefix")) {
        headers_strip_prefix = project.at("headers_strip_prefix");
    }

    // assumes $sourcedir/ + ...
    string base = hdr.substr(11);

    string output = distdir("$includedir/");
    output.append(base.substr(headers_strip_prefix.size() + 1));
    if (debug())
        log() << "output: " << output << endl;

    return output;
}

