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

    for (const string& source : project.at("sources")) {
        Statement build(rule);

        build.appendInput(sourcedir(source));
        build.appendOutput(object(source));

        output() << build << endl;
    }

    /*
     * Each file, or directory in headers needs to be intalled.
     *
     * Generating multiple statements because install/copy commands under
     * Windows, mostly suck.
     */
    if (has(project, "headers")) {
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

                /*
                 * headers_strip_prefix is so you can do headers entries like
                 * 'include/myproject' and get $includedir/myproject instead of
                 * $includedir/include/myproject.
                 */

                string headers_strip_prefix;
                if (has(project, "headers_strip_prefix")) {
                    headers_strip_prefix = project.at("headers_strip_prefix");
                }

                string output = distdir("$includedir/");
                output.append(base.substr(headers_strip_prefix.size() + 1));
                if (debug())
                    log() << "output: " << output << endl;

                /*
                 * rule install = copy and set executable.
                 * rule copy = just copy it.
                 */
                Statement install_header("copy");

                install_header
                    .appendInput(input)
                    .appendOutput(output)
                ;

                this->output() << install_header << endl;
            }
        }
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

    string base_exe = "$bindir/" + projectName() + applicationExtension();
    string build_exe = builddir(base_exe);
    string install_exe = distdir(base_exe);

    build.appendOutput(build_exe);

    output() << build << endl;

    Statement install("install");
    install
        .appendInput(build_exe)
        .appendOutput(install_exe)
        ;
    output() << install << endl;

    /*
     * Makes a handy target, and one that's expected by super projects.
     */

    Statement all("phony");

    all
        .appendInput(install_exe)
        .appendOutput(sourcedir(""))
        ;

    output() << endl << all << endl;

    return true;
}


bool cxxbase::generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForLibrary(project, type, rule)) {
        return false;
    }

    Statement build(rule);

    build.appendInputs(objects(project));

    string base_lib = "$libdir/" + libraryPrefix() + projectName() + libraryExtension();
    string build_lib = builddir(base_lib);
    string dist_lib = distdir(base_lib);

    build.appendOutput(build_lib);

    output() << build << endl;

    Statement install("install");

    install
        .appendInput(build_lib)
        .appendOutput(dist_lib)
        ;

    output() << install << endl;

    /*
     * Makes a handy target, and one that's expected by super projects.
     */

    Statement all("phony");

    all
        .appendInput(dist_lib)
        .appendOutput(projectName())
        ;

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
