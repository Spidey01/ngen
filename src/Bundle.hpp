#ifndef NGEN_BUNDLE__HPP
#define NGEN_BUNDLE__HPP
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
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct Bundle
{
    bool debug;

    std::vector<std::string> argv;

    /** Root of the source tree.
     *
     * Where we find sources to build.
     */
    std::string sourcedir;

    /** Root of the build tree.
     *
     * Where we put objects built from sources.
     */
    std::string builddir;

    /** Root of the redistribution tree.
     *
     * Where we put artifacts to be zipped up and shared.
     */
    std::string distdir;

    /* Where to store stuff.
     *
     * When building package type, this will be shared by sub projects.
     */
    nlohmann::json distribution;

    /** The real important part.
     */
    nlohmann::json project;

    /** Input pathname
     *
     * Use '-' for stdin.
     */
    std::string inputpath;

    /** Input file stream.
     */
    std::ifstream input;

    /** Output pathname.
     */
    std::string outputpath;

    /** Output file stream.
     */
    std::ofstream output;

    /* Handle -C.
     *
     * Needs betterment, if you like tar.
     */
    std::string directory;

    /** Shinobi Backend name.
     */
    std::string generatorname;

    /** Ye who generates.
     */
    Shinobi::unique_ptr generator;
};

#endif // NGEN_BUNDLE__HPP
