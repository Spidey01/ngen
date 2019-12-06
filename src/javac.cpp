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

#include "javac.hpp"

#include "Statement.hpp"
#include "path.hpp"

using std::endl;
using std::quoted;

javac::javac(Bundle& bundle)
    : Shinobi(bundle)
{
}


javac::string javac::generatorName() const
{
    return "javac";
}


bool javac::generateVariables(const json& project)
{
    if (!Shinobi::generateVariables(project))
        return false;

    output()
        << "# Assuming the normal Java Development Kit." << endl
        << "javac = javac" << endl
        << "jar = jar" << endl
        << endl
        ;

    return true;
}


bool javac::generateRules()
{
    if (!Shinobi::generateRules())
        return false;

    static const char* indent = "    ";

    /* TODO's:
     * - Interface over hard coding the rules wanted.
     * - Ditto a Rule class like Statement.
     * - install / cp / strip concerns.
     */

    output()
        << "# compile *.java -> *.class" << endl
        << "rule java_compile" << endl
        << indent << "description = javac $in -> $out" << endl
#if defined(_WIN32)
        << indent << "command = cmd /C FOR /F " << quoted("delims=") << " %i IN ("
            << quoted("$out") << ") DO ( FOR /F " << quoted("delims=") << " %j IN ("
            << quoted("%i") << ") DO ( $javac -d %~pi $in ) )"
        << endl
#else
        << indent << "command = $javac -d $(dirname $out) $in" << endl
#endif
        << endl
        ;

    output()
        << "# compile *.class -> *.jar" << endl
        << "rule java_library" << endl
        << indent << "description = jar $out <- $in" << endl
        << indent << "command = $jar cf $out $in" << endl
        << endl
        ;

    return true;
}


bool javac::generateBuildStatementsForObjects(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForObjects(project, type, rule)) {
        return false;
    }

    for (const string& source : project.at("sources")) {
        Statement build(rule);

        build.appendInput(sourcedir(source));
        build.appendOutput(klass(source));

        output() << build << endl;
    }

    return true;
}


bool javac::generateBuildStatementsForLibrary(const json& project, const string& type, const string& rule)
{
    if (!isSupportedType(type) || !Shinobi::generateBuildStatementsForLibrary(project, type, rule)) {
        return false;
    }

    Statement build(rule);

    string jar = builddir(project.at("project").get<string>() + ".jar");
    build.appendOutput(jar);

    for (const string& source : project.at("sources")) {
        build.appendInput(klass(source));
    }

    output() << build << endl;

    /*
     * Makes a handy target, and one that's expected by super projects.
     */

    Statement all("phony");

    all
        .appendInput(jar)
        .appendOutput(project.at("project"))
        ;

    output() << endl << all << endl;


    return true;
}


bool javac::isSupportedType(const string& type) const
{
    if (type.find("java_") != 0) {
        warning() << "javac backend does not support " << type;
        return false;
    }

    return true;
}


javac::string javac::klass(const string& source) const
{
    return builddir(replace_extension(source, ".class"));
}
