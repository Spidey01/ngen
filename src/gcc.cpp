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

#include "gcc.hpp"
#include "path.hpp"

gcc::gcc(const Bundle& bundle)
    : Shinobi(bundle)
{
}


bool gcc::generateProject(const json& project)
{
    if (!Shinobi::generateProject(project))
        return false;

    /*
     * gcc is *.cpp -> *.o.
     */

    for (const string& source : project.at("sources")) {
        string input = source;
        string output = replace_extension(input, ".o");

        log() << "build " << output << ": cxx_compile " << input << std::endl;
        log() << " filename: " << filename(input) << " extension: " << extension(input) << std::endl;
    }
    
    return false;
}


