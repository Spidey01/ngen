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

#include "Statement.hpp"
#include "path.hpp"

using std::endl;

gcc::gcc(Bundle& bundle)
    : cxxbase(bundle)
{
}


gcc::string gcc::objectExtension() const
{
    /*
     * GCC is *.cpp -> .o.
     */
    return ".o";
}


gcc::string gcc::applicationExtension() const
{
    /*
     * We don't need no stinking dot exe.
     */
    return "";
}


gcc::string gcc::libraryExtension() const
{
    /*
     * Things get uppity if you're not prefixed lib, as well.
     */
    return ".so";
}
