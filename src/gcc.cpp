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


bool gcc::generateVariables()
{
    if (!cxxbase::generateVariables())
        return false;

    output()
        << "# GNU Compiler Collection." << endl
        << "cc  = gcc" << endl
        << "cxx = g++" << endl
        << endl
        ;

    return true;
}


bool gcc::generateRules()
{
    if (!cxxbase::generateRules())
        return false;

    static const char* indent = "    ";

    /* TODO's:
     * - Interface over hard coding the rules wanted.
     * - Ditto a Rule class like Statement.
     * - install / cp / strip concerns.
     */

    /*
     * C programs
     */

    output()
        << "# compile *.c -> *.o" << endl
        << "rule c_compile" << endl
        << indent << "description = CC $in -> $out" << endl
        << indent << "depfile = $out.d" << endl
        << indent << "deps = gcc" << endl
        << indent << "command = $cc $cppflags $cflags -o $out -c $in" << endl
        << endl
        ;

    output()
        << "# link *.o -> executable" << endl
        << "rule c_application" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cc $ldflags -o $out $in $ldlibs" << endl
        << endl
        ;
    output()
        << "# link *.o -> *.so" << endl
        << "rule c_library" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cc $ldflags -shared -o $out $in $ldlibs" << endl
        << endl
        ;

    /*
     * CXX programs
     */

    output()
        << "# compile *.cpp -> *.o" << endl
        << "rule cxx_compile" << endl
        << indent << "description = CXX $in -> $out" << endl
        << indent << "depfile = $out.d" << endl
        << indent << "deps = gcc" << endl
        << indent << "command = $cxx -MMD -MF $cppflags $cxxflags -o $out -c $in" << endl
        << endl
        ;

    // XXX: same note as c_application
    output()
        << "# link *.o -> executable" << endl
        << "rule cxx_application" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cxx $ldflags -o $out $in" << endl
        << endl
        ;
    output()
        << "# link *.o -> *.so" << endl
        << "rule cxx_library" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cxx $ldflags -shared -o $out $in" << endl
        << endl
        ;

    return true;
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
