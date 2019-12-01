#ifndef NGEN_PATH__HPP
#define NGEN_PATH__HPP
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

#include <string>

/*
 * If C++17 is available these are `return std::filesystem::path(path).*(...)`.
 *
 * Otherwise these are similar things that assume UNIX style / paths and much
 * laziness because I don't want to bootstrap boost::filesystem.
 */

std::string filename(const std::string& path);
std::string extension(const std::string& path);
std::string replace_extension(const std::string& path, const std::string& new_extension);

#endif // NGEN_PATH__HPP
