#ifndef NGEN_UTIL__HPP
#define NGEN_UTIL__HPP
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

/*
 * Utility functions
 */

#include "Shinobi.hpp"

#include <string>
#include <nlohmann/json.hpp>
#include <vector>


struct Bundle;

/* Like sysexits.h on BSD. */
constexpr int Ex_Usage = 64;
constexpr int Ex_DataErr = 65;
constexpr int Ex_NoInput = 66;
constexpr int Ex_CantCreate = 73;


/** Returns if obj has named field.
 */
bool has(const nlohmann::json& obj, const std::string& field);

void logBundle(std::ostream& log, const Bundle& b, const std::string& header);

std::string pwd();
bool cd(const std::string& where);

/** Lists a directory.
 *
 * Note that given a path like /foo/bar, on Windows you will get back
 * /foo/bar\\ham\\result. Not all software loves that, much as not all MS
 * software likes /.
 *
 * @param path root to ls.
 * @param recurse, repalce directory entries with their file contents.
 *
 * @returns resulting list of path, or all <em>files</em> recusively in path.
 */
std::vector<std::string> ls(const std::string& path, bool recurse);

/** Handle parsing data into the bundle's fields.
 *
 * @returns < 0 on success; >= 0 on failure.
 */
int parse(Bundle& b);

/** Returns the default generator name for project type.
 */
std::string defaultGenerator(const Bundle& bundle);
std::string defaultGenerator(const std::string& type);

/** Returns the Shinobi dispenser for name.
 */
Shinobi::unique_ptr makeGenerator(const std::string& name, Bundle& bundle);

/** Returns the default "distribution" structure.
 */
nlohmann::json defaultDistribution();

/** Returns the sort order for defaultDistribution
 *
 * E.g. an array of keys into defaultDistribution in sorted order to be processed.
 */
nlohmann::json sortedDistributionKeys();

#endif // NGEN_UTIL__HPP
