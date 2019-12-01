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

#include "path.hpp"

using std::string;

#if __cplusplus >= 201703L
#define HAVE_STD_FILESYSTEM 1
#elif defined(_MSC_VER) && ( _MSVC_LANG > 201402L && _MSC_VER >= 1915)
#define HAVE_STD_FILESYSTEM 1
#else
#define HAVE_STD_FILESYSTEM 0
#endif

#if HAVE_STD_FILESYSTEM
#include <filesystem>
#endif

string filename(const string& path)
{
#if HAVE_STD_FILESYSTEM
    return std::filesystem::path(path).filename().string();
#else
    auto slash = path.rfind("/");

    if (slash == string::npos)
        return path;

    return path.substr(slash + 1);
#endif
}

string extension(const string& path)
{
#if HAVE_STD_FILESYSTEM
    return std::filesystem::path(path).extension().string();
#else
    auto dot = path.rfind('.');

    if (dot == string::npos)
        return path;

    return path.substr(dot + 1);
#endif
}

string replace_extension(const string& path, const string& new_extension)
{
#if HAVE_STD_FILESYSTEM
    return std::filesystem::path(path).replace_extension(new_extension).string();
#else
    auto dot = path.rfind('.');

    if (dot == string::npos)
        return path;

    return path.substr(0, dot) + new_extension;
#endif
}

#ifdef HAVE_STD_FILESYSTEM
#undef HAVE_STD_FILESYSTEM
#endif
