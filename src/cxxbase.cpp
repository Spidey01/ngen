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

#include "Statement.hpp"
#include "path.hpp"

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

        build.appendInput(source);
        build.appendOutput(object(source));

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

    string base_exe = "$bindir/" + project.at("project").get<string>() + applicationExtension();
    string build_exe = "$builddir/" + base_exe;
    string install_exe = "$distdir/" + base_exe;

    build.appendOutput(build_exe);

    output() << build << endl;

    Statement install("install");
    install
        .appendInput(build_exe)
        .appendOutput(install_exe)
        ;
    output() << install << endl;

    return true;
}


bool cxxbase::generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForLibrary(project, type, rule)) {
        return false;
    }

    Statement build(rule);

    build.appendInputs(objects(project));

    string base_lib = "$libdir/" + libraryPrefix() + project.at("project").get<string>() + libraryExtension();
    string build_lib = "$builddir/" + base_lib;
    string dist_lib = "$distdir/" + base_lib;

    build.appendOutput(build_lib);

    output() << build << endl;

    Statement install("install");

    install
        .appendInput(build_lib)
        .appendOutput(dist_lib)
        ;

    output() << install << endl;

    return true;
}


bool cxxbase::isSupportedType(const string& type) const
{
    if (type.find("c_") != 0 && type.find("cxx_") != 0) {
        warning() << "cxxbase backend does not support " << type;
        return false;
    }

    return true;
}

cxxbase::string cxxbase::object(const string& source) const
{
    return "$builddir/" + replace_extension(source, objectExtension());
}


cxxbase::list cxxbase::objects(const json& project) const
{
    cxxbase::list objs;

    for (const string& source : project.at("sources")) {
        objs.push_back(object(source));
    }

    return objs;
}
