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
        json& dist = bundle.distribution;
        dist.at("library") = "$exec_prefix/lib";
        dist.at("libdir") = "$exec_prefix/" + dist.at("runtime").get<string>();

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
        << "pdb = " << targetName() << ".pdb" << endl
        << endl
        ;

    output()
        << "# import library and export file when making a dll." << endl
        << "implib = " << targetName() << ".lib" << endl
        << "exp = " << targetName() << ".exp" << endl
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
        << indent << "generator = true" << endl
        << indent << "restat = true" << endl
        << indent << "pool = console" << endl
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
        << indent << "command = Powershell Copy-Item -Force -Path $in -Destination $out" << endl
        << endl
        ;

    output()
        << "# install non file" << endl
        << "rule copy" << endl
        << indent << "description = install $out" << endl
        << indent << "command = Powershell Copy-Item -Force -Path $in -Destination $out" << endl 
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
        << indent << "command = $cc /nologo /Fd$builddir/$pdb /Fe$out $in $ldflags $ldlibs" << endl
        << endl
        ;
    output()
        << "# link *.obj -> *.dll" << endl
        << "rule c_library" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cc /nologo /LD /Fd$builddir/$pdb /Fe$out $in $ldflags $ldlibs " << endl
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
        << indent << "command = $cxx /nologo /Fd$builddir/$pdb /Fe$out $in $ldflags $ldlibs" << endl
        << endl
        ;
    output()
        << "# link *.obj -> *.dll" << endl
        << "rule cxx_library" << endl
        << indent << "description = LD $in -> $out" << endl
        << indent << "command = $cxx /nologo /LD /Fd$builddir/$pdb /Fe$out $in $ldflags $ldlibs"
        << endl
        ;

    return true;
}


bool msvc::generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule)
{
    if (!cxxbase::generateBuildStatementsForLibrary(project, type, rule)) {
        return false;
    }


    string built_dll = builddir(libraryBase());
    string built_implib = "$builddir/$libdir/$implib";
    string dist_implib = "$distdir/$library/$implib";
    string built_exp = "$builddir/$libdir/$exp";
    string dist_exp = "$distdir/$library/$exp";

    /*
     * cxxbase will use our implicitOutputsForLibrary(), which handle the
     * implib/exp in builddir being tied to the .dll in builddir. It will also use our
     * implicitOutputsForLibraryInstall() for the distdir versions of same.
     *
     * We still need to have statements to install them, for the latter to
     * work.
     */

    Statement install_implib("install");
    install_implib
        .appendInput(built_implib)
        .appendOutput(dist_implib)
        ;

    output() << install_implib << endl;

    Statement install_exp("install");
    install_exp
        .appendInput(built_exp)
        .appendOutput(dist_exp)
        ;

    output() << install_exp << endl;

    return true;
}


msvc::list msvc::implicitOutputsForLibrary(const json& project, const string& type, const string& rule)
{
    cxxbase::implicitOutputsForLibrary(project, type, rule);

    return list{
        "$builddir/$libdir/$implib",
        "$builddir/$libdir/$exp",
    };
}


msvc::list msvc::extraInputsForTargetName(const json& project, const string& type, const string& rule)
{
    list r = cxxbase::extraInputsForTargetName(project, type, rule);

    if (isLibraryType(type)) {
        r.push_back("$distdir/$library/$implib");
        r.push_back("$distdir/$library/$exp");
    }

    return r;
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

