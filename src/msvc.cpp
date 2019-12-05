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

#include "msvc.hpp"

#include "Bundle.hpp"
#include "Statement.hpp"
#include "path.hpp"

using std::endl;
using std::quoted;

msvc::msvc(Bundle& bundle)
    : cxxbase(bundle)
{
    /*
     * We need to put .dll files in $runtime rather than $library, which is
     * what cxxbase and the rest of the world wants.
     */
    try {
        if (debug()) log() << "updating libdir" << endl;
        json& dist = bundle.data.at("distribution");
        dist.at("libdir") = dist.at("runtime");

    } catch (std::runtime_error& ex) {
        warning() << "Failed to update vars for msvc .dll/.lib handling: " << ex.what() << endl;
    }
}


msvc::string msvc::generatorName() const
{
    return "msvc";
}


bool msvc::generateVariables(const json& project)
{
    if (!cxxbase::generateVariables(project))
        return false;

    output()
        << "# Microsoft Visual C++ compiler." << endl
        << "cc  = cl.exe" << endl
        << "cxx = cl.exe" << endl
        << "make = nmake.exe" << endl
        << endl
        ;

    output()
        << "# program debug database filename." << endl
        << "pdb = " << project.at("project").get<string>() << ".pdb" << endl
        << endl
        ;

    return true;
}


bool msvc::generateRules()
{
    if (!cxxbase::generateRules())
        return false;

    static const char* indent = "    ";

    // TODO: add flags/goals vars.
    output()
        << "# run cd $(dirname $in) && nmake -f $(basename $in)" << endl
        << "rule make" << endl
        << indent << "description = make $in" << endl
        ;
    output() << "    command = cmd /C FOR /F " << quoted("delims=") << " %i IN ("
        << quoted("$in") << ") DO ( FOR /F " << quoted("delims=") << " %j IN ("
        << quoted("%i") << ") DO ( CD %~pi && $make /nologo -f %~nxj ) )"
        << endl
        ;
    output() << endl ;

    output()
        << "# install executable file" << endl
        << "rule install" << endl
        << indent << "description = install $out" << endl
        << indent << "command = Powershell Copy-Item -Path $in -Destination $out" << endl 
        << endl
        ;

    output()
        << "# install non file" << endl
        << "rule copy" << endl
        << indent << "description = install $out" << endl
        << indent << "command = Powershell Copy-Item -Path $in -Destination $out" << endl 
        << endl
        ;

    /* TODO's:
     * - Interface over hard coding the rules wanted.
     * - Ditto a Rule class like Statement.
     * - /FdMASTER_PDB_FILE for *_compile and *_link.
     * - handling the .lib/.exp file for .dll.
     * - When vars get set, add a "msvc_deps_prefix = Note: including file:".
     * - cmd /C COPY /B $in $out or similar.
     */

    /*
     * C programs
     */

    output()
        << "# compile *.c -> *.obj" << endl
        << "rule c_compile" << endl
        << indent << "description = CC $in -> $out" << endl
        << indent << "deps = msvc" << endl
        << indent << "command = $cc /showIncludes /nologo $cppflags $cflags /Fd$builddir/$pdb /Fo$out /c $in" << endl
        << endl
        ;

    // XXX: ldflags would usually be more applicable to running link than cl, and using cflags should be safe here.
    output()
        << "# link *.obj -> *.exe" << endl
        << "rule c_application" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cc /nologo $ldflags /Fd$builddir/$pdb /Fe$out $in $ldlibs" << endl
        << endl
        ;
    output()
        << "# link *.obj -> *.dll" << endl
        << "rule c_library" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cc /nologo $ldflags /LD /Fd$builddir/$pdb /Fe$out $in $ldlibs" << endl
        << endl
        ;

    /*
     * CXX programs
     */

    output()
        << "# compile *.cpp -> *.obj" << endl
        << "rule cxx_compile" << endl
        << indent << "description = CXX $in -> $out" << endl
        << indent << "deps = msvc" << endl
        << indent << "command = $cxx /showIncludes /nologo $cppflags $cxxflags /Fd$builddir/$pdb /Fo$out /c $in" << endl
        << endl
        ;

    // XXX: same note as c_application
    output()
        << "# link *.obj -> *.exe" << endl
        << "rule cxx_application" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cxx /nologo $ldflags /Fd$builddir/$pdb /Fe$out $in $ldlibs" << endl
        << endl
        ;
    output()
        << "# link *.obj -> *.dll" << endl
        << "rule cxx_library" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cxx /nologo $ldflags /LD /Fd$builddir/$pdb /Fe$out $in $ldlibs" << endl
        << endl
        ;

    return true;
}


msvc::string msvc::objectExtension() const
{
    /*
     * MSVC is *.cpp -> *.obj.
     *
     * Technically, also PDB file: but there should be one of those for
     * target incorporating this object.
     */

    return ".obj";
}


msvc::string msvc::applicationExtension() const
{
    return ".exe";
}


msvc::string msvc::libraryPrefix() const
{
    return "";
}


msvc::string msvc::libraryExtension() const
{
    /*
     * Typically you will have a:
     *
     *  .dll -> runtime link to run against.
     *  .lib -> import library to build against.
     *  .exp -> exported symbol stuff for build.
     */
    return ".dll";
}

