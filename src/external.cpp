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

#include "external.hpp"

#include "Statement.hpp"
#include "path.hpp"

using std::endl;
using std::quoted;

external::external(Bundle& bundle)
    : Shinobi(bundle)
{
}


external::string external::generatorName() const
{
    return "external";
}


bool external::generateVariables(const json& project)
{
    if (!Shinobi::generateVariables(project))
        return false;

    return true;
}


bool external::generateRules()
{
    if (!Shinobi::generateRules())
        return false;

    // static const char* indent = "    ";

    output() << endl ;

    return true;
}


bool external::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForObjects(project, type, rule)) {
        return false;
    }

    return true;
}


bool external::generateBuildStatementsForExternal(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForExternal(project, type, rule)) {
        return false;
    }

    return true;
}


external::string external::operatingSystemName() const
{
#if defined(_WIN32)
    return "windows";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown";
#endif
}


const external::json& external::operatingSystem() const
{
    string os = operatingSystemName();
    const json& p = projectData();

    if (!has(p, os)) {
        throw std::runtime_error(generatorName() + ": not supported on operating system: " + os);
    }

    return  p.at(os);
}

