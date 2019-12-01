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

    for (const string& source : project.at("sources")) {
        build.appendInput(object(source));
    }

    string exe = "$builddir/" + project.at("project").get<string>() + applicationExtension();
    build.appendOutput(exe);

    output() << build << endl;

    log() << build << endl;

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
