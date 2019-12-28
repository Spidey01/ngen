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


bool external::generateBuildStatementsForExternal(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForExternal(project, type, rule))
        return false;

    if (debug())
        log() << "type: " << type << " rule: " << rule << endl;

    string args;
    for (const string& src : project.at("sources")) {
        if (!args.empty())
            args += ' ';
        args.append(src);
    }

    for (const string& cmd : operatingSystem()) {
        string script = sourcedir(cmd);

        Statement build(rule);

        build
            .appendInput(script)
            .appendOutput(targetName())
        .appendVariable("args", args)
            ;

        output() << build << endl;
    }

    return true;
}

bool external::generateBuildStatementsForTargetName(const json& project, const string& type, const string& rule)
{
    if (!Shinobi::generateBuildStatementsForTargetName(project, type, rule))
        return false;

    if (debug())
        log() << "type: " << type << " rule: " << rule << endl;

    Statement phony(rule);

    phony
        .appendOutput(sourcedir(""))
        .appendInput(targetName())
        ;

    output() << phony << endl;

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

    if (!has(p, "external")) {
        throw std::runtime_error(generatorName() + ": no external configuration given.");
    }

    const json& p_ext = p.at("external");

    if (!has(p_ext, os)) {
        throw std::runtime_error(generatorName() + ": not supported on operating system: " + os);
    }

    return  p_ext.at(os);
}

