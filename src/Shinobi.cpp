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

Shinobi::Shinobi(const Bundle& bundle)
    : mBundle(bundle)
{
}


bool Shinobi::generate()
{
    for (const json& project : projects()) {
        if (!generateProject(project)) {
            log() << "generateProject failed for project " << project.at("project") << std::endl;
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
            log() << std::endl;
        }
    }

    if (!has(project, "sources")) {
        if (debug())
            log() << "nothing to do for " << project.at("project") << std::endl;
        return true;
    }

    return true;
}


void Shinobi::failure(std::ostream& log)
{
    log << "error in shuriken avoidance algorithm." << std::endl;
}


std::ostream& Shinobi::log()
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


std::ostream& Shinobi::output()
{
    return (std::ostream&)(mBundle.output);
}

